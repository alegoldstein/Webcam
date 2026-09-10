#include <asf.h>
#include "camera.h"
#include "conf_board.h"
#include "conf_clock.h"
#include <stdint.h>
#include <stdio.h>
#include "pio.h"
#include "ov2640.h"


/* Vsync signal information (true if it's triggered and false otherwise) */
static volatile uint32_t g_ul_vsync_flag = 0;

volatile uint32_t image_len;

uint8_t g_p_uc_cap_dest_buf[CAP_BUFFER_SIZE];

///////////////////////////////////////////////////////
//handler for rising edge of vsync, set flag
///////////////////////////////////////////////////////
void vsync_handler(uint32_t ul_id, uint32_t ul_mask){
    unused(ul_id);
	unused(ul_mask);

	g_ul_vsync_flag = true;
}


///////////////////////////////////////////////////////
//configure vsync interrupt
///////////////////////////////////////////////////////
void init_vsync_interrupts(){
    pio_handler_set(PIN_VSYNC_PIO,          // PIOA base pointer
                    OV2640_VSYNC_ID,        // ID_PIOA
                    OV2640_VSYNC_MASK,      // 1 << 15
                    OV2640_VSYNC_TYPE,      // PIO_IT_RISE_EDGE
                    vsync_handler);

    NVIC_EnableIRQ((IRQn_Type) OV2640_VSYNC_ID);
}


///////////////////////////////////////////////////////
//configure TWI (two wire interface I2C)
///////////////////////////////////////////////////////
void configure_twi(){
    twi_options_t opt;

	pmc_enable_periph_clk(ID_TWI0);

	gpio_configure_pin(PIO_PA3A_TWD0,  PIO_PERIPH_A | PIO_PULLUP);
	gpio_configure_pin(PIO_PA4A_TWCK0, PIO_PERIPH_A | PIO_PULLUP);

    // Setup TWI options
    opt.master_clk = sysclk_get_cpu_hz();
    opt.speed      = 100000;        // 100kHz standard mode
    opt.chip       = 0x30;          // OV2640 SCCB write address

    // Initialize TWI Master
    twi_master_init(TWI0, &opt);
}



///////////////////////////////////////////////////////
//configure and initialize parallel capture 
///////////////////////////////////////////////////////
void pio_capture_init(Pio *p_pio, uint32_t ul_id){
	
/* Enable periphral clock */
	pmc_enable_periph_clk(ul_id);

	/* Disable pio capture */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_PCEN);

	/* Disable rxbuff interrupt */
	p_pio->PIO_PCIDR |= PIO_PCIDR_RXBUFF;

	/* 32bit width*/
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_DSIZE_Msk);
	p_pio->PIO_PCMR |= PIO_PCMR_DSIZE_WORD;

	/* Only HSYNC and VSYNC enabled */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_ALWYS);
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_HALFS);

#if !defined(DEFAULT_MODE_COLORED)
	/* Samples only data with even index */
	p_pio->PIO_PCMR |= PIO_PCMR_HALFS;
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_FRSTS);
#endif
}



///////////////////////////////////////////////////////
//use parallel capture and PDC to store images in buffer
///////////////////////////////////////////////////////
uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size){
/* Check if the first PDC bank is free */
	if ((p_pio->PIO_RCR == 0) && (p_pio->PIO_RNCR == 0)) {
		p_pio->PIO_RPR = (uint32_t)uc_buf;
		p_pio->PIO_RCR = ul_size;
		p_pio->PIO_PTCR = PIO_PTCR_RXTEN;
		return 1;
	} else if (p_pio->PIO_RNCR == 0) {
		p_pio->PIO_RNPR = (uint32_t)uc_buf;
		p_pio->PIO_RNCR = ul_size;
		return 1;
	} else {
		return 0;
	}
}



