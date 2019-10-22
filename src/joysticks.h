/*
    File: joysticks.h
    Author: Philip Haynes
    Just some tools to manage devices like "/dev/input/js0".
*/

#ifndef JOYSTICKS_H
#define JOYSTICKS_H

#include <linux/joystick.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    JS_OK = 0,
    JS_DEVICE_NOT_FOUND = -1,
    JS_INVALID_ARGUMENT = -2,
    JS_UNKNOWN_ERROR    = -3,
    JS_DEVICE_LOST      = -4,
};

const char* jsErrorString(int error);

#define JS_ALL_AXES_COUNT 24
#define JS_ALL_BTNS_COUNT 52
extern const char* jsAllAxes[JS_ALL_AXES_COUNT];
extern const char* jsAllBtns[JS_ALL_BTNS_COUNT];

typedef int (*fpJsMapFunc)(int);

/*  Takes in a mapped axis index as reported by
the driver and provides the index into the above array.    */
int jsAxisMapActualIndexToAll(int i);
/*  Effectively does the opposite of the above function,
returning JS_INVALID_ARGUMENT if the input is out of range.
This error is reported for "NOT_SUPPORTED" as well, so you
have to account for it.                                     */
int jsAxisMapAllIndexToActual(int i);

int jsButtonMapActualIndexToAll(int i);
int jsButtonMapAllIndexToActual(int i);

// NOTE: Someone will have to verify these
//       since I don't have a joystick to test with.
#define JS_JOYSTICK_AXES_COUNT 7
#define JS_JOYSTICK_BTNS_COUNT 14
extern const char* jsJoystickAxes[JS_JOYSTICK_AXES_COUNT];
extern const char* jsJoystickBtns[JS_JOYSTICK_BTNS_COUNT];

int jsAxisMapActualIndexToJoystick(int i);
int jsAxisMapJoystickIndexToActual(int i);
int jsButtonMapActualIndexToJoystick(int i);
int jsButtonMapJoystickIndexToActual(int i);

#define JS_GAMEPAD_AXES_COUNT 9
#define JS_GAMEPAD_BTNS_COUNT 16
extern const char* jsGamepadAxes[JS_GAMEPAD_AXES_COUNT];
extern const char* jsGamepadBtns[JS_GAMEPAD_BTNS_COUNT];

int jsAxisMapActualIndexToGamepad(int i);
int jsAxisMapGamepadIndexToActual(int i);
int jsButtonMapActualIndexToGamepad(int i);
int jsButtonMapGamepadIndexToActual(int i);

// NOTE: Someone will have to verify these as well.
#define JS_WHEEL_AXES_COUNT 6
#define JS_WHEEL_BTNS_COUNT 16
extern const char* jsWheelAxes[JS_WHEEL_AXES_COUNT];
extern const char* jsWheelBtns[JS_WHEEL_BTNS_COUNT];

int jsAxisMapActualIndexToWheel(int i);
int jsAxisMapWheelIndexToActual(int i);
int jsButtonMapActualIndexToWheel(int i);
int jsButtonMapWheelIndexToActual(int i);

// A single device
typedef struct {
    int fd;
    char numAxes;
    char numButtons;
    char device; // the number after "/dev/input/js"
    char name[117];
} jsDevice;

int jsDeviceInit(jsDevice *device, char pathNum);
void jsDeviceClean(jsDevice *device);

typedef struct {
    __u8 axis[ABS_CNT];
    __u16 button[(KEY_MAX - BTN_MISC + 1)];
} jsMapping;

int jsMappingGet(jsMapping *dst, jsDevice *device);
int jsMappingSet(jsMapping *src, jsDevice *device);

int jsEventGet(struct js_event *dst, jsDevice *device);

int jsMappingSave(jsMapping *src, jsDevice *device);
int jsMappingLoad(jsMapping *dst, jsDevice *device);

#ifdef __cplusplus
}
#endif

#endif
