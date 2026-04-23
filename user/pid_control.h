/*********************************************************************************************************************
 * 疯狂电路组 - PID 控制器
 * 提供：位置式 PID（用于转向）、增量式 PID（用于速度）
 ********************************************************************************************************************/
#ifndef PID_CONTROL_H
#define PID_CONTROL_H

#include "zf_common_typedef.h"

/* 位置式 PID */
typedef struct {
    float kp, ki, kd;
    float target;
    float last_err;
    float integral;
    float out_max;         /* 绝对值上限 */
} pid_pos_t;

/* 增量式 PID */
typedef struct {
    float kp, ki, kd;
    float target;
    float err[3];          /* err[0]=k-2, err[1]=k-1, err[2]=k */
    float out;             /* 累加的输出（带限幅） */
    float out_max;
} pid_inc_t;

void  pid_pos_init   (pid_pos_t *p, float kp, float ki, float kd, float out_max);
void  pid_pos_reset  (pid_pos_t *p);
float pid_pos_compute(pid_pos_t *p, float feedback);

void  pid_inc_init   (pid_inc_t *p, float kp, float ki, float kd, float out_max);
void  pid_inc_reset  (pid_inc_t *p);
float pid_inc_compute(pid_inc_t *p, float feedback);

#endif
