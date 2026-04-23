/*********************************************************************************************************************
 * 疯狂电路组 - 应用层主逻辑（状态机）
 *
 * 对外暴露 3 个钩子：
 *   app_init()         — 初始化（在 cpu0_main 的初始化阶段调一次）
 *   app_on_new_frame() — 每帧图像到达时调（主循环里 if(mt9v03x_finish_flag_1)）
 *   app_on_10ms()      — 10ms PIT 中断里调（速度控制 + 超时保护）
 ********************************************************************************************************************/
#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "zf_common_typedef.h"
#include "config.h"

typedef enum {
    STATE_INIT = 0,
    STATE_WAIT_START,      /* 等按键/超时发车 */
    STATE_RUNNING,         /* 比赛中 */
    STATE_AT_NODE,         /* 节点中（降速通过） */
    STATE_FINISHED,        /* 完赛 */
    STATE_ERROR            /* 异常 */
} app_state_t;

extern volatile app_state_t app_state;
extern volatile int32       current_step;      /* 当前进行到 track_path 的哪一步 */
extern volatile uint32      race_start_ms;
extern volatile uint32      race_elapsed_ms;

void app_init       (void);
void app_on_new_frame(const uint8 img[IMG_H][IMG_W]);
void app_on_10ms    (void);

/* 调试辅助：绘制图像+中线+边线到屏幕。app_on_new_frame 内部已经调用此函数，
   主循环一般不需要再调一次。保留 app_debug_draw() 作兼容 stub。 */
void app_debug_draw_with(const uint8 src[IMG_H][IMG_W]);
void app_debug_draw (void);

#endif
