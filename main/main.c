#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "led_strip.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "tcp_server.h"
#include "udp_server.h"
#include "station.h"
#include "uart_events.h"
#include "communication_delay.h"

#include "led_strip.h"
#include "led.h"

static const char *TAG = "esp_controller";

//static uint8_t s_led_state = 0;

led_strip_handle_t led_strip;

uint32_t rev_count = 0;

#pragma pack(1)
struct CommunicationStatus{
   uint16_t communication_delay : 9; //0~400
   uint8_t packet_loss_rate : 7;    //0~100
};
#pragma pack()
struct CommunicationStatus CS={.communication_delay = 400, .packet_loss_rate = 100};



#pragma pack(1)
struct SocketMsg {
    uint8_t head : 8;   //1
    uint8_t btn1 : 2;
    uint8_t btn2 : 1;
    uint8_t btn3 : 1;
    uint8_t btn4 : 1;
    uint8_t pole : 3;   //2
    uint16_t ch1 : 11;
    uint16_t ch2 : 11;
    uint8_t reverse1 : 2;//5
    uint16_t ch3 : 11;
    uint16_t ch4 : 11;
    uint8_t reverse2 : 2;//8
    int8_t sc : 8;       //9
    uint16_t sequence : 16;//11
};
#pragma pack()

struct SocketMsg smsg;



void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "SET smsg.head=0X5D");
    smsg.head = 0x5D;
    ESP_LOGI(TAG, "configure_led");
    configure_led();
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
    esp_wifi_set_ps(WIFI_PS_NONE);
    ESP_LOGI(TAG, "uart_init");   
    uart_init();
#ifdef CONFIG_EXAMPLE_IPV4
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);
    // xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET, 5, NULL);
#endif
#ifdef CONFIG_EXAMPLE_IPV6
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET6, 5, NULL);
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET6, 5, NULL);
#endif

    // xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);

    // xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, 5, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, 4, NULL);
        /* Configure the peripheral according to the LED type */
    xTaskCreate(blink_led, "led_statue", 1024*2, NULL, 3, NULL);
    xTaskCreate(communication_delay, "delay_calculate", 1024*2, NULL, 3, NULL);


}

