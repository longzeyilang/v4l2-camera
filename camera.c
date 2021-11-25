#include "camera.h"

#include <sys/ioctl.h>
#include <time.h>

int status = 1;           //(0:normal, 1:dequeue, 2:enqueue)
pthread_mutex_t g_mutex;  // mutex locks
pthread_cond_t g_cond;      // condition locks
pthread_t g_thread;       
int g_camera = 0;                     //camera status: -1(error) 0(normal)
int length = 0;                           //image length
int image_index=-1;             // image index
//  thread
void *get(void *args) {
  while (1) {
    pthread_mutex_lock(&g_mutex);
    if (status == 1) {
      for (size_t i = 0; i < CAMERA_NUMS; i++) {
        struct v4l2_buffer vb;
	      memset(&vb,0,sizeof(vb));
	      vb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	      vb.memory = V4L2_MEMORY_MMAP;
        if (!dequeueframe(&vct[i], &vb)) {
          g_camera = -1;
          break;
        }

        //若存在取相机数据index不相同时，相机出现故障问题
        if (i == 0) image_index = vb.index;
        if (image_index != vb.index) {
          printf("camera error\n");
          g_camera = -1;
          break;
        }
        memcpy(yuyv[i], vct[i].buffers[image_index].start, length);
      }
      status = 0;
    } else if (status == 2) {
      for (size_t i = 0; i < CAMERA_NUMS; i++) {
        if (!enqueueframe(&vct[i], &wholeimages[i])) {
          g_camera = -1;
          break;
        }
      }
      status = 0;
    } else if (status == 3)  //退出线程
    {
      printf("thread close\n");
      pthread_exit(0);
    }

    while (status == 0) {
      pthread_cond_wait(&g_cond, &g_mutex);
    }

    pthread_mutex_unlock(&g_mutex);
    usleep(500);
  }
}

int InitCameras() {
  // Initialize the V4L2 device
  for (size_t i = 0; i < CAMERA_NUMS; i++) {
    vct[i].dev_name = dev_names[i];
    vct[i].fd = -1;
    vct[i].w = PIX_W;
    vct[i].h = PIX_H;
    vct[i].buffers_cnt = BUF_CNT;
    bool res = initcamera(&vct[i], &length);
    if (!res) {
      printf("[initcamera] Could not initialize the %s V4L2 device.\n",dev_names[i]);
      return -1;
    }
    // create malloc
    yuyv[i] = (unsigned char *)calloc(1, sizeof(unsigned char) * length);
  }

  // initiate mutex
  pthread_mutex_init(&g_mutex, NULL);
  pthread_cond_init(&g_cond, NULL);
  pthread_create(&g_thread, NULL, get, NULL);
  pthread_detach(g_thread);
  return 0;
}

//获取数据
int GetPixels(unsigned char **image) {
  struct timeval ts, tf;
  gettimeofday(&ts, NULL);
  double start = ts.tv_sec * 1000.0 + ts.tv_usec / 1000.0;

  //等待恢复状态
  while (status != 0) {
    usleep(500);
  };

  pthread_mutex_lock(&g_mutex);
  if (g_camera >= 0)  // camera no error
  {
    for (size_t i = 0; i < CAMERA_NUMS; i++) 
      image[i] = yuyv[i];
    status = 2;
    pthread_cond_signal(&g_cond);
  }
  pthread_mutex_unlock(&g_mutex);

  gettimeofday(&tf, NULL);
  double end = tf.tv_sec * 1000.0 + tf.tv_usec / 1000.0;
  printf("GetPixels: %f mseconds\n", (end - start));
  return g_camera;
}

int ReleasePixels() {
  struct timeval ts, tf;
  gettimeofday(&ts, NULL);
  double start = ts.tv_sec * 1000.0 + ts.tv_usec / 1000.0;

  //等待恢复状态
  while (status != 0) {
    usleep(500);
  };

  pthread_mutex_lock(&g_mutex);
  if (g_camera >= 0)  // camera no error
  {
    status = 1;
    pthread_cond_signal(&g_cond);
  }
  pthread_mutex_unlock(&g_mutex);

  gettimeofday(&tf, NULL);
  double end = tf.tv_sec * 1000.0 + tf.tv_usec / 1000.0;
  printf("ReleasePixels: %f mseconds\n", (end - start));
  // usleep(5000);
  return g_camera;
}

// destory camera
int DestoryCameras() {
  pthread_mutex_lock(&g_mutex);
  status = 3;
  pthread_cond_signal(&g_cond);
  pthread_mutex_unlock(&g_mutex);

  pthread_mutex_destroy(&g_mutex);
  pthread_cond_destroy(&g_cond);
  for (size_t i = 0; i < CAMERA_NUMS; i++) {
    stop(&vct[i]);
    if (yuyv[i]) free(yuyv[i]);
  }
  return 0;
}
