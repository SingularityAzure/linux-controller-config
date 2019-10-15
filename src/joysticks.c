/*
    File: joystics.c
    Author: Philip Haynes
*/

#include "joysticks.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

const char* jsErrorStrings[] = {
    "JS_OK",
    "JS_DEVICE_NOT_FOUND",
    "JS_INVALID_ARGUMENT",
    "JS_UNKNOWN_ERROR",
};

const char* jsErrorString(int error) {
    if (error < -3 || error > 0) {
        return "Invalid error code";
    }
    return jsErrorStrings[-error];
}

#define JSIOCSNAME(len)		_IOC(_IOC_WRITE, 'j', 0x13, len)

int jsDeviceInit(jsDevice *device, char pathNum) {
    if (!device || pathNum >= 100 || pathNum < 0) return JS_INVALID_ARGUMENT;
    char path[] = "/dev/input/jsXX";
    if (pathNum < 10) {
        path[13] = pathNum+'0';
        path[14] = 0;
    } else {
        path[13] = pathNum/10+'0';
        path[14] = pathNum%10+'0';
    }
    device->fd = open(path, O_RDONLY | O_NONBLOCK);
    if (device->fd < 0) {
        return JS_DEVICE_NOT_FOUND;
    }
    if (-1 == ioctl(device->fd, JSIOCGNAME(117), device->name)) {
        close(device->fd);
        device->fd = -1;
        return JS_UNKNOWN_ERROR;
    }
    if (-1 == ioctl(device->fd, JSIOCGAXES, &device->numAxes)) {
        close(device->fd);
        device->fd = -1;
        return JS_UNKNOWN_ERROR;
    }
    if (-1 == ioctl(device->fd, JSIOCGBUTTONS, &device->numButtons)) {
        close(device->fd);
        device->fd = -1;
        return JS_UNKNOWN_ERROR;
    }
    printf("Opened %s, named %s with %i axes and %i buttons\n",
        path, device->name, (int)device->numAxes, (int)device->numButtons);
    return JS_OK;
}

void jsDeviceClean(jsDevice *device) {
    if (device->fd >= 0) close(device->fd);
    device->fd = -1;
}

int jsMappingGet(jsMapping *dst, jsDevice *device) {
    if (!device) return JS_INVALID_ARGUMENT;
    if (device->fd < 0) return JS_INVALID_ARGUMENT;
    if (-1 == ioctl(device->fd, JSIOCGAXMAP, dst->axis)) {
        return JS_UNKNOWN_ERROR;
    }
    if (-1 == ioctl(device->fd, JSIOCGBTNMAP, dst->button)) {
        return JS_UNKNOWN_ERROR;
    }
    for (int i = 0; i < device->numAxes; i++) {
        printf("Axis %i mapped to %i\n", i, (int)dst->axis[i]);
    }
    for (int i = 0; i < device->numButtons; i++) {
        printf("Button %i mapped to %i\n", i, (int)dst->button[i]);
    }
    return JS_OK;
}

int jsMappingSet(jsMapping *src, jsDevice *device) {
    if (!device) return JS_INVALID_ARGUMENT;
    if (device->fd < 0) return JS_INVALID_ARGUMENT;
    if (-1 == ioctl(device->fd, JSIOCSAXMAP, src->axis)) {
        return JS_UNKNOWN_ERROR;
    }
    if (-1 == ioctl(device->fd, JSIOCSBTNMAP, src->button)) {
        return JS_UNKNOWN_ERROR;
    }
    return JS_OK;
}
