// init.h
#ifndef INIT_H
#define INIT_H

#include <asf.h>

#define WIFI_RST_PIN        (PIO_PB2_IDX)
#define WIFI_NET_PIN        (PIO_PA1_IDX)
#define WIFI_CLIENTS_PIN    (PIO_PB1_IDX)

void init_board_pins(void);

#endif