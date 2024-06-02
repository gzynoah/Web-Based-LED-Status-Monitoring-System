#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <esp_http_client.h>
#include "driver/gpio.h"

#define GPIO_INPUT_IO_17    17
#define GPIO_INPUT_IO_18    18

static const char *TAG = "http_request";


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


static void wifi_init_sta()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "SCS-WLAN-MA",
            .password = ""
        },
    };
    ESP_LOGI(TAG, "Connecting to AP...");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void http_post_task(void *pvParameters)
{
    gpio_set_direction(GPIO_INPUT_IO_17, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_INPUT_IO_18, GPIO_MODE_INPUT);

    int status = 0;
    if (gpio_get_level(GPIO_INPUT_IO_17) == 1) {
        status = 1;
    } else if (gpio_get_level(GPIO_INPUT_IO_18) == 1) {
        status = 0;
    }

    char post_data[100];
    sprintf(post_data, "{\"status\": %d}", status);

    esp_http_client_config_t config = {
        .url = "http://192.168.1.2/update-status", 
        .method = HTTP_METHOD_POST,
        .post_data = post_data,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}

void app_main()
{
    nvs_flash_init();
    wifi_init_sta();
    xTaskCreate(&http_post_task, "http_post_task", 4096, NULL, 5, NULL);
}
