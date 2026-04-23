#ifndef _zf_device_mt9v03x_h_
#define _zf_device_mt9v03x_h_

#include "zf_common_typedef.h"
#include "zf_device_type.h"

#define MT9V03X_1_COF_IIC_DELAY   (800)
#define MT9V03X_1_COF_IIC_SCL     (P33_13)
#define MT9V03X_1_COF_IIC_SDA     (P32_4)
#define MT9V03X_1_DMA_CH          (IfxDma_ChannelId_7)
#define MT9V03X_1_PCLK_PIN        (ERU_CH1_REQ10_P14_3)
#define MT9V03X_1_VSYNC_PIN       (ERU_CH4_REQ8_P33_7)
#define MT9V03X_1_DATA_PIN        (P00_0)
#define MT9V03X_1_DATA_ADD        (get_port_in_addr(MT9V03X_1_DATA_PIN))
#define MT9V03X_1_INIT_TIMEOUT    (0x0080)

#define MT9V03X_2_COF_IIC_DELAY   (800)
#define MT9V03X_2_COF_IIC_SCL     (P13_1)
#define MT9V03X_2_COF_IIC_SDA     (P13_0)
#define MT9V03X_2_DMA_CH          (IfxDma_ChannelId_6)
#define MT9V03X_2_PCLK_PIN        (ERU_CH2_REQ2_P10_2)
#define MT9V03X_2_VSYNC_PIN       (ERU_CH3_REQ3_P10_3)
#define MT9V03X_2_DATA_PIN        (P02_0)
#define MT9V03X_2_DATA_ADD        (get_port_in_addr(MT9V03X_2_DATA_PIN))
#define MT9V03X_2_INIT_TIMEOUT    (0x0080)

#define MT9V03X_INIT_INTV_DELAY   (0)

#define MT9V03X_1_W               (188)
#define MT9V03X_1_H               (120)
#define MT9V03X_1_IMAGE_SIZE      (MT9V03X_1_W * MT9V03X_1_H)
#define MT9V03X_1_AUTO_EXP_DEF    (0)
#define MT9V03X_1_EXP_TIME_DEF    (512)
#define MT9V03X_1_FPS_DEF         (50)
#define MT9V03X_1_LR_OFFSET_DEF   (0)
#define MT9V03X_1_UD_OFFSET_DEF   (0)
#define MT9V03X_1_GAIN_DEF        (32)
#define MT9V03X_1_PCLK_MODE_DEF   (0)

#define MT9V03X_2_W               (188)
#define MT9V03X_2_H               (120)
#define MT9V03X_2_IMAGE_SIZE      (MT9V03X_2_W * MT9V03X_2_H)
#define MT9V03X_2_AUTO_EXP_DEF    (0)
#define MT9V03X_2_EXP_TIME_DEF    (512)
#define MT9V03X_2_FPS_DEF         (50)
#define MT9V03X_2_LR_OFFSET_DEF   (0)
#define MT9V03X_2_UD_OFFSET_DEF   (0)
#define MT9V03X_2_GAIN_DEF        (32)
#define MT9V03X_2_PCLK_MODE_DEF   (0)

#define MT9V03X_W                 MT9V03X_1_W
#define MT9V03X_H                 MT9V03X_1_H
#define MT9V03X_IMAGE_SIZE        MT9V03X_1_IMAGE_SIZE

typedef enum
{
    MT9V03X_DOUBLE_INIT = 0,
    MT9V03X_DOUBLE_AUTO_EXP,
    MT9V03X_DOUBLE_EXP_TIME,
    MT9V03X_DOUBLE_FPS,
    MT9V03X_DOUBLE_SET_COL,
    MT9V03X_DOUBLE_SET_ROW,
    MT9V03X_DOUBLE_LR_OFFSET,
    MT9V03X_DOUBLE_UD_OFFSET,
    MT9V03X_DOUBLE_GAIN,
    MT9V03X_DOUBLE_PCLK_MODE,
    MT9V03X_DOUBLE_CONFIG_FINISH,

    MT9V03X_DOUBLE_COLOR_GET_WHO_AM_I = 0xEF,
    MT9V03X_DOUBLE_SET_EXP_TIME = 0xF0,
    MT9V03X_DOUBLE_GET_STATUS,
    MT9V03X_DOUBLE_GET_VERSION,
    MT9V03X_DOUBLE_SET_ADDR = 0xFE,
    MT9V03X_DOUBLE_SET_DATA
} m9v03x_double_cmd_enum;

typedef enum
{
    mt9v03x_1,
    mt9v03x_2,
    mt9v03x_double,
} m9v03x_double_init_type_enum;

extern vuint8 mt9v03x_finish_flag_1;
extern vuint8 mt9v03x_finish_flag_2;
extern uint8  mt9v03x_image_1[MT9V03X_1_H][MT9V03X_1_W];
extern uint8  mt9v03x_image_2[MT9V03X_2_H][MT9V03X_2_W];
extern uint32 mt9v03x_fps[2];
extern int16  mt9v03x_set_confing_buffer_1[MT9V03X_DOUBLE_CONFIG_FINISH][2];
extern int16  mt9v03x_set_confing_buffer_2[MT9V03X_DOUBLE_CONFIG_FINISH][2];

#define mt9v03x_finish_flag mt9v03x_finish_flag_1
#define mt9v03x_image       mt9v03x_image_1

uint8 mt9v03x_double_init(m9v03x_double_init_type_enum init_mode);
uint8 mt9v03x_set_exposure_time_sccb(m9v03x_double_init_type_enum init_type, unsigned short int light);

static inline uint8 mt9v03x_init(void)
{
    return mt9v03x_double_init(mt9v03x_1);
}

static inline uint8 mt9v03x_set_exposure_time(uint16 light)
{
    return mt9v03x_set_exposure_time_sccb(mt9v03x_1, light);
}

#endif
