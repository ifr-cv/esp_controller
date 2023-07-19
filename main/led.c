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

#include "status.h"

static const char *TAG = "led_strip";

extern led_strip_handle_t led_strip;

void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = 48,
        .max_leds = 1, // at least one LED on board
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

void blink_led(void *arg)
{
	while (1) {
        /* If the addressable LED is enabled */
        if (smsg.btn1 == 1) {
            /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
            led_strip_set_pixel(led_strip, 0, 16, 16, 16);
            /* Refresh the strip to send data */
            led_strip_refresh(led_strip);
        } else {
            /* Set all LED off to clear all pixels */
            led_strip_clear(led_strip);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);

	}

}

