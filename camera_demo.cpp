#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "camera.h"
static inline float clamp(float val, float mn, float mx) {
  return (val >= mn) ? ((val <= mx) ? val : mx) : mn;
}

static void YUVToRGB(const unsigned char y, const unsigned char u,
                     const unsigned char v, unsigned char *r, unsigned char *g,
                     unsigned char *b) {
  const int y1 = static_cast<int>(y) - 16;
  const int u1 = static_cast<int>(u) - 128;
  const int v1 = static_cast<int>(v) - 128;

  *r = clamp(1.164f * y1 + 2.018f * v1, 0.0f, 255.0f);
  *g = clamp(1.164f * y1 - 0.813f * u1 - 0.391f * v1, 0.0f, 255.0f);
  *b = clamp(1.164f * y1 + 1.596f * u1, 0.0f, 255.0f);
}

static void YUYVToRGB(unsigned char *yuv, int w, int h,int index) {
  int num_pixels = w * h;
  unsigned char *bgr =
      (unsigned char *)calloc(num_pixels * 3, sizeof(unsigned char));
  memset(bgr, 0, num_pixels * 3 * sizeof(char));

  int i, j;
  unsigned char y0, y1, u, v;
  unsigned char r, g, b;
  for (i = 0, j = 0; i < (num_pixels << 1); i += 4, j += 6) {
    y0 = yuv[i + 0];
    u = yuv[i + 1];
    y1 = yuv[i + 2];
    v = yuv[i + 3];

    YUVToRGB(y0, u, v, &r, &g, &b);
    bgr[j + 0] = b;
    bgr[j + 1] = g;
    bgr[j + 2] = r;

    YUVToRGB(y1, u, v, &r, &g, &b);
    bgr[j + 3] = b;
    bgr[j + 4] = g;
    bgr[j + 5] = r;
  }
  cv::Mat img(h, w, CV_8UC3, bgr);
  char windname[10]={};
  snprintf(windname, sizeof(windname), "img%d", index);
  cv::imshow(windname, img);
  delete[] bgr;
}
int main(int argc, char **argv) {
	// v4l2_device vct;
	// vct.dev_name= "/dev/video0";
	// vct.fd=-1;
	// vct.w=1280;
	// vct.h=720;
	// vct.buffers_cnt=4;
	// int length=0;
	// bool res=initcamera(&vct,&length);
	// v4l2_buffer vd;
	// memset(&vd,0,sizeof(vd));

	// vd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	// vd.memory = V4L2_MEMORY_MMAP;
	// while (1)
	// {
	// 	dequeueframe(&vct,&vd);
	// 	YUYVToRGB((unsigned char*)vct.buffers[vd.index].start,1280,720);
	// 	enqueueframe(&vct,vd.index);
	// 	cv::waitKey(1);
	// }
	int res=InitCameras();
	unsigned char * yv[CAMERA_NUMS];
	while (1)
	{
		GetPixels(yv);
		for(int i=0;i<CAMERA_NUMS;i++)
		YUYVToRGB(yv[i],1280,720,i);
		ReleasePixels();
		cv::waitKey(1);
	}
	

}