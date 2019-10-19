/*
    File: joystics.c
    Author: Philip Haynes
*/

#include "joysticks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

const char* jsErrorStrings[] = {
    "JS_OK",
    "JS_DEVICE_NOT_FOUND",
    "JS_INVALID_ARGUMENT",
    "JS_UNKNOWN_ERROR",
    "JS_DEVICE_LOST"
};

const char* jsErrorString(int error) {
    if (error < -4 || error > 0) {
        return "Invalid error code";
    }
    return jsErrorStrings[-error];
}

const char* jsMapStrings[] = {
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
    "0x120 - BTN_TRIGGER",          // 0x120    24
    "0x121 - BTN_THUMB",            // 0x121    25
    "0x122 - BTN_THUMB2",           // 0x122    26
    "0x123 - BTN_TOP",              // 0x123    27
    "0x124 - BTN_TOP2",             // 0x124    28
    "0x125 - BTN_PINKIE",           // 0x125    29
    "0x126 - BTN_BASE",             // 0x126    30
    "0x127 - BTN_BASE2",            // 0x127    31
    "0x128 - BTN_BASE3",            // 0x128    32
    "0x129 - BTN_BASE4",            // 0x129    33
    "0x12a - BTN_BASE5",            // 0x12a    34
    "0x12b - BTN_BASE6",            // 0x12b    35
    "0x12f - BTN_DEAD",             // 0x12f    36
    "0x130 - BTN_SOUTH",            // 0x130    37
    "0x131 - BTN_EAST",             // 0x131    38
    "0x132 - BTN_C",                // 0x132    39
    "0x133 - BTN_NORTH",            // 0x133    40
    "0x134 - BTN_WEST",             // 0x134    41
    "0x135 - BTN_Z",                // 0x135    42
    "0x136 - BTN_TL",               // 0x136    43
    "0x137 - BTN_TR",               // 0x137    44
    "0x138 - BTN_TL2",              // 0x138    45
    "0x139 - BTN_TR2",              // 0x139    46
    "0x13a - BTN_SELECT",           // 0x13a    47
    "0x13b - BTN_START",            // 0x13b    48
    "0x13c - BTN_MODE",             // 0x13c    49
    "0x13d - BTN_THUMBL",           // 0x13d    50
    "0x13e - BTN_THUMBR",           // 0x13e    51
    "0x13f - BTN_NULL",             // 0x13f    52
    "0x140 - BTN_TOOL_PEN",         // 0x140    53
    "0x141 - BTN_TOOL_RUBBER",      // 0x141    54
    "0x142 - BTN_TOOL_BRUSH",       // 0x142    55
    "0x143 - BTN_TOOL_PENCIL",      // 0x143    56
    "0x144 - BTN_TOOL_AIRBRUSH",    // 0x144    57
    "0x145 - BTN_TOOL_FINGER",      // 0x145    58
    "0x146 - BTN_TOOL_MOUSE",       // 0x146    59
    "0x147 - BTN_TOOL_LENS",        // 0x147    60
    "0x148 - BTN_TOOL_QUINTTAP",    // 0x148    61
    "0x149 - BTN_STYLUS3",          // 0x149    62
    "0x14a - BTN_TOUCH",            // 0x14a    63
    "0x14b - BTN_STYLUS",           // 0x14b    64
    "0x14c - BTN_STYLUS2",          // 0x14c    65
    "0x14d - BTN_TOOL_DOUBLETAP",   // 0x14d    66
    "0x14e - BTN_TOOL_TRIPLETAP",   // 0x14e    67
    "0x14f - BTN_TOOL_QUADTAP",     // 0x14f    68
    "0x150 - BTN_GEAR_DOWN",        // 0x150    69
    "0x151 - BTN_GEAR_UP",          // 0x151    70
    "0x220 - BTN_DPAD_UP",          // 0x220    71
    "0x221 - BTN_DPAD_DOWN",        // 0x221    72
    "0x222 - BTN_DPAD_LEFT",        // 0x222    73
    "0x223 - BTN_DPAD_RIGHT",       // 0x223    74
    "NOT_SUPPORTED",                //          75
};

int jsMapActualIndexToString(int i) {
    if (i < 0x0b) {
        return i;
    } else if (i < 0x10) {
        return JS_TOTAL_MAP_STRINGS - 1;
    } else if (i < 0x1d) {
        return i - 0x10 + 11;
    } else if (i < 0x120) {
        return JS_TOTAL_MAP_STRINGS - 1;
    } else if (i < 0x12c) {
        return i - 0x120 + 24;
    } else if (i < 0x12f) {
        return JS_TOTAL_MAP_STRINGS - 1;
    } else if (i < 0x152) {
        return i - 0x12f + 36;
    } else if (i < 0x220) {
        return JS_TOTAL_MAP_STRINGS - 1;
    } else if (i < 0x224) {
        return i - 0x220 + 71;
    } else {
        return JS_TOTAL_MAP_STRINGS - 1;
    }
}

