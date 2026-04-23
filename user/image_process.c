/*********************************************************************************************************************
 * 疯狂电路组 - 图像处理 实现（无 binary 副本版 —— 省 22.5KB RAM）
 ********************************************************************************************************************/
#include "image_process.h"

image_process_t img_state;

/*--------------------------------------------------------------------------------------------------------------------
 *  大津法（Otsu）动态阈值
 *------------------------------------------------------------------------------------------------------------------*/
uint8 otsu_threshold(const uint8 *src, int32 length)
{
    int32 hist[256] = {0};
    for (int32 i = 0; i < length; i++) hist[src[i]]++;

    int64 sum_total = 0;
    for (int32 i = 0; i < 256; i++) sum_total += (int64)i * hist[i];

    int32 weight_bg = 0;
    int64 sum_bg = 0;
    float var_max = 0.0f;
    uint8 threshold = 128;

    for (int32 t = 0; t < 256; t++) {
        weight_bg += hist[t];
        if (weight_bg == 0) continue;
        int32 weight_fg = length - weight_bg;
        if (weight_fg == 0) break;

        sum_bg += (int64)t * hist[t];
        float mean_bg = (float)sum_bg / weight_bg;
        float mean_fg = (float)(sum_total - sum_bg) / weight_fg;

        float var_between = (float)weight_bg * weight_fg *
                            (mean_bg - mean_fg) * (mean_bg - mean_fg);
        if (var_between > var_max) {
            var_max = var_between;
            threshold = (uint8)t;
        }
    }
    return threshold;
}

/*--------------------------------------------------------------------------------------------------------------------
 *  计算本帧阈值 + 统计白像素数
 *------------------------------------------------------------------------------------------------------------------*/
void compute_threshold(const uint8 src[IMG_H][IMG_W])
{
#if BINARY_THRESH_FIXED
    img_state.threshold = BINARY_THRESH_VALUE;
#else
    img_state.threshold = otsu_threshold((const uint8 *)src, IMG_H * IMG_W);
#endif

    uint32 white_cnt = 0;
    for (int32 y = 0; y < IMG_H; y++)
        for (int32 x = 0; x < IMG_W; x++)
            if (src[y][x] > img_state.threshold) white_cnt++;
    img_state.white_pixels = white_cnt;
}

/*--------------------------------------------------------------------------------------------------------------------
 *  边线提取：基于原图 + 阈值直接比较
 *------------------------------------------------------------------------------------------------------------------*/
void scan_edges(const uint8 src[IMG_H][IMG_W])
{
    int16 base_mid = IMG_MID_COL;

    for (int32 y = IMG_H - 1; y >= 0; y--) {
        int16 left = 0;
        uint8 lost_l = 1;
        for (int16 x = base_mid; x > 0; x--) {
            if (IS_WHITE(src, y, x) && !IS_WHITE(src, y, x - 1)) {
                left = x - 1;
                lost_l = 0;
                break;
            }
        }
        img_state.left_edge[y]  = left;
        img_state.lost_left[y]  = lost_l;

        int16 right = IMG_W - 1;
        uint8 lost_r = 1;
        for (int16 x = base_mid; x < IMG_W - 1; x++) {
            if (IS_WHITE(src, y, x) && !IS_WHITE(src, y, x + 1)) {
                right = x + 1;
                lost_r = 0;
                break;
            }
        }
        img_state.right_edge[y] = right;
        img_state.lost_right[y] = lost_r;

        if (lost_l && lost_r) {
            img_state.center[y] = base_mid;
        } else if (lost_l) {
            img_state.center[y] = right - EDGE_COMPENSATE;
            if (img_state.center[y] < 0) img_state.center[y] = 0;
        } else if (lost_r) {
            img_state.center[y] = left + EDGE_COMPENSATE;
            if (img_state.center[y] >= IMG_W) img_state.center[y] = IMG_W - 1;
        } else {
            img_state.center[y] = (left + right) / 2;
            base_mid = img_state.center[y];
        }
    }
}

/*--------------------------------------------------------------------------------------------------------------------
 *  加权中线误差
 *------------------------------------------------------------------------------------------------------------------*/
void calc_center_error(void)
{
    int32 weighted_sum = 0;
    int32 weight_total = 0;

    for (int32 y = SCAN_ROW_NEAR; y >= SCAN_ROW_FAR; y -= SCAN_ROW_STEP) {
        int32 weight = ERR_WEIGHT_FAR +
                       ((ERR_WEIGHT_NEAR - ERR_WEIGHT_FAR) * (y - SCAN_ROW_FAR))
                       / (SCAN_ROW_NEAR - SCAN_ROW_FAR);
        weighted_sum += (img_state.center[y] - IMG_MID_COL) * weight;
        weight_total += weight;
    }
    img_state.center_error = (weight_total > 0) ? (int16)(weighted_sum / weight_total) : 0;
}

/*--------------------------------------------------------------------------------------------------------------------
 *  某一行白色团块数（原图 + 阈值）
 *------------------------------------------------------------------------------------------------------------------*/
static int16 count_white_blobs_on_row(const uint8 src[IMG_H][IMG_W], int32 y)
{
    int16 count = 0;
    uint8 in_white = 0;
    for (int32 x = 0; x < IMG_W; x++) {
        if (IS_WHITE(src, y, x)) {
            if (!in_white) { count++; in_white = 1; }
        } else {
            in_white = 0;
        }
    }
    return count;
}

void count_paths(const uint8 src[IMG_H][IMG_W])
{
    img_state.path_count     = count_white_blobs_on_row(src, PATH_SCAN_ROW_BOTTOM);
    img_state.path_count_far = count_white_blobs_on_row(src, PATH_SCAN_ROW_TOP);
}

/*--------------------------------------------------------------------------------------------------------------------
 *  一帧完整流水线
 *------------------------------------------------------------------------------------------------------------------*/
void process_frame(const uint8 src[IMG_H][IMG_W])
{
    compute_threshold(src);
    scan_edges(src);
    calc_center_error();
    count_paths(src);
}
