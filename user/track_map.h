/*********************************************************************************************************************
 * 疯狂电路组 - 赛道节点建模
 *
 * 核心思路（逐飞方案 2.2 节点判断与转向策略）：
 *   官方提前两周公布赛道 → 把节点序列和转向动作提前写死
 *   运行时识别"路径数变化"触发 current_step++，按预设动作选转向
 *
 * 改赛道时：只需改 track_map.c 里的 track_path[] 数组
 ********************************************************************************************************************/
#ifndef TRACK_MAP_H
#define TRACK_MAP_H

#include "zf_common_typedef.h"

typedef enum {
    NODE_START = 0,        /* 起点（开关）等待发车 */
    NODE_STRAIGHT,         /* 直行通过 */
    NODE_LEFT_TURN,        /* 弯道左拐 */
    NODE_RIGHT_TURN,       /* 弯道右拐 */
    NODE_T_LEFT,           /* T 字口选左 */
    NODE_T_RIGHT,          /* T 字口选右 */
    NODE_T_STRAIGHT,       /* T 字口保持直行 */
    NODE_CROSS_LEFT,       /* 十字选左 */
    NODE_CROSS_STRAIGHT,   /* 十字保持直 */
    NODE_CROSS_RIGHT,      /* 十字选右 */
    NODE_ELEMENT,          /* 通过元件（保持直行不干扰） */
    NODE_END               /* 终点（回到起点开关） */
} node_action_t;

typedef struct {
    node_action_t action;
    int16         expect_path_count;  /* 预期路径数（3=路口, 1=单线, 2=元件） */
    int16         steer_bias;         /* 固定转向偏置（单位：像素误差） */
    const char   *desc;               /* 中文说明（调试用） */
} path_step_t;

extern const path_step_t track_path[];
extern const int32       track_path_length;

#endif
