#ifndef _zf_device_config_h_
#define _zf_device_config_h_

extern const unsigned char imu660ra_config_file[8192];
extern const unsigned char dl1b_config_file[135];

void          mt9v03x_sccb_check_id_1          (void *soft_iic_obj);
unsigned char mt9v03x_set_config_sccb_1        (void *soft_iic_obj, short int buff[10][2]);
unsigned char mt9v03x_set_exposure_time_sccb_1 (unsigned short int light);
unsigned char mt9v03x_set_reg_sccb_1           (unsigned char addr, unsigned short int data);

void          mt9v03x_sccb_check_id_2          (void *soft_iic_obj);
unsigned char mt9v03x_set_config_sccb_2        (void *soft_iic_obj, short int buff[10][2]);
unsigned char mt9v03x_set_exposure_time_sccb_2 (unsigned short int light);
unsigned char mt9v03x_set_reg_sccb_2           (unsigned char addr, unsigned short int data);

unsigned char scc8660_set_config_sccb          (void *soft_iic_obj, short int buff[10][2]);
unsigned char scc8660_set_brightness_sccb      (unsigned short int brightness);
unsigned char scc8660_set_manual_wb_sccb       (unsigned short int manual_wb);
unsigned char scc8660_set_reg_sccb             (unsigned char reg, unsigned short int data);

#endif