// init.c
#include "init.h"
#include <asf.h>
#include <stdio.h>
#include <stdint.h>
#include "conf_board.h"
#include "conf_clock.h"

void init_board_pins(void){
    gpio_configure_pin(WIFI_RST_PIN,     PIO_OUTPUT_1);  // reset, default high
    gpio_configure_pin(WIFI_NET_PIN,     PIO_INPUT);      // read network status
    gpio_configure_pin(WIFI_CLIENTS_PIN, PIO_INPUT);      // read clients status
}