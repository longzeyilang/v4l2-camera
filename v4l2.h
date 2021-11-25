#ifndef _CV_LIB_V4L2_H
#define _CV_LIB_V4L2_H

#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>  /* low-level i/o */
#include <getopt.h> /* getopt_long() */
#include <linux/videodev2.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
struct buffer {
  void *start;
  size_t length;
};

struct v4l2_device {
  char *dev_name;  ///< The name of the device
  int fd;          ///< The file pointer to the device
  int w;           ///< The width of the image
  int h;           ///< The height of the image
  int buffers_cnt;
  struct buffer *buffers;
};

bool initcamera(struct v4l2_device *vd, int *length);
bool dequeueframe(struct v4l2_device *vd, struct v4l2_buffer *buf);
bool enqueueframe(struct v4l2_device *vd,int index);
bool stop(struct v4l2_device *vd);
#endif
