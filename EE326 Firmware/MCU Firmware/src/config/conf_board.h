

#ifndef conf_board_h
#define conf_board_h

#define PINS_WIFI_USART				(PIO_PA5A_RXD0 | PIO_PA6A_TXD0)
#define PINS_WIFI_USART_FLAGS		(PIO_PERIPH_A | PIO_DEFAULT)
#define PINS_WIFI_USART_MASK		(PIO_PA5A_RXD0 | PIO_PA6A_TXD0)
#define PINS_WIFI_USART_PIO			PIOA
#define PINS_WIFI_USART_ID			ID_PIOA
#define PINS_WIFI_USART_TYPE		PIO_PERIPH_A
#define PINS_WIFI_USART_ATTR		PIO_DEFAULT

/** USART0 pin RX */
#define PIN_USART0_RXD	  {PIO_PA5A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_USART0_RXD_IDX        (PIO_PA5_IDX)
#define PIN_USART0_RXD_FLAGS      (PIO_PERIPH_A | PIO_PULLUP)
/** USART0 pin TX */
#define PIN_USART0_TXD    {PIO_PA6A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_USART0_TXD_IDX        (PIO_PA6_IDX)
#define PIN_USART0_TXD_FLAGS      (PIO_PERIPH_A | PIO_PULLUP)

/** SPI MISO pin definition. */
#define SPI_MISO_GPIO         (PIO_PA12_IDX)
#define SPI_MISO_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI MOSI pin definition. */
#define SPI_MOSI_GPIO         (PIO_PA13_IDX)
#define SPI_MOSI_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI SPCK pin definition. */
#define SPI_SPCK_GPIO         (PIO_PA14_IDX)
#define SPI_SPCK_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)

/** SPI chip select 0 pin definition. (Only one configuration is possible) */
#define SPI_NPCS0_GPIO         (PIO_PA11_IDX)
#define SPI_NPCS0_FLAGS           (PIO_PERIPH_A | PIO_DEFAULT)



//cam pin declarations
#define PIN_D0     (PIO_PA25_IDX)
#define PIN_D1     (PIO_PA24_IDX)
#define PIN_D2     (PIO_PA26_IDX)
#define PIN_D3     (PIO_PA27_IDX)
#define PIN_D4     (PIO_PA28_IDX)
#define PIN_D5     (PIO_PA29_IDX)
#define PIN_D6     (PIO_PA30_IDX)
#define PIN_D7     (PIO_PA31_IDX)

#define PIN_CAM_SCL     (PIO_PA4_IDX)
#define PIN_CAM_SDA     (PIO_PA3_IDX)
#define PIN_CAM_XCLK     (PIO_PA17_IDX)
#define PIN_CAM_PCLK     (PIO_PA23_IDX)
#define PIN_HREF     (PIO_PA16_IDX)
#define PIN_VSYNC     (PIO_PA15_IDX)
#define PIN_CAM_RST     (PIO_PA20_IDX)

#define OV2640_DATA_BUS_PIO     PIOA
#define OV2640_DATA_BUS_ID      ID_PIOA

#define PIN_VSYNC_PIO           PIOA
#define OV2640_VSYNC_ID         ID_PIOA
#define OV2640_VSYNC_MASK       (1 << 15)   // PA15



#define CONSOLE_UART_ID ID_UART0

#define CONF_BOARD_SPI

#define CONF_BOARD_SPI_NPCS0

//peripheral ID definitions
#define SPI_ID ID_SPI

//base addresses
#define SPI_MASTER_BASE
#define SPI_SLAVE_BASE

#endif