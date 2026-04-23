/*********************************************************************************************************************
 * 疯狂电路组 - PID 控制器 实现
 ********************************************************************************************************************/
#include "pid_control.h"

/* ===================== 位置式 ===================== */
void pid_pos_init(pid_pos_t *p, float kp, float ki, float kd, float out_max)
{
    p->kp       = kp;
    p->ki       = ki;
    p->kd       = kd;
    p->out_max  = out_max;
    pid_pos_reset(p);
}

void pid_pos_reset(pid_pos_t *p)
{
    p->target   = 0.0f;
    p->last_err = 0.0f;
    p->integral = 0.0f;
}

float pid_pos_compute(pid_pos_t *p, float feedback)
{
    float err = p->target - feedback;
    p->integral += err;

    /* 积分抗饱和：限制 ki * integral 不超过输出上限 */
    if (p->ki > 1e-6f) {
        float integ_max = p->out_max / p->ki;
        if (p->integral >  integ_max) p->integral =  integ_max;
        if (p->integral < -integ_max) p->integral = -integ_max;
    }

    float out = p->kp * err
              + p->ki * p->integral
              + p->kd * (err - p->last_err);
    p->last_err = err;

    if (out >  p->out_max) out =  p->out_max;
    if (out < -p->out_max) out = -p->out_max;
    return out;
}

/* ===================== 增量式 ===================== */
void pid_inc_init(pid_inc_t *p, float kp, float ki, float kd, float out_max)
{
    p->kp      = kp;
    p->ki      = ki;
    p->kd      = kd;
    p->out_max = out_max;
    pid_inc_reset(p);
}

void pid_inc_reset(pid_inc_t *p)
{
    p->target = 0.0f;
    p->err[0] = p->err[1] = p->err[2] = 0.0f;
    p->out    = 0.0f;
}

float pid_inc_compute(pid_inc_t *p, float feedback)
{
    p->err[0] = p->err[1];
    p->err[1] = p->err[2];
    p->err[2] = p->target - feedback;

    /* 增量式经典公式：Δu = KP*(e_k - e_{k-1}) + KI*e_k + KD*(e_k - 2e_{k-1} + e_{k-2}) */
    float delta = p->kp * (p->err[2] - p->err[1])
                + p->ki *  p->err[2]
                + p->kd * (p->err[2] - 2.0f * p->err[1] + p->err[0]);

    p->out += delta;
    if (p->out >  p->out_max) p->out =  p->out_max;
    if (p->out < -p->out_max) p->out = -p->out_max;
    return p->out;
}
