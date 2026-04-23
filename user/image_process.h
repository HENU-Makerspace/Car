/*********************************************************************************************************************
 * 疯狂电路组 - 图像处理
 * 二值化 / 边线提取 / 中线计算 / 路径数统计
 *
 * 内存优化：不保存二值图副本，只保存阈值，对原图灰度做 "> threshold" 比较
 *           节省 IMG_W * IMG_H = 22.5 KB RAM
 ********************************************************************************************************************/
#ifndef IMAGE_PROCESS_H
#define IMAGE_PROCESS_H

#include "zf_common_typedef.h"
#include "config.h"

typedef struct {
    int16  left_edge [IMG_H];          // 每行左边线列坐标
    int16  right_edge[IMG_H];          // 每行右边线列坐标
    int16  center    [IMG_H];          // 每行中线列坐标
    uint8  lost_left [IMG_H];          // 左边线是否丢失
    uint8  lost_right[IMG_H];          // 右边线是否丢失

    uint8  threshold;                  // 本帧使用的二值化阈值
    int16  center_error;               // 加权中线误差（正=偏右，负=偏左）
    int16  path_count;                 // 底部扫描行白色团块数（用于识别路口）
    int16  path_count_far;             // 远处扫描行白色团块数（提前预判）
    uint32 white_pixels;               // 本帧白像素总数（用于判图像异常）
} image_process_t;

extern image_process_t img_state;

/* 判定某像素是否"白"（相对当前阈值） */
#define IS_WHITE(src, y, x)   ((src)[y][x] > img_state.threshold)

/* 对外接口 —— 所有扫描类函数都接收原图指针 */
uint8 otsu_threshold(const uint8 *src, int32 length);
void  compute_threshold(const uint8 src[IMG_H][IMG_W]);
void  scan_edges      (const uint8 src[IMG_H][IMG_W]);
void  calc_center_error(void);
void  count_paths     (const uint8 src[IMG_H][IMG_W]);
void  process_frame   (const uint8 src[IMG_H][IMG_W]);

#endif
