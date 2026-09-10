/*
 * wifi.h
 *
 * Created: 1/31/2023 1:02:50 PM
 *  Author: imikh
 */ 


#ifndef WIFI_H_
#define WIFI_H_

#define WIFI_USART					USART0
#define WIFI_USART_ID				ID_USART0
#define WIFI_USART_BAUDRATE			115200//5000000//921600//
#define WIFI_USART_HANDLER			USART0_Handler
#define WIFI_USART_IRQn				USART0_IRQn
#define WIFI_USART_CHAR_LENGTH		US_MR_CHRL_8_BIT
#define WIFI_USART_PARITY			US_MR_PAR_NO
#define WIFI_USART_STOP_BITS		US_MR_NBSTOP_1_BIT
#define WIFI_USART_MODE				US_MR_CHMODE_NORMAL


/** All interrupt mask. */
#define ALL_INTERRUPT_MASK  0xffffffff

/* Chip select. */
#define SPI_CHIP_SEL 0
#define SPI_CHIP_PCS spi_get_pcs(SPI_CHIP_SEL)

/* Clock polarity. */
#define SPI_CLK_POLARITY 0

/* Clock phase. */
#define SPI_CLK_PHASE 0//1

/* Delay before SPCK. */
#define SPI_DLYBS 0x40

/* Delay between consecutive transfers. */
#define SPI_DLYBCT 0x10

volatile uint32_t transfer_index;
volatile uint32_t transfer_len;

void spi_prep_transfer(void);
void spi_peripheral_initialize(void);
void configure_spi();
void configure_usart_wifi(void);



#endif /* WIFI_H_ */