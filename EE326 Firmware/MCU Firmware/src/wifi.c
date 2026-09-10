#include "wifi.h"
#include <stdio.h>
#include <stdint.h>
#include "conf_board.h"
#include "conf_clock.h"
#include "camera.h"

// ── Variable definitions (extern declared in wifi.h) ─────────────────────────
volatile char           input_line_wifi[MAX_INPUT_WIFI];
volatile uint32_t       received_byte_wifi = 0;
volatile bool           new_rx_wifi = false;
volatile unsigned int   input_pos_wifi = 0;
volatile bool           wifi_comm_success = false;
volatile uint32_t       transfer_index = 0;
volatile uint32_t       transfer_len = 0;
volatile uint32_t       WIFI_PROVISION_FLAG = 0;
volatile uint32_t       counts = 0;

// image buffer
//uint8_t im_buf[100000];


////////////////////////////////////////////////////
// handles incoming data from wifi
////////////////////////////////////////////////////
void WIFI_USART_HANDLER(void){
   	uint32_t ul_status;

	/* Read USART status. */
	ul_status = usart_get_status(WIFI_USART);

	/* Receive buffer is full. */
	if (ul_status & US_CSR_RXBUFF) {
		usart_read(WIFI_USART, &received_byte_wifi);
		new_rx_wifi = true;
		process_incoming_byte_wifi((uint8_t)received_byte_wifi);
	}
}

////////////////////////////////////////////////////
// handler for "command complete" rising edge interrupt
////////////////////////////////////////////////////
void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask){
    unused(ul_id);
	unused(ul_mask);
	
	wifi_comm_success = true;
	process_data_wifi();
	for (int jj=0;jj<MAX_INPUT_WIFI;jj++) input_line_wifi[jj] = 0;
	input_pos_wifi = 0;
}


////////////////////////////////////////////////////
// stores all incoming bytes from the ESP into a buffer
////////////////////////////////////////////////////
void process_incoming_byte_wifi(uint8_t in_byte){
    input_line_wifi[input_pos_wifi++] = in_byte;
}


////////////////////////////////////////////////////
// processes response of the ESP held in buffer
////////////////////////////////////////////////////
void process_data_wifi(void){
    if (strstr((char*)input_line_wifi, "SUCCESS")) {
        wifi_comm_success = true;
        gpio_configure_pin(WIFI_COMM_PIN_NUM, HIGH);
    }
}



////////////////////////////////////////////////////
// configure "command complete" rising edge interrupt
////////////////////////////////////////////////////
void configure_wifi_comm_pin(void){
    pmc_enable_periph_clk(WIFI_COMM_ID);

    pio_handler_set(WIFI_COMM_PIO, WIFI_COMM_ID, WIFI_COMM_PIN_NUM,
        WIFI_COMM_ATTR, wifi_command_response_handler);

    NVIC_EnableIRQ((IRQn_Type)WIFI_COMM_ID);
    pio_enable_interrupt(WIFI_COMM_PIO, WIFI_COMM_PIN_NUM);
}


////////////////////////////////////////////////////
// configure USART port to communicate with ESP
////////////////////////////////////////////////////
void configure_usart_wifi(void){
    gpio_configure_pin(PIN_USART0_RXD_IDX, PIN_USART0_RXD_FLAGS);
    gpio_configure_pin(PIN_USART0_TXD_IDX, PIN_USART0_TXD_FLAGS);

    const sam_usart_opt_t usart_console_settings = {
        WIFI_USART_BAUDRATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
    };

    /* Enable peripheral clock. */
	sysclk_enable_peripheral_clock(WIFI_USART_ID);

	/* Configure USART. */
	usart_init_rs232(WIFI_USART, &usart_console_settings, sysclk_get_peripheral_hz());

	/* Disable all the interrupts. */
	usart_disable_interrupt(WIFI_USART, ALL_INTERRUPT_MASK);
	
	/* Enable TX & RX function. */
	usart_enable_tx(WIFI_USART);
	usart_enable_rx(WIFI_USART);
	
	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(WIFI_USART_IRQn);
	
	usart_enable_interrupt(WIFI_USART, US_IER_RXRDY);
}


////////////////////////////////////////////////////
// writes command to ESP, waits for ack or timeout
// counts incremented by timer every second
////////////////////////////////////////////////////
void write_wifi_command(char* comm, uint8_t cnt){
    while(*comm) {
        usart_write(WIFI_USART, *comm++);
    }

    counts = 0;
    while (counts < cnt && !wifi_comm_success) {
    }
    wifi_comm_success = false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*SPI*/

////////////////////////////////////////////////////
// SPI interrupt handler — sends one image byte per SPI clock from ESP
// named SPI_Handler so hardware vector table routes to it correctly
////////////////////////////////////////////////////
void SPI_Handler(void)
{
	uint32_t new_cmd = 0;
	static uint16_t data;
	uint8_t uc_pcs;

	if (spi_read_status(SPI) & SPI_SR_RDRF) {
		spi_read(SPI, &data, &uc_pcs);
		
		if (transfer_len--) {
			spi_write(SPI, transfer_index++, 0, 0);
		}
	}
}


////////////////////////////////////////////////////
// configure SPI ports and interrupts
////////////////////////////////////////////////////
void configure_spi(void){
    gpio_configure_pin(SPI_MISO_GPIO, SPI_MISO_FLAGS);
    gpio_configure_pin(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
    gpio_configure_pin(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
    gpio_configure_pin(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);

    NVIC_DisableIRQ(SPI_IRQn);
    NVIC_ClearPendingIRQ(SPI_IRQn);
    NVIC_SetPriority(SPI_IRQn, 0);
    NVIC_EnableIRQ(SPI_IRQn);
}


////////////////////////////////////////////////////
// initialize SPI port as peripheral (slave)
////////////////////////////////////////////////////
void spi_peripheral_initialize(void){
    spi_enable_clock(SPI);
    spi_disable(SPI);
    spi_reset(SPI);
    spi_set_slave_mode(SPI);
    spi_disable_mode_fault_detect(SPI);
    spi_set_peripheral_chip_select_value(SPI, SPI_CHIP_PCS);
    spi_set_clock_polarity(SPI, SPI_CHIP_SEL, SPI_CLK_POLARITY);
    spi_set_clock_phase(SPI, SPI_CHIP_SEL, SPI_CLK_PHASE);
    spi_set_bits_per_transfer(SPI, SPI_CHIP_SEL, SPI_CSR_BITS_8_BIT);
    spi_enable_interrupt(SPI, SPI_IER_RDRF);
    spi_enable(SPI);
}


////////////////////////////////////////////////////
// set parameters to prepare for SPI transfer
////////////////////////////////////////////////////
void prepare_spi_transfer(void){
    transfer_len = image_len;
    transfer_index = 0;
}


////////////////////////////////////////////////////
// configure button interrupt for provisioning mode
////////////////////////////////////////////////////
void configure_wifi_provision_pin(void){
    pmc_enable_periph_clk(WIFI_COMM_ID);

    pio_set_debounce_filter(WIFI_COMM_PIO, PUSH_BUTTON_PIN_MSK, 10);

    pio_handler_set(WIFI_COMM_PIO, WIFI_COMM_ID, PUSH_BUTTON_PIN_MSK,
        PUSH_BUTTON_ATTR, wifi_provisioning_handler);

    NVIC_EnableIRQ((IRQn_Type)WIFI_COMM_ID);
    pio_enable_interrupt(WIFI_COMM_PIO, PUSH_BUTTON_PIN_MSK);
}


////////////////////////////////////////////////////
// handler for provisioning button, sets flag
////////////////////////////////////////////////////
void wifi_provisioning_handler(uint32_t ul_id, uint32_t ul_mask){
    unused(ul_id);
    unused(ul_mask);

    WIFI_PROVISION_FLAG = 1;
}


////////////////////////////////////////////////////
// write image from MCU to ESP
////////////////////////////////////////////////////
void write_image_to_web(void){
    if (image_len == 0) return;

    // step 1 - set up SPI transfer parameters
    prepare_spi_transfer();

    // step 2 - send image_transfer command with length
    char cmd_buf[50];
    sprintf(cmd_buf, "image_transfer %lu\r\n", image_len);
    write_wifi_command(cmd_buf, 10);

    // steps 3+4 - ESP pulls comm pin low, transfers over SPI,
    // then pulls comm pin high which fires wifi_command_response_handler
    // write_wifi_command already waits for wifi_comm_success so we are done
}