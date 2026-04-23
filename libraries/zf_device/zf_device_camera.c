#include "zf_common_debug.h"
#include "zf_common_interrupt.h"
#include "zf_driver_gpio.h"
#include "zf_driver_dma.h"
#include "zf_driver_exti.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ov7725.h"
#include "zf_device_scc8660.h"
#include "isr_config.h"
#include "zf_device_camera.h"

fifo_struct camera_receiver_fifo;
uint8 camera_receiver_buffer[CAMERA_RECEIVER_BUFFER_SIZE];
uint8 camera_send_image_frame_header[4] = {0x00, 0xFF, 0x01, 0x01};

void camera_binary_image_decompression(const uint8 *data1, uint8 *data2, uint32 image_size)
{
    zf_assert(NULL != data1);
    zf_assert(NULL != data2);

    while (image_size--)
    {
        uint8 i = 8;
        while (i--)
        {
            *data2++ = (((*data1 >> i) & 0x01) ? 255 : 0);
        }
        data1++;
    }
}

void camera_send_image(uart_index_enum uartn, const uint8 *image_addr, uint32 image_size)
{
    zf_assert(NULL != image_addr);
    uart_write_buffer(uartn, camera_send_image_frame_header, 4);
    uart_write_buffer(uartn, (uint8 *)image_addr, image_size);
}

void camera_fifo_init(void)
{
    fifo_init(&camera_receiver_fifo, FIFO_DATA_8BIT, camera_receiver_buffer, CAMERA_RECEIVER_BUFFER_SIZE);
}

uint8 camera_init(uint8 *source_addr, uint8 *destination_addr, uint32 image_size)
{
    uint8 num = 0;
    uint8 link_list_num = 0;

    switch (camera_type)
    {
        case CAMERA_BIN_IIC:
        case CAMERA_BIN_UART:
            for (num = 0; num < 8; num++)
            {
                gpio_init((gpio_pin_enum)(OV7725_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
            }
            link_list_num = dma_init(OV7725_DMA_CH, source_addr, destination_addr, OV7725_PCLK_PIN, EXTI_TRIGGER_FALLING, image_size);
            exti_init(OV7725_VSYNC_PIN, EXTI_TRIGGER_FALLING);
            break;

        case CAMERA_GRAYSCALE_1:
            for (num = 0; num < 8; num++)
            {
                gpio_init((gpio_pin_enum)(MT9V03X_1_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
            }
            link_list_num = dma_init(MT9V03X_1_DMA_CH, source_addr, destination_addr, MT9V03X_1_PCLK_PIN, EXTI_TRIGGER_RISING, image_size);
            exti_init(MT9V03X_1_VSYNC_PIN, EXTI_TRIGGER_BOTH);
            break;

        case CAMERA_GRAYSCALE_SINGLE:
        case CAMERA_GRAYSCALE_2:
            for (num = 0; num < 8; num++)
            {
                gpio_init((gpio_pin_enum)(MT9V03X_2_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
            }
            link_list_num = dma_init_2(MT9V03X_2_DMA_CH, source_addr, destination_addr, MT9V03X_2_PCLK_PIN, EXTI_TRIGGER_RISING, image_size);
            exti_init(MT9V03X_2_VSYNC_PIN, EXTI_TRIGGER_BOTH);
            break;

        case CAMERA_COLOR:
            for (num = 0; num < 8; num++)
            {
                gpio_init((gpio_pin_enum)(SCC8660_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
            }
            link_list_num = dma_init(SCC8660_DMA_CH, source_addr, destination_addr, SCC8660_PCLK_PIN, EXTI_TRIGGER_RISING, image_size);
            exti_init(SCC8660_VSYNC_PIN, EXTI_TRIGGER_FALLING);
            break;

        default:
            break;
    }

    return link_list_num;
}