#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>

#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/v4l2-common.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

int check();
int setRF(int cameraDescriptor);
int setBuffer(int cameraDescriptor);
int makeFrame(int cameraDescriptor);