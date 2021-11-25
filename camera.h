#ifndef VIDEO_DEVICE_H
#define VIDEO_DEVICE_H

#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "v4l2.h"
#define CAMERA_NUMS 2
#define PIX_W 1280
#define PIX_H 720
#define BUF_CNT 4
// static char *dev_names[CAMERA_NUMS] = {"/dev/video0", "/dev/video1", "/dev/video2", "/dev/video3", "/dev/video4"};
 static char *dev_names[CAMERA_NUMS] = {"/dev/video0", "/dev/video1"};
static struct v4l2_buffer wholeimages[CAMERA_NUMS];
static struct v4l2_device vct[CAMERA_NUMS];
static unsigned char *yuyv[CAMERA_NUMS] = {0};

#ifdef __cplusplus
extern "C" {
#endif
int InitCameras();
int GetPixels(unsigned char **image);
int DestoryCameras();
int ReleasePixels();
#ifdef __cplusplus
}
#endif
#endif