///////////////////////////////////////////////////////
//finds image length based on JPED protocol, can find start of image (SOI) before end of image 
///////////////////////////////////////////////////////(EOI)
//return 1 if success 0 and 0 if failure
uint8_t find_image_len(){
    uint8_t *buf = g_p_uc_cap_dest_buf;
    uint32_t i;

    // Verify SOI marker at the start (0xFF 0xD8)
    if (buf[0] != 0xFF || buf[1] != 0xD8){
        image_len = 0;
        return 0;   // no valid JPEG start found
    }

    // Scan for EOI marker (0xFF 0xD9)
    for (i = 2; i < CAP_BUFFER_SIZE - 1; i++){
        if (buf[i] == 0xFF && buf[i + 1] == 0xD9){
            image_len = i + 2;  // include the EOI marker itself
            return 1;
        }
    }

    // EOI not found
    image_len = 0;
    return 0;
}


///////////////////////////////////////////////////////
//configure camera pins and clk (XCLK), and call configure_twi()
///////////////////////////////////////////////////////
//not sure if this is the right to copy and paste, capture_init in ov7440 ex
void init_camera(){
// Data bus (camera -> MCU)
gpio_configure_pin(PIN_D0, PIO_INPUT);
gpio_configure_pin(PIN_D1, PIO_INPUT);
gpio_configure_pin(PIN_D2, PIO_INPUT);
gpio_configure_pin(PIN_D3, PIO_INPUT);
gpio_configure_pin(PIN_D4, PIO_INPUT);
gpio_configure_pin(PIN_D5, PIO_INPUT);
gpio_configure_pin(PIN_D6, PIO_INPUT);
gpio_configure_pin(PIN_D7, PIO_INPUT);

// Timing signals (camera -> MCU)
gpio_configure_pin(PIN_CAM_PCLK, PIO_INPUT);
gpio_configure_pin(PIN_HREF, PIO_INPUT);
gpio_configure_pin(PIN_VSYNC, PIO_INPUT | PIO_PULLUP);

gpio_configure_pin(PIN_CAM_XCLK, PIO_PERIPH_B);
pmc_enable_pck(PMC_PCK_0);
pmc_switch_pck_to_mainck(PMC_PCK_0, PMC_PCK_PRES_CLK_1);

// Reset (MCU -> camera)
// OV2640 reset is active LOW, so default HIGH = not in reset
gpio_configure_pin(PIN_CAM_RST, PIO_OUTPUT_1);
pio_capture_init(OV2640_DATA_BUS_PIO, OV2640_DATA_BUS_ID);
configure_twi();
}


//////////////////////////////////////////////////
//configure 0v2640 registers
//////////////////////////////////////////////////
void configure_camera(){
	ov_configure(BOARD_TWI, JPEG_INIT);
	ov_configure(BOARD_TWI, YUV422);
	ov_configure(BOARD_TWI, JPEG);
	ov_configure(BOARD_TWI, JPEG_320x240);
}

//////////////////////////////////////////////////
//capture image on rising edge of vsync and gets image length
//////////////////////////////////////////////////
uint8_t start_capture(){
    g_ul_vsync_flag = 0;
    pio_enable_interrupt(PIN_VSYNC_PIO, OV2640_VSYNC_MASK);
    while (!g_ul_vsync_flag) {
	}
    pio_disable_interrupt(PIN_VSYNC_PIO, OV2640_VSYNC_MASK);
    g_ul_vsync_flag = 0;

    pio_capture_enable(OV2640_DATA_BUS_PIO);
    pio_capture_to_buffer(OV2640_DATA_BUS_PIO, g_p_uc_cap_dest_buf,
            CAP_BUFFER_SIZE >> 2);
    while (!((OV2640_DATA_BUS_PIO->PIO_PCISR & PIO_PCIMR_RXBUFF) == PIO_PCIMR_RXBUFF)) {}
    pio_capture_disable(OV2640_DATA_BUS_PIO);

    // find image length and return success/fail
    return find_image_len();
}



