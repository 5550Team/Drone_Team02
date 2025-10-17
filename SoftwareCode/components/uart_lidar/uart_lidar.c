#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define LIDAR_UART_NUM UART_NUM_1
#define LIDAR_UART_TX 35
#define LIDAR_UART_RX 37
#define LIDAR_UART_BAUDRATE 230400
#define TAG "LIDAR"

#define LIDAR_CMD_LEN 2
static const uint8_t lidar_start_cmd[LIDAR_CMD_LEN] = {0xA5, 0x60};

void lidar_uart_init(void)
{
    const uart_config_t uart_cfg = {
        .baud_rate = LIDAR_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_driver_install(LIDAR_UART_NUM, 2048, 0, 0, NULL, 0);
    uart_param_config(LIDAR_UART_NUM, &uart_cfg);
    uart_set_pin(LIDAR_UART_NUM, LIDAR_UART_TX, LIDAR_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    ESP_LOGI(TAG, "UART1 init OK, TX=%d RX=%d, %d bps", LIDAR_UART_TX, LIDAR_UART_RX, LIDAR_UART_BAUDRATE);
}

static void lidar_read_task(void *arg)
{
    // 发送启动扫描命令
    uart_write_bytes(LIDAR_UART_NUM, (const char *)lidar_start_cmd, LIDAR_CMD_LEN);
    ESP_LOGI(TAG, "LIDAR scan command sent: %02X %02X", lidar_start_cmd[0], lidar_start_cmd[1]);

    uint8_t rx_buf[128];
    while (1) {
        int len = uart_read_bytes(LIDAR_UART_NUM, rx_buf, sizeof(rx_buf), pdMS_TO_TICKS(200));
        if (len > 0) {
            ESP_LOGI(TAG, "Received %d bytes:", len);
            for (int i = 0; i < len; ++i) {
                printf("%02X ", rx_buf[i]);
            }
            printf("\n");
        } else {
            ESP_LOGW(TAG, "No data (timeout)");
        }
    }
}

void start_lidar_task(void)
{
    xTaskCreatePinnedToCore(lidar_read_task, "lidar_read_task", 4096, NULL, 5, NULL, tskNO_AFFINITY);
}
