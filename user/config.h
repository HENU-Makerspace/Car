#ifndef FENGKUANG_CONFIG_H
#define FENGKUANG_CONFIG_H

/* image */
#define IMG_W                       188
#define IMG_H                       120
#define IMG_MID_COL                 (IMG_W / 2)
#define BINARY_THRESH_FIXED         1
#define BINARY_THRESH_VALUE         120

#define SCAN_ROW_NEAR               115
#define SCAN_ROW_FAR                20
#define SCAN_ROW_STEP               2
#define EDGE_COMPENSATE             30

#define PATH_SCAN_ROW_BOTTOM        118
#define PATH_SCAN_ROW_TOP           10

/* control */
#define APP_MAX_DUTY                1600
#define SPEED_TARGET_SLOW           160
#define SPEED_TARGET_NORMAL         180
#define SPEED_TARGET_FAST           260

#define STEER_KP                    1.8f
#define STEER_KI                    0.0f
#define STEER_KD                    8.0f
#define STEER_OUT_MAX               260
#define STEER_DEADZONE              4

#define SPEED_KP                    2.2f
#define SPEED_KI                    0.4f
#define SPEED_KD                    0.0f

#define MOTOR_START_BOOST_MS        300
#define MOTOR_START_DUTY            900
#define MOTOR_MIN_FORWARD_DUTY      520

#define MOTOR_LEFT_SIGN             1
#define MOTOR_RIGHT_SIGN            1
#define MOTOR_STEER_NO_REVERSE      1

#define ERR_WEIGHT_NEAR             8
#define ERR_WEIGHT_FAR              1

/* node detect */
#define PATH_COUNT_T_JUNCTION       3
#define PATH_COUNT_STRAIGHT         1
#define NODE_HOLD_FRAMES            3
#define NODE_SPEED_FACTOR           30

/* state machine */
#define START_WAIT_MS               2000
#define RACE_TIMEOUT_MS             180000

/* debug */
#define DEBUG_DRAW_COMPARE          1
#define DEBUG_DRAW_CENTER           1
#define DEBUG_DRAW_EDGES            1
#define DEBUG_PRINT_UART            0

#endif
