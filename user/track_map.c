#include "track_map.h"

const path_step_t track_path[] = {
    /* action,          expect_paths, steer_bias, desc */
    { NODE_START,        1,           0, "start_wait"      },
    { NODE_STRAIGHT,     1,           0, "go_up_v1"        },
    { NODE_ELEMENT,      2,           0, "pass_power"      },
    { NODE_LEFT_TURN,    1,          -8, "left_top_turn"   },
    { NODE_STRAIGHT,     1,           0, "top_go_right"    },
    { NODE_ELEMENT,      2,           0, "pass_r1"         },
    { NODE_T_RIGHT,      3,           8, "t_right_to_pnp"  },
    { NODE_ELEMENT,      2,           0, "pass_t1"         },
    { NODE_STRAIGHT,     1,           0, "keep_right"      },
    { NODE_ELEMENT,      2,           0, "pass_r2"         },
    { NODE_RIGHT_TURN,   1,           8, "right_top_down"  },
    { NODE_ELEMENT,      2,           0, "pass_r3"         },
    { NODE_ELEMENT,      2,           0, "pass_r4"         },
    { NODE_ELEMENT,      2,           0, "pass_led_d1"     },
    { NODE_RIGHT_TURN,   1,           8, "right_bottom"    },
    { NODE_STRAIGHT,     1,           0, "bottom_go_left"  },
    { NODE_END,          1,           0, "finish"          },
};

const int32 track_path_length = sizeof(track_path) / sizeof(track_path[0]);