/*
    File: joystick_maps.c
    Author: Philip Haynes
*/

#include "joystick_maps.h"

#include <linux/input-event-codes.h>

const char* jsAllAxes[] = {
    "0x000 - ABS_X",                // 0x00     00
    "0x001 - ABS_Y",                // 0x01     01
    "0x002 - ABS_Z",                // 0x02     02
    "0x003 - ABS_RX",               // 0x03     03
    "0x004 - ABS_RY",               // 0x04     04
    "0x005 - ABS_RZ",               // 0x05     05
    "0x006 - ABS_THROTTLE",         // 0x06     06
    "0x007 - ABS_RUDDER",           // 0x07     07
    "0x008 - ABS_WHEEL",            // 0x08     08
    "0x009 - ABS_GAS",              // 0x09     09
    "0x00a - ABS_BRAKE",            // 0x0a     10
    "0x010 - ABS_HAT0X",            // 0x10     11
    "0x011 - ABS_HAT0Y",            // 0x11     12
    "0x012 - ABS_HAT1X",            // 0x12     13
    "0x013 - ABS_HAT1Y",            // 0x13     14
    "0x014 - ABS_HAT2X",            // 0x14     15
    "0x015 - ABS_HAT2Y",            // 0x15     16
    "0x016 - ABS_HAT3X",            // 0x16     17
    "0x017 - ABS_HAT3Y",            // 0x17     18
    "0x018 - ABS_PRESSURE",         // 0x18     19
    "0x019 - ABS_DISTANCE",         // 0x19     20
    "0x01a - ABS_TILT_X",           // 0x1a     21
    "0x01b - ABS_TILT_Y",           // 0x1b     22
    "0x01c - ABS_TOOL_WIDTH",       // 0x1c     23
};

const char* jsAllBtns[] = {
    "0x120 - BTN_TRIGGER",          // 0x120    00
    "0x121 - BTN_THUMB",            // 0x121    01
    "0x122 - BTN_THUMB2",           // 0x122    02
    "0x123 - BTN_TOP",              // 0x123    03
    "0x124 - BTN_TOP2",             // 0x124    04
    "0x125 - BTN_PINKIE",           // 0x125    05
    "0x126 - BTN_BASE",             // 0x126    06
    "0x127 - BTN_BASE2",            // 0x127    07
    "0x128 - BTN_BASE3",            // 0x128    08
    "0x129 - BTN_BASE4",            // 0x129    09
    "0x12a - BTN_BASE5",            // 0x12a    10
    "0x12b - BTN_BASE6",            // 0x12b    11
    "0x12f - BTN_DEAD",             // 0x12f    12
    "0x130 - BTN_SOUTH",            // 0x130    13
    "0x131 - BTN_EAST",             // 0x131    14
    "0x132 - BTN_C",                // 0x132    15
    "0x133 - BTN_NORTH",            // 0x133    16
    "0x134 - BTN_WEST",             // 0x134    17
    "0x135 - BTN_Z",                // 0x135    18
    "0x136 - BTN_TL",               // 0x136    19
    "0x137 - BTN_TR",               // 0x137    20
    "0x138 - BTN_TL2",              // 0x138    21
    "0x139 - BTN_TR2",              // 0x139    22
    "0x13a - BTN_SELECT",           // 0x13a    23
    "0x13b - BTN_START",            // 0x13b    24
    "0x13c - BTN_MODE",             // 0x13c    25
    "0x13d - BTN_THUMBL",           // 0x13d    26
    "0x13e - BTN_THUMBR",           // 0x13e    27
    "0x13f - BTN_NULL",             // 0x13f    28
    "0x140 - BTN_TOOL_PEN",         // 0x140    29
    "0x141 - BTN_TOOL_RUBBER",      // 0x141    30
    "0x142 - BTN_TOOL_BRUSH",       // 0x142    31
    "0x143 - BTN_TOOL_PENCIL",      // 0x143    32
    "0x144 - BTN_TOOL_AIRBRUSH",    // 0x144    33
    "0x145 - BTN_TOOL_FINGER",      // 0x145    34
    "0x146 - BTN_TOOL_MOUSE",       // 0x146    35
    "0x147 - BTN_TOOL_LENS",        // 0x147    36
    "0x148 - BTN_TOOL_QUINTTAP",    // 0x148    37
    "0x149 - BTN_STYLUS3",          // 0x149    38
    "0x14a - BTN_TOUCH",            // 0x14a    39
    "0x14b - BTN_STYLUS",           // 0x14b    40
    "0x14c - BTN_STYLUS2",          // 0x14c    41
    "0x14d - BTN_TOOL_DOUBLETAP",   // 0x14d    42
    "0x14e - BTN_TOOL_TRIPLETAP",   // 0x14e    43
    "0x14f - BTN_TOOL_QUADTAP",     // 0x14f    44
    "0x150 - BTN_GEAR_DOWN",        // 0x150    45
    "0x151 - BTN_GEAR_UP",          // 0x151    46
    "0x220 - BTN_DPAD_UP",          // 0x220    47
    "0x221 - BTN_DPAD_DOWN",        // 0x221    48
    "0x222 - BTN_DPAD_LEFT",        // 0x222    49
    "0x223 - BTN_DPAD_RIGHT",       // 0x223    50
    "Disabled",                     //          51
};

int jsAxisMapToAll(int i) {
    const int disabled = JS_ALL_AXES_COUNT - 1;
    if (i <= ABS_BRAKE) {
        return i;
    } else if (i < ABS_HAT0X) {
        return disabled;
    } else if (i <= ABS_TOOL_WIDTH) {
        return i - ABS_HAT0X + 11;
    } else {
        return disabled;
    }
}

int jsAxisMapFromAll(int i) {
    if (i < 0) {
        return -1;
    } else if (i < 11) {
        return i;
    } else if (i < 24) {
        return i + ABS_HAT0X - 11;
    } else {
        return -1;
    }
}

int jsButtonMapToAll(int i) {
    const int disabled = JS_ALL_BTNS_COUNT - 1;
    if (i < BTN_JOYSTICK) {
        return disabled;
    } else if (i <= BTN_BASE6) {
        return i - BTN_JOYSTICK;
    } else if (i < BTN_DEAD) {
        return disabled;
    } else if (i <= BTN_GEAR_UP) {
        return i - BTN_DEAD + 12;
    } else if (i < BTN_DPAD_UP) {
        return disabled;
    } else if (i <= BTN_DPAD_RIGHT) {
        return i - BTN_DPAD_UP + 47;
    } else {
        return disabled;
    }
}

int jsButtonMapFromAll(int i) {
    if (i < 0) {
        return -1;
    } else if (i < 12) {
        return i + BTN_JOYSTICK;
    } else if (i < 47) {
        return i + BTN_DEAD - 12;
    } else if (i < 51) {
        return i + BTN_DPAD_UP - 47;
    } else {
        return -1;
    }
}

const char *jsJoystickAxes[7] = {
    "X-Axis (Roll)",
    "Y-Axis (Pitch)",
    "Z-Axis (Yaw)",
    "Throttle",
    "Hat X-Axis",
    "Hat Y-Axis",
    "Disabled"
};

int jsAxisMapToJoystick(int i) {
    const int disabled = JS_JOYSTICK_AXES_COUNT - 1;
    if (i <= ABS_Z) {
        return i;
    } else if (i < ABS_THROTTLE) {
        return disabled;
    } else if (i == ABS_THROTTLE) {
        return 3;
    } else if (i < ABS_HAT0X) {
        return disabled;
    } else if (i <= ABS_HAT0Y) {
        return i-ABS_HAT0X+4;
    } else {
        return disabled;
    }
}

int jsAxisMapFromJoystick(int i) {
    if (i < 0) {
        return -1;
    } else if (i <= 2) {
        return i;
    } else if (i == 3) {
        return ABS_THROTTLE;
    } else if (i < 6) {
        return i-4+ABS_HAT0X;
    } else {
        return -1;
    }
}

const char *jsJoystickBtns[14] = {
    "Trigger",
    "Thumb 1",
    "Thumb 2",
    "Top 1",
    "Top 2",
    "Pinkie",
    "Base 1",
    "Base 2",
    "Base 3",
    "Base 4",
    "Base 5",
    "Base 6",
    "Dead",
    "Disabled"
};

int jsButtonMapToJoystick(int i) {
    const int disabled = JS_JOYSTICK_BTNS_COUNT - 1;
    if (i < BTN_TRIGGER) {
        return disabled;
    } else if (i <= BTN_BASE6) {
        return i-BTN_TRIGGER;
    } else if (i == BTN_DEAD) {
        return 12;
    } else {
        return disabled;
    }
}

int jsButtonMapFromJoystick(int i) {
    if (i < 0) {
        return -1;
    } else if (i < 12) {
        return i+BTN_TRIGGER;
    } else if (i == 13) {
        return BTN_DEAD;
    } else {
        return -1;
    }
}

const char *jsGamepadAxes[9] = {
    "Left Stick X",
    "Left Stick Y",
    "Left Trigger",
    "Right Stick X",
    "Right Stick Y",
    "Right Trigger",
    "D-Pad X",
    "D-Pad Y",
    "Disabled"
};

int jsAxisMapToGamepad(int i) {
    const int disabled = JS_GAMEPAD_AXES_COUNT - 1;
    if (i < 0) {
        return disabled;
    } else if (i <= ABS_RZ) {
        return i;
    } else if (i < ABS_HAT0X) {
        return disabled;
    } else if (i <= ABS_HAT0Y) {
        return i-ABS_HAT0X+6;
    } else {
        return disabled;
    }
}

int jsAxisMapFromGamepad(int i) {
    if (i < 0) {
        return -1;
    } else if (i <= 6) {
        return i;
    } else if (i < 8) {
        return i-6+ABS_HAT0X;
    } else {
        return -1;
    }
}

const char *jsGamepadBtns[16] = {
    "South (A or Cross)",
    "East (B or Circle)",
    "C (Old-Fashioned)",
    "North (Y or Triangle)",
    "West (X or Square)",
    "Z (Old-Fashioned)",
    "Left Index (Trigger or Bumper)",
    "Right Index (Trigger or Bumper)",
    "Left Index 2 (Trigger)",
    "Right Index 2 (Trigger)",
    "Select or Back",
    "Start",
    "Mode (Central Button)",
    "Left Thumbstick Press",
    "Right Thumbstick Press",
    "Disabled"
};

int jsButtonMapToGamepad(int i) {
    const int disabled = JS_GAMEPAD_BTNS_COUNT - 1;
    if (i < BTN_SOUTH) {
        return disabled;
    } else if (i <= BTN_THUMBR) {
        return i-BTN_SOUTH;
    } else {
        return disabled;
    }
}

int jsButtonMapFromGamepad(int i) {
    if (i < 0) {
        return -1;
    } else if (i < 15) {
        return i+BTN_SOUTH;
    } else {
        return -1;
    }
}

const char *jsWheelAxes[6] = {
    "Wheel",
    "Gas Pedal",
    "Brake Pedal",
    "D-Pad X",
    "D-Pad Y",
    "Disabled"
};

int jsAxisMapToWheel(int i) {
    const int disabled = JS_WHEEL_AXES_COUNT - 1;
    if (i < ABS_WHEEL) {
        return disabled;
    } else if (i <= ABS_BRAKE) {
        return i-ABS_WHEEL;
    } else if (i < ABS_HAT0X) {
        return disabled;
    } else if (i <= ABS_HAT0Y) {
        return i-ABS_HAT0X+3;
    } else {
        return disabled;
    }
}

int jsAxisMapFromWheel(int i) {
    if (i < 0) {
        return -1;
    } else if (i < 3) {
        return i+ABS_WHEEL;
    } else if (i < 5) {
        return i+ABS_HAT0X-3;
    } else {
        return -1;
    }
}

const char *jsWheelBtns[16] = {
    "South (A or Cross)",
    "East (B or Circle)",
    "North (Y or Triangle)",
    "West (X or Square)",
    "L1",
    "R1",
    "L2",
    "R2",
    "Select (Share)",
    "Start",
    "Mode (Central Button)",
    "L3", // This is a guess
    "R3", // Maybe these are supposed to be C and Z?
    "Gear Down",
    "Gear Up",
    "Disabled"
};

int jsButtonMapToWheel(int i) {
    const int disabled = JS_WHEEL_BTNS_COUNT - 1;
    if (i < BTN_SOUTH) {
        return disabled;
    } else if (i <= BTN_EAST) {
        return i-BTN_SOUTH;
    } else if (i < BTN_NORTH) {
        return disabled;
    } else if (i <= BTN_WEST) {
        return i-BTN_NORTH+2;
    } else if (i < BTN_TL) {
        return disabled;
    } else if (i <= BTN_THUMBR) {
        return i-BTN_TL+4;
    } else if (i < BTN_GEAR_DOWN) {
        return disabled;
    } else if (i <= BTN_GEAR_UP) {
        return i-BTN_GEAR_DOWN+13;
    } else {
        return disabled;
    }
}

int jsButtonMapFromWheel(int i) {
    if (i < 0) {
        return -1;
    } else if (i < 2) {
        return i+BTN_SOUTH;
    } else if (i < 4) {
        return i+BTN_NORTH-2;
    } else if (i < 13) {
        return i+BTN_TL-4;
    } else if (i < 15) {
        return i+BTN_GEAR_DOWN-13;
    } else {
        return -1;
    }
}
