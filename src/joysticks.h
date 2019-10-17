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

#define JS_TOTAL_MAP_STRINGS 76
extern const char* jsMapStrings[JS_TOTAL_MAP_STRINGS];

/*  Takes in a mapped button or axis index as reported by
    the driver and provides the index into the above array.    */
int jsMapActualIndexToString(int i);
/*  Effectively does the opposite of the above function,
    returning JS_INVALID_ARGUMENT if the input is out of range.
    This error is reported for "NOT_SUPPORTED" as well, so you
    have to account for it.                                     */
int jsMapStringIndexToActual(int i);

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
