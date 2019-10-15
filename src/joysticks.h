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
};

const char* jsErrorString(int error);

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

#ifdef __cplusplus
}
#endif

#endif