int jsMapStringIndexToActual(int i) {
    if (i < 11) {
        return i;
    } else if (i < 24) {
        return i + 0x10 - 11;
    } else if (i < 36) {
        return i + 0x120 - 24;
    } else if (i < 71) {
        return i + 0x12f - 36;
    } else if (i < 75) {
        return i + 0x220 - 71;
    } else {
        return JS_INVALID_ARGUMENT;
    }
}

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

int jsEventGet(struct js_event *dst, jsDevice *device) {
    ssize_t rc = read(device->fd, dst, sizeof(struct js_event));
    if (rc == -1 && errno != EAGAIN) {
        close(device->fd);
        device->fd = -1;
        return JS_DEVICE_LOST;
    }
    return rc == sizeof(struct js_event);
}

void jsNameHash(char dst[8], const char *name) {
    unsigned hash = 0;
    for (int i = 0; name[i] != 0; i++) {
        hash ^= ((int)name[i]) << (8 * (i % 4));
    }
    for (int i = 0; i < 8; i++) {
        char c = hash % 16;
        dst[7-i] = c < 10 ? c+'0' : c+'a'-10;
        hash /= 16;
    }
}

// No padding bytes
#pragma pack(1)

typedef struct {
    unsigned short nameSize;
    char numAxes, numButtons;
    // char name[nameSize];
    // __u8 axis[numAxes];
    // __u16 button[numButtons];
} jsMappingFileHeader;

#pragma pack()

char *jsMappingFilename(const char *name) {
    char filename[] = "/.config/controller-config/mappingXXXXXXXX";
    jsNameHash(filename+34, name);
    char *homeDir = getenv("HOME");
    if (!homeDir) {
        return NULL;
    }
    int homeDirLen = strlen(homeDir);
    int filenameLen = strlen(filename);
    char *fullPath = (char*)malloc(homeDirLen+filenameLen+1);
    strcpy(fullPath, homeDir);
    strcpy(fullPath+homeDirLen, filename);
    return fullPath;
}

int jsMappingSave(jsMapping *src, jsDevice *device) {
    if (-1 == system("mkdir -p ~/.config/controller-config")) {
        return JS_UNKNOWN_ERROR;
    }
    char *fullPath = jsMappingFilename(device->name);
    if (!fullPath) {
        return JS_UNKNOWN_ERROR;
    }
    FILE *file = fopen(fullPath, "wb");
    free(fullPath);
    if (!file) {
        return JS_UNKNOWN_ERROR;
    }
    jsMappingFileHeader header;
    header.nameSize = strlen(device->name);
    header.numAxes = device->numAxes;
    header.numButtons = device->numButtons;
    fwrite(&header, 1, sizeof(header), file);
    fwrite(device->name, 1, header.nameSize, file);
    fwrite(src->axis, 1, header.numAxes, file);
    fwrite(src->button, 1, header.numButtons*2, file);
    fclose(file);
    return JS_OK;
}

int jsMappingLoad(jsMapping *dst, jsDevice *device) {
    char *fullPath = jsMappingFilename(device->name);
    if (!fullPath) {
        return JS_UNKNOWN_ERROR;
    }
    FILE *file = fopen(fullPath, "rb");
    free(fullPath);
    if (!file) {
        return JS_UNKNOWN_ERROR;
    }
    jsMappingFileHeader header;
    if (fread(&header, 1, sizeof(header), file) != sizeof(header)) {
        fclose(file);
        return JS_UNKNOWN_ERROR;
    }
    if (header.numAxes != device->numAxes) {
        printf("Wrong number of axes in the file (%i)!\n", (int)header.numAxes);
        fclose(file);
        return JS_UNKNOWN_ERROR;
    }
    if (header.numButtons != device->numButtons) {
        printf("Wrong number of buttons in the file!\n");
        fclose(file);
        return JS_UNKNOWN_ERROR;
    }
    char *name = (char*)malloc(header.nameSize+1);
    name[header.nameSize] = 0;
    if (fread(name, 1, header.nameSize, file) != header.nameSize) {
        free(name);
        fclose(file);
        return JS_UNKNOWN_ERROR;
    }
    if (strcmp(name, device->name) != 0) {
        printf("Wrong device name in the file!\n");
        free(name);
        fclose(file);
        return JS_UNKNOWN_ERROR;
    }
    free(name);
    if (fread(dst->axis, 1, header.numAxes, file) != (size_t)header.numAxes) {
        fclose(file);
        return JS_UNKNOWN_ERROR;
    }
    if (fread(dst->button, 1, header.numButtons*2, file) != (size_t)header.numButtons*2) {
        fclose(file);
        return JS_UNKNOWN_ERROR;
    }
    fclose(file);
    return JS_OK;
}
