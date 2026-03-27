#include "qmi8658.h"
#include "i2c_bsp/i2c_bsp.h"
#include "user_config.h"
#include "esp_log.h"
#include <Arduino.h>

static const char *TAG = "QMI8658";

// QMI8658 Registers
#define QMI8658_WHO_AM_I     0x00
#define QMI8658_CTRL1        0x02
#define QMI8658_CTRL2        0x03
#define QMI8658_CTRL3        0x04
#define QMI8658_CTRL4        0x05
#define QMI8658_CTRL5        0x06
#define QMI8658_CTRL7        0x08
#define QMI8658_AX_L         0x35

void qmi8658_init() {
    uint8_t id = 0;
    i2c_read_buff(imu_dev_handle, QMI8658_WHO_AM_I, &id, 1);
    if (id != 0x05) {
        ESP_LOGE(TAG, "QMI8658 not found! ID: 0x%02x", id);
        return;
    }
    ESP_LOGI(TAG, "QMI8658 found, ID: 0x%02x", id);

    uint8_t ctrl1 = 0x60; // 0x60: Address auto-increment, enable reading
    i2c_write_buff(imu_dev_handle, QMI8658_CTRL1, &ctrl1, 1);

    // Enable Accelerometer and set ODR
    uint8_t ctrl2 = 0x24; // 0x24: aODR 1KHz, aFS 2g
    i2c_write_buff(imu_dev_handle, QMI8658_CTRL2, &ctrl2, 1);

    // Enable Gyroscope and set ODR (optional, but good to enable)
    uint8_t ctrl3 = 0x64; // 0x64: gODR 1KHz, gFS 2048dps
    i2c_write_buff(imu_dev_handle, QMI8658_CTRL3, &ctrl3, 1);

    // Enable sensors
    uint8_t ctrl7 = 0x03; // Enable Gyro and Accel
    i2c_write_buff(imu_dev_handle, QMI8658_CTRL7, &ctrl7, 1);

    delay(10);
}

void qmi8658_read_accel(float *x, float *y, float *z) {
    uint8_t buf[6];
    i2c_read_buff(imu_dev_handle, QMI8658_AX_L, buf, 6);

    int16_t ax = (buf[1] << 8) | buf[0];
    int16_t ay = (buf[3] << 8) | buf[2];
    int16_t az = (buf[5] << 8) | buf[4];

    // Scale to g (assume +/- 2g range)
    *x = ax / 16384.0f;
    *y = ay / 16384.0f;
    *z = az / 16384.0f;
}
