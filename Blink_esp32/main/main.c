#include <stdio.h>

#include "driver/gpio.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"


static const char *TAG = "WS_SERVER";

// WIFI Conectivity
#define WIFI_SSID "SSID"
#define WIFI_PASS "password"
void wifi_init_sta();

// LED
#define led1 12

// HTTP Server
httpd_handle_t server = NULL;
int socket_fd = -1;
httpd_handle_t start_webserver(void);
static esp_err_t connection_handler(httpd_req_t *req);
static esp_err_t send(httpd_req_t *req);

// LED  Blink
uint8_t led_level = 1;
TimerHandle_t xTimer;
esp_err_t init_led(void);
esp_err_t blink_led(void);

void app_main(void){
    // WIFI
    nvs_flash_init();
    wifi_init_sta();

    // LED
    init_led();

    // SERVER
    server = start_webserver();
}

// Minimalistic function to establish wifi connection
void wifi_init_sta(){
    // ESP uses lwIP stack, so we need to initialize TCP/IP stack
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };
    // Station mode is default mode sot its not needed
    //esp_wifi_set_mode(WIFI_MODE_STA);

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    // Smarter would be to create event handler. It's super error friendly
    // but I am lazy  and it works:)
    esp_wifi_connect();
}

// Function to establish websocket connection with ROS2
httpd_handle_t start_webserver(void){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK){
        // Set URI handlers
        httpd_uri_t uri_get = {
            .uri = "/ws",
            .method = HTTP_GET,
            .handler = connection_handler,
            .user_ctx = NULL,
            .is_websocket = true
        };
        httpd_register_uri_handler(server, &uri_get);
        return server;
    }
    return NULL;
}

// Execute after connection is established
static esp_err_t connection_handler(httpd_req_t *req){
    ESP_LOGI(TAG, "Handshake done");
    //socket_fd = httpd_req_to_sockfd(req); \\ For future plan to make it asynchronous
    while (1){
        // Every 1.5s blink led and send its state to ROS2
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        blink_led();
        send(req);
    }
    return ESP_OK;
}

// Send led state to ROS2
static esp_err_t send(httpd_req_t *req){
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t)); // Clear frame structure
    ws_pkt.type = HTTPD_WS_TYPE_BINARY;
    ws_pkt.payload = &led_level;
    ws_pkt.len = sizeof(led_level);
    httpd_ws_send_frame(req, &ws_pkt);
    return ESP_OK;
}

esp_err_t init_led(void){
    gpio_reset_pin(led1);
    gpio_set_direction(led1, GPIO_MODE_OUTPUT);
    return ESP_OK;
}

esp_err_t blink_led(void){
    led_level = !led_level;
    gpio_set_level(led1, led_level);
    return ESP_OK;
}
