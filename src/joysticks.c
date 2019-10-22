/*
    File: joysticks.c
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
        hash = hash * 31 + name[i];
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
