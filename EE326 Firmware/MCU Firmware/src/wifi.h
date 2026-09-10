#ifndef WIFI_H_
#define WIFI_H_

#include <asf.h>
#include <string.h>

// ── USART ────────────────────────────────────────────────────────────────────
#define WIFI_USART					USART0
#define WIFI_USART_ID				ID_USART0
#define WIFI_USART_BAUDRATE			115200
#define WIFI_USART_HANDLER			USART0_Handler
#define WIFI_USART_IRQn				USART0_IRQn
#define WIFI_USART_CHAR_LENGTH		US_MR_CHRL_8_BIT
#define WIFI_USART_PARITY			US_MR_PAR_NO
#define WIFI_USART_STOP_BITS		US_MR_NBSTOP_1_BIT
#define WIFI_USART_MODE				US_MR_CHMODE_NORMAL

/** USART0 pin RX */
#define PIN_USART0_RXD	  {PIO_PA5A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_USART0_RXD_IDX        (PIO_PA5_IDX)
#define PIN_USART0_RXD_FLAGS      (PIO_PERIPH_A | PIO_PULLUP)
/** USART0 pin TX */
#define PIN_USART0_TXD    {PIO_PA6A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_USART0_TXD_IDX        (PIO_PA6_IDX)
#define PIN_USART0_TXD_FLAGS      (PIO_PERIPH_A | PIO_PULLUP)

/** All interrupt mask. */
#define ALL_INTERRUPT_MASK  0xffffffff

// ── SPI ──────────────────────────────────────────────────────────────────────
#define SPI_MISO_GPIO           (PIO_PA12_IDX)
#define SPI_MISO_FLAGS          (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI_MOSI_GPIO           (PIO_PA13_IDX)
#define SPI_MOSI_FLAGS          (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI_SPCK_GPIO           (PIO_PA14_IDX)
#define SPI_SPCK_FLAGS          (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI_NPCS0_GPIO          (PIO_PA11_IDX)
#define SPI_NPCS0_FLAGS         (PIO_PERIPH_A | PIO_DEFAULT)

#define SPI_CHIP_SEL            0
#define SPI_CHIP_PCS            spi_get_pcs(SPI_CHIP_SEL)
#define SPI_CLK_POLARITY        0
#define SPI_CLK_PHASE           0

// ── WiFi comm complete pin ────────────────────────────────────────────────────
#define WIFI_COMM_PIN_NUM       PIO_PA2
#define WIFI_COMM_PIO           PIOA
#define WIFI_COMM_ID            ID_PIOA
#define WIFI_COMM_ATTR          PIO_IT_RISE_EDGE

// ── WiFi provisioning button ──────────────────────────────────────────────────
#define PUSH_BUTTON_PIN_MSK     PIO_PB0
#define PUSH_BUTTON_ATTR        PIO_IT_RISE_EDGE


// ── Buffer / misc ─────────────────────────────────────────────────────────────
#define MAX_INPUT_WIFI          1000
#define ALL_INTERRUPT_MASK      0xffffffff

// ── Shared variables (defined once in wifi.c) ────────────────────────────────
extern volatile char            input_line_wifi[MAX_INPUT_WIFI];
extern volatile uint32_t        received_byte_wifi;
extern volatile bool            new_rx_wifi;
extern volatile unsigned int    input_pos_wifi;
extern volatile bool            wifi_comm_success;
extern volatile uint32_t        transfer_index;
extern volatile uint32_t        transfer_len;
extern volatile uint32_t        WIFI_PROVISION_FLAG;
extern uint8_t                  im_buf[];

// ── Function declarations ─────────────────────────────────────────────────────

// USART handler — expands to USART0_Handler via macro, called by hardware vector table
void WIFI_USART_HANDLER(void);

// USART
void process_incoming_byte_wifi(uint8_t in_byte);
void process_data_wifi(void);
void configure_usart_wifi(void);

// Interrupt handlers
void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask);
void wifi_provisioning_handler(uint32_t ul_id, uint32_t ul_mask);

// Pin configuration
void configure_wifi_comm_pin(void);
void configure_wifi_provision_pin(void);

// SPI handler — must be named SPI_Handler for hardware vector table
void SPI_Handler(void);
void configure_spi(void);
void spi_peripheral_initialize(void);
void prepare_spi_transfer(void);

// WiFi commands
void write_wifi_command(char* comm, uint8_t cnt);
void write_image_to_web(void);

#endif /* WIFI_H_ */