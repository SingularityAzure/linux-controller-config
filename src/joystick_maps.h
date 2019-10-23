/*
    File: joystick_maps.h
    Author: Philip Haynes
    Provides functions that map human-readable control names
    to and from the linux/input-event-codes.h values.
*/

#ifndef JOYSTICK_MAPS_H
#define JOYSTICK_MAPS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*fpJsMapFunc)(int);

#define JS_ALL_AXES_COUNT 24
#define JS_ALL_BTNS_COUNT 52
extern const char* jsAllAxes[JS_ALL_AXES_COUNT];
extern const char* jsAllBtns[JS_ALL_BTNS_COUNT];

/*  Takes in a mapped axis index as reported by
the driver and provides the index into the above array.    */
int jsAxisMapToAll(int i);
/*  Effectively does the opposite of the above function,
returning -1 if the input is out of range.
This error is reported for "Disabled" as well, so you
have to account for it.                                     */
int jsAxisMapFromAll(int i);
int jsButtonMapToAll(int i);
int jsButtonMapFromAll(int i);

// NOTE: Someone will have to verify these
//       since I don't have a joystick to test with.
#define JS_JOYSTICK_AXES_COUNT 7
#define JS_JOYSTICK_BTNS_COUNT 14
extern const char* jsJoystickAxes[JS_JOYSTICK_AXES_COUNT];
extern const char* jsJoystickBtns[JS_JOYSTICK_BTNS_COUNT];

int jsAxisMapToJoystick(int i);
int jsAxisMapFromJoystick(int i);
int jsButtonMapToJoystick(int i);
int jsButtonMapFromJoystick(int i);

#define JS_GAMEPAD_AXES_COUNT 9
#define JS_GAMEPAD_BTNS_COUNT 20
extern const char* jsGamepadAxes[JS_GAMEPAD_AXES_COUNT];
extern const char* jsGamepadBtns[JS_GAMEPAD_BTNS_COUNT];

int jsAxisMapToGamepad(int i);
int jsAxisMapFromGamepad(int i);
int jsButtonMapToGamepad(int i);
int jsButtonMapFromGamepad(int i);

// NOTE: Someone will have to verify these as well.
#define JS_WHEEL_AXES_COUNT 6
#define JS_WHEEL_BTNS_COUNT 20
extern const char* jsWheelAxes[JS_WHEEL_AXES_COUNT];
extern const char* jsWheelBtns[JS_WHEEL_BTNS_COUNT];

int jsAxisMapToWheel(int i);
int jsAxisMapFromWheel(int i);
int jsButtonMapToWheel(int i);
int jsButtonMapFromWheel(int i);

#ifdef __cplusplus
}
#endif

#endif
