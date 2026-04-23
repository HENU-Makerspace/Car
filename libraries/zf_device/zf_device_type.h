#ifndef _zf_device_type_h_
#define _zf_device_type_h_

#include "zf_common_debug.h"

typedef enum
{
    NO_CAMERE = 0,
    CAMERA_BIN_IIC,
    CAMERA_BIN_UART,
    CAMERA_GRAYSCALE_SINGLE,
    CAMERA_GRAYSCALE_1,
    CAMERA_GRAYSCALE_2,
    CAMERA_COLOR,
} camera_type_enum;

typedef enum
{
    NO_WIRELESS = 0,
    WIRELESS_UART,
    BLE6A20,
    BLUETOOTH_CH9141,
    WIFI_UART,
    RECEIVER_UART,
} wireless_type_enum;

typedef enum
{
    NO_TOF = 0,
    TOF_DL1A,
    TOF_DL1B,
} tof_type_enum;

typedef void (*callback_function)(void);

extern wireless_type_enum wireless_type;
extern callback_function wireless_module_uart_handler;
extern callback_function wireless_module_spi_handler;

extern camera_type_enum camera_type;
extern callback_function camera_uart_handler_1;
extern callback_function camera_vsync_handler_1;
extern callback_function camera_dma_handler_1;
extern callback_function camera_uart_handler_2;
extern callback_function camera_vsync_handler_2;
extern callback_function camera_dma_handler_2;

extern tof_type_enum tof_type;
extern callback_function tof_module_exti_handler;

void set_camera_type(camera_type_enum type_set, callback_function vsync_callback, callback_function dma_callback, callback_function uart_callback);
void set_camera_type_2(camera_type_enum type_set, callback_function vsync_callback, callback_function dma_callback, callback_function uart_callback);
void set_wireless_type(wireless_type_enum type_set, callback_function wireless_callback);
void set_tof_type(tof_type_enum type_set, callback_function exti_callback);

#endif