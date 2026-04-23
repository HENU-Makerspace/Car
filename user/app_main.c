#include "app_main.h"
#include "image_process.h"
#include "pid_control.h"
#include "track_map.h"
#include "zf_common_headfile.h"

extern volatile int16  encoder_count_l;
extern volatile int16  encoder_count_r;
extern volatile uint32 ms_tick;
extern void motor_drive(int32 forward, int32 steer);

volatile app_state_t app_state       = STATE_INIT;
volatile int32       current_step    = 0;
volatile uint32      race_start_ms   = 0;
volatile uint32      race_elapsed_ms = 0;

static pid_pos_t s_steer_pid;
static pid_inc_t s_speed_pid;
static uint32    s_start_boost_until_ms = 0;
static int16     s_prev_path_count      = 0;
static int32     s_path_hold_frames     = 0;
static int32     s_at_node_frames       = 0;
static int16     s_current_speed_target = 0;

static void race_start(void);
static void race_finish(void);
static void node_transition_fsm(void);
static int16 get_steer_bias(void);

void app_init(void)
{
    pid_pos_init(&s_steer_pid, STEER_KP, STEER_KI, STEER_KD, (float)STEER_OUT_MAX);
    pid_inc_init(&s_speed_pid, SPEED_KP, SPEED_KI, SPEED_KD, (float)APP_MAX_DUTY);

    pid_pos_reset(&s_steer_pid);
    pid_inc_reset(&s_speed_pid);

    s_current_speed_target = 0;
    s_prev_path_count      = 0;
    s_path_hold_frames     = 0;
    s_at_node_frames       = 0;

    current_step    = 0;
    app_state       = STATE_WAIT_START;
    race_start_ms   = 0;
    race_elapsed_ms = 0;

    key_init(10);
    key_clear_all_state();
    motor_drive(0, 0);
}

static void race_start(void)
{
    race_start_ms = ms_tick * 5;
    current_step  = 1;
    s_current_speed_target = SPEED_TARGET_SLOW;
    s_start_boost_until_ms = race_start_ms + MOTOR_START_BOOST_MS;

    pid_pos_reset(&s_steer_pid);
    pid_inc_reset(&s_speed_pid);
    s_speed_pid.target = (float)s_current_speed_target;

    motor_drive(0, 0);
    app_state = STATE_RUNNING;
}

static void race_finish(void)
{
    motor_drive(0, 0);
    s_current_speed_target = 0;
    s_start_boost_until_ms = 0;
    pid_pos_reset(&s_steer_pid);
    pid_inc_reset(&s_speed_pid);
    app_state = STATE_FINISHED;
}

static void node_transition_fsm(void)
{
    int16 paths = img_state.path_count;

    if (app_state == STATE_RUNNING)
    {
        if (paths >= PATH_COUNT_T_JUNCTION && s_prev_path_count < PATH_COUNT_T_JUNCTION)
        {
            s_path_hold_frames = 1;
        }
        else if (paths >= PATH_COUNT_T_JUNCTION)
        {
            s_path_hold_frames++;
            if (s_path_hold_frames >= NODE_HOLD_FRAMES)
            {
                app_state = STATE_AT_NODE;
                s_at_node_frames = 6;
                s_current_speed_target = (SPEED_TARGET_NORMAL * NODE_SPEED_FACTOR) / 100;
                s_speed_pid.target = (float)s_current_speed_target;
            }
        }
        else
        {
            s_path_hold_frames = 0;
        }
    }
    else if (app_state == STATE_AT_NODE)
    {
        if (--s_at_node_frames <= 0)
        {
            current_step++;
            s_path_hold_frames = 0;
            s_current_speed_target = SPEED_TARGET_NORMAL;
            s_speed_pid.target = (float)s_current_speed_target;

            if (current_step >= track_path_length - 1 || track_path[current_step].action == NODE_END)
            {
                race_finish();
                return;
            }
            app_state = STATE_RUNNING;
        }
    }

    s_prev_path_count = paths;
}

static int16 get_steer_bias(void)
{
    if (current_step < 0 || current_step >= track_path_length)
    {
        return 0;
    }
    return track_path[current_step].steer_bias;
}

void app_on_new_frame(const uint8 img[IMG_H][IMG_W])
{
    process_frame(img);
    app_debug_draw_with(img);

    if (app_state != STATE_RUNNING && app_state != STATE_AT_NODE)
    {
        motor_drive(0, 0);
        return;
    }

    node_transition_fsm();

    {
        float err = (float)img_state.center_error + (float)get_steer_bias();
        int32 avg_enc = (encoder_count_l + encoder_count_r) / 2;
        int32 forward_cmd;
        float steer_out;
        float speed_out;

        if (err > -(float)STEER_DEADZONE && err < (float)STEER_DEADZONE)
        {
            err = 0.0f;
        }

        s_steer_pid.target = 0.0f;
        steer_out = pid_pos_compute(&s_steer_pid, -err);
        speed_out = pid_inc_compute(&s_speed_pid, (float)avg_enc);
        forward_cmd = (int32)speed_out;

        if (forward_cmd > 0 && forward_cmd < MOTOR_MIN_FORWARD_DUTY)
        {
            forward_cmd = MOTOR_MIN_FORWARD_DUTY;
        }

        if ((ms_tick * 5) < s_start_boost_until_ms && forward_cmd < MOTOR_START_DUTY)
        {
            forward_cmd = MOTOR_START_DUTY;
        }

        motor_drive(forward_cmd, (int32)steer_out);
    }
}

void app_on_10ms(void)
{
    key_scanner();

    if (app_state == STATE_WAIT_START)
    {
        s_current_speed_target = 0;
        s_speed_pid.target = 0.0f;
        motor_drive(0, 0);

        if (KEY_SHORT_PRESS == key_get_state(KEY_1) || KEY_LONG_PRESS == key_get_state(KEY_1))
        {
            key_clear_state(KEY_1);
            race_start();
        }
        return;
    }

    if (app_state == STATE_RUNNING || app_state == STATE_AT_NODE)
    {
        race_elapsed_ms = ms_tick * 5 - race_start_ms;
        if (race_elapsed_ms >= RACE_TIMEOUT_MS)
        {
            race_finish();
        }
    }
}

#define IPS_W_MAX   240
#define IPS_H_MAX   135
#define COMPARE_RAW_X   0
#define COMPARE_BIN_X   96
#define COMPARE_Y       0
#define COMPARE_W       94
#define COMPARE_H       60

void app_debug_draw_with(const uint8 src[IMG_H][IMG_W])
{
#if DEBUG_DRAW_CENTER || DEBUG_DRAW_EDGES
    int32 draw_x_max = IPS_W_MAX;
    int32 draw_y_max = IPS_H_MAX;

#if DEBUG_DRAW_COMPARE
    ips114_show_gray_image(COMPARE_RAW_X, COMPARE_Y, (const uint8 *)src, IMG_W, IMG_H, COMPARE_W, COMPARE_H, 0);
    ips114_show_gray_image(COMPARE_BIN_X, COMPARE_Y, (const uint8 *)src, IMG_W, IMG_H, COMPARE_W, COMPARE_H, img_state.threshold);
    ips114_show_string(0,   0, "RAW");
    ips114_show_string(96,  0, "BIN");
    ips114_show_string(192,  0, "T");
    ips114_show_uint  (200,  0, img_state.threshold, 3);
    ips114_show_string(192, 16, "W");
    ips114_show_uint  (200, 16, img_state.white_pixels, 5);
    ips114_show_string(192, 32, "E");
    ips114_show_int   (200, 32, img_state.center_error, 4);
    ips114_show_string(192, 48, "P");
    ips114_show_uint  (200, 48, img_state.path_count, 2);

    draw_x_max = COMPARE_BIN_X + COMPARE_W;
    draw_y_max = COMPARE_Y + COMPARE_H;
#else
    ips114_show_gray_image(0, 0, (const uint8 *)src, IMG_W, IMG_H, IMG_W, IMG_H, img_state.threshold);
    ips114_show_string(188,  0, "T");
    ips114_show_uint  (196,  0, img_state.threshold, 3);
    ips114_show_string(188, 16, "W");
    ips114_show_uint  (196, 16, img_state.white_pixels, 5);
    ips114_show_string(188, 32, "E");
    ips114_show_int   (196, 32, img_state.center_error, 4);
    ips114_show_string(188, 48, "P");
    ips114_show_uint  (196, 48, img_state.path_count, 2);
#endif

#if DEBUG_DRAW_CENTER
    {
        int32 y;
        for (y = 0; y < IMG_H; y += 2)
        {
            int16 cx = img_state.center[y];
            int16 draw_x;
            int16 draw_y;

#if DEBUG_DRAW_COMPARE
            draw_x = (int16)(COMPARE_BIN_X + ((int32)cx * COMPARE_W) / IMG_W);
            draw_y = (int16)(COMPARE_Y + ((int32)y * COMPARE_H) / IMG_H);
#else
            draw_x = cx;
            draw_y = (int16)y;
#endif

            if (draw_x >= 0 && draw_x < draw_x_max && draw_y >= 0 && draw_y < draw_y_max)
            {
                ips114_draw_point((uint16)draw_x, (uint16)draw_y, RGB565_RED);
            }
        }
    }
#endif

#if DEBUG_DRAW_EDGES
    {
        int32 y;
        for (y = 0; y < IMG_H; y += 2)
        {
            int16 draw_y;

#if DEBUG_DRAW_COMPARE
            draw_y = (int16)(COMPARE_Y + ((int32)y * COMPARE_H) / IMG_H);
#else
            draw_y = (int16)y;
#endif

            if (draw_y >= draw_y_max)
            {
                break;
            }
            if (!img_state.lost_left[y])
            {
                int16 lx = img_state.left_edge[y];
                int16 draw_x;

#if DEBUG_DRAW_COMPARE
                draw_x = (int16)(COMPARE_BIN_X + ((int32)lx * COMPARE_W) / IMG_W);
#else
                draw_x = lx;
#endif

                if (draw_x >= 0 && draw_x < draw_x_max)
                {
                    ips114_draw_point((uint16)draw_x, (uint16)draw_y, RGB565_BLUE);
                }
            }
            if (!img_state.lost_right[y])
            {
                int16 rx = img_state.right_edge[y];
                int16 draw_x;

#if DEBUG_DRAW_COMPARE
                draw_x = (int16)(COMPARE_BIN_X + ((int32)rx * COMPARE_W) / IMG_W);
#else
                draw_x = rx;
#endif

                if (draw_x >= 0 && draw_x < draw_x_max)
                {
                    ips114_draw_point((uint16)draw_x, (uint16)draw_y, RGB565_GREEN);
                }
            }
        }
    }
#endif
#endif
}

void app_debug_draw(void)
{
}
