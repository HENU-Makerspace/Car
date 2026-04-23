#include "zf_device_type.h"

static void type_default_callback(void);

camera_type_enum  camera_type                  = NO_CAMERE;
callback_function camera_uart_handler_1        = type_default_callback;
callback_function camera_vsync_handler_1       = type_default_callback;
callback_function camera_dma_handler_1         = type_default_callback;
callback_function camera_uart_handler_2        = type_default_callback;
callback_function camera_vsync_handler_2       = type_default_callback;
callback_function camera_dma_handler_2         = type_default_callback;

wireless_type_enum wireless_type               = NO_WIRELESS;
callback_function  wireless_module_uart_handler = type_default_callback;
callback_function  wireless_module_spi_handler  = type_default_callback;

tof_type_enum     tof_type                     = NO_TOF;
callback_function tof_module_exti_handler      = type_default_callback;

static void type_default_callback(void)
{
}

void set_camera_type(camera_type_enum type_set, callback_function vsync_callback, callback_function dma_callback, callback_function uart_callback)
{
    camera_type = type_set;

    if (type_set == CAMERA_GRAYSCALE_2 || type_set == CAMERA_GRAYSCALE_SINGLE)
    {
        camera_uart_handler_2  = (uart_callback  == NULL) ? type_default_callback : uart_callback;
        camera_vsync_handler_2 = (vsync_callback == NULL) ? type_default_callback : vsync_callback;
        camera_dma_handler_2   = (dma_callback   == NULL) ? type_default_callback : dma_callback;
    }
    else
    {
        camera_uart_handler_1  = (uart_callback  == NULL) ? type_default_callback : uart_callback;
        camera_vsync_handler_1 = (vsync_callback == NULL) ? type_default_callback : vsync_callback;
        camera_dma_handler_1   = (dma_callback   == NULL) ? type_default_callback : dma_callback;
    }
}

void set_camera_type_2(camera_type_enum type_set, callback_function vsync_callback, callback_function dma_callback, callback_function uart_callback)
{
    set_camera_type(type_set, vsync_callback, dma_callback, uart_callback);
}

void set_wireless_type(wireless_type_enum type_set, callback_function wireless_callback)
{
    wireless_type = type_set;
    wireless_module_uart_handler = (wireless_callback == NULL) ? type_default_callback : wireless_callback;
}

void set_tof_type(tof_type_enum type_set, callback_function exti_callback)
{
    tof_type = type_set;
    tof_module_exti_handler = (exti_callback == NULL) ? type_default_callback : exti_callback;
}