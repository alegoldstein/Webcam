//camera pin, variable, and function declarations
//camera TWI parameters



#ifndef camera_h
#define camera_h

#include "asf.h"
#include <stdint.h>
#include <stdio.h>
#include "pio.h"
//#include <twi_master.h>
#include <pmc.h>
#include <gpio.h>
#include <sysclk.h>
#include "ov2640.h"

extern volatile uint32_t image_len;

#define OV2640_DATA_BUS_PIO     PIOA
#define OV2640_DATA_BUS_ID      ID_PIOA
#define PIN_VSYNC_PIO           PIOA
#define OV2640_VSYNC_ID         ID_PIOA
#define OV2640_VSYNC_MASK       (1 << 15)
#define OV2640_VSYNC_TYPE       PIO_IT_RISE_EDGE
#define BOARD_TWI				TWI0

#define CAP_BUFFER_SIZE     (50000)         // 50kb
extern uint8_t g_p_uc_cap_dest_buf[];

//handler for rising edge of vsync, set flag
void vsync_handler(uint32_t ul_id, uint32_t ul_mask);

//configure vsync interrupt
void init_vsync_interrupts(void);

//configure TWI (two wire interface I2C)
void configure_twi(void);

//configure and initialize parallel capture 
void pio_capture_init(Pio *p_pio, uint32_t ul_id);

//use parallel capture and PDC to store images in buffer
uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size);

//configure camera pins and clk (XCLK), and call configure_twi()
void init_camera(void);

//configure camera registers
void configure_camera(void);

//capture image on rising edge of vsync, gets image length and returns 1 on success and 0 on fail
uint8_t start_capture(void);

//finds image length based on JPED protocol, can find start of image (SOI) before end of image (EOI)
//return 1 if success 0 and 0 if failure
uint8_t find_image_len(void);

#endif