#include <asf.h>
#include "wifi.h"
#include "camera.h"
#include "init.h"
#include "timer_interface.h"

/* int main(void)
{
    // system init
    sysclk_init();
    board_init();
    init_board_pins();
    configure_tc();

    // wifi init
    configure_usart_wifi();             // fixed name
    configure_spi();
    spi_peripheral_initialize();
    configure_wifi_comm_pin();
    configure_wifi_provision_pin();

    // camera init
    init_vsync_interrupts();
    init_camera();
    configure_camera();

    // reset ESP first so it's running before we send commands
    gpio_set_pin_low(WIFI_RST_PIN);
    delay_ms(100);
    gpio_set_pin_high(WIFI_RST_PIN);
    delay_ms(500);                      // wait for ESP to boot

    // configure ESP32 GPIOs
	usart_write_line(WIFI_USART, "set comm_gpio 21\r\n");


    char cmd_buf[50];
    sprintf(cmd_buf, "set wlan_gpio 27\r\n");
    write_wifi_command(cmd_buf, 10);

    sprintf(cmd_buf, "set websocket_gpio 26\r\n");
    write_wifi_command(cmd_buf, 10);

    sprintf(cmd_buf, "set ap_gpio 25\r\n");
    write_wifi_command(cmd_buf, 10);

    sprintf(cmd_buf, "set comm_pin 21\r\n");
    write_wifi_command(cmd_buf, 10);

    sprintf(cmd_buf, "set net_pin 22\r\n");
    write_wifi_command(cmd_buf, 10);

    sprintf(cmd_buf, "set clients_pin 32\r\n");
    write_wifi_command(cmd_buf, 10);

    // wait for network connection, handle provisioning while waiting
    while (!ioport_get_pin_level(WIFI_NET_PIN)) {
        if (WIFI_PROVISION_FLAG) {
            WIFI_PROVISION_FLAG = 0;
            sprintf(cmd_buf, "provision\r\n");
            write_wifi_command(cmd_buf, 10);
        }
    }

    // send test, retry every 10 seconds until SUCCESS
    wifi_comm_success = false;
    counts = 0;
    usart_write_line(WIFI_USART, "test\r\n");

    while (!wifi_comm_success) {
        if (counts >= 10) {
            counts = 0;
            gpio_set_pin_low(WIFI_RST_PIN);
            delay_ms(100);
            gpio_set_pin_high(WIFI_RST_PIN);
            delay_ms(500);
            while (!ioport_get_pin_level(WIFI_NET_PIN)) {}
            sprintf(cmd_buf, "test\r\n");
            write_wifi_command(cmd_buf, 10);
        }
    }
    wifi_comm_success = false;

    // main loop
    while (1) {

        // check for provisioning request
        if (WIFI_PROVISION_FLAG) {
            WIFI_PROVISION_FLAG = 0;
            sprintf(cmd_buf, "provision\r\n");
            write_wifi_command(cmd_buf, 10);
            while (!ioport_get_pin_level(WIFI_NET_PIN)) {}
        }

        // capture and send if network up and clients connected
        if (ioport_get_pin_level(WIFI_NET_PIN) && ioport_get_pin_level(WIFI_CLIENTS_PIN)) {
            if (start_capture()) {
                write_image_to_web();
            }
        }
    }
}
	*/

int main(void){
	sysclk_init();
	//wdt_disable(WDT);
	board_init();
	
	configure_usart_wifi();
	configure_wifi_comm_pin();
	usart_write_line(WIFI_USART, "set comm_gpio 21\r\n");

	while(1) {
		usart_write_line(WIFI_USART, "test\r\n");
		delay_ms(500);
	}
}