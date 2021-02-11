#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>

#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/v4l2-common.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "cameraControls.h"

using namespace std;
uint8_t* buf;

int check()
{	//check whether camera is connected
	int camD;//camera descriptor
	camD = open("/dev/video0", O_RDWR);

	if (camD<0) {
		cout << "No camera is connected \n";
		close(camD);
		return -1;
	}
	else {
		//check if camera can take a picture
		cout << "Camera is connected \n";
		v4l2_capability capability;
		if (ioctl(camD, VIDIOC_QUERYCAP, &capability) < 0) {
			//camera is not capable of taking a picture or something went wrong
			cout << "Failed to retrieve device capability, error at VIDIOC_QUERYCAP";
		}
		if (setRF(camD) < 0) {
			cout << "unable to set resolution";
		}
		if (setBuffer(camD) < 0) {
			cout << "buffer not set";
		}
		if (makeFrame(camD) < 0) {
			cout << "idk man im tired";
		}
	}
	close(camD);
	return 0;

}

int setRF(int cameraDescriptor) {
	//set resolution and format of recorded picture
	
	/*TODO*/
	//make this customisable, f.e. width, height, format etc.


	v4l2_format imageRF;
	v4l2_pix_format res;
	imageRF.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	//cameras native resolution
	imageRF.fmt.pix.width = 1270;
	imageRF.fmt.pix.height = 720;
	imageRF.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;
	imageRF.fmt.pix.field = V4L2_FIELD_NONE;

	if (ioctl(cameraDescriptor, VIDIOC_S_FMT, &imageRF) < 0) {
		cout << " VIDIOC_S_FMT error, unable to set resolution";
		return -1;
	}
	return 0;
}

int setBuffer(int cameraDescriptor)
{
	//requesting buffer from device and allocating space
	v4l2_requestbuffers requestedBuffer = { 0 };
	requestedBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	requestedBuffer.memory = V4L2_MEMORY_MMAP;
	requestedBuffer.count = 1;

	if (ioctl(cameraDescriptor, VIDIOC_REQBUFS, &requestedBuffer) < 0) {
		perror("VIDIOC_REQBUFS error");
		return -1;
	}



	//query buffer and alloc space
	v4l2_buffer querryBuffer = { 0 };
	querryBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	querryBuffer.memory = V4L2_MEMORY_MMAP;
	querryBuffer.index = 0;

	if (ioctl(cameraDescriptor, VIDIOC_QUERYBUF, &querryBuffer) < 0) {
		perror("VIDIOC_QUERYBUF error");
		return -1;
	}
	buf = (uint8_t*) mmap(NULL, querryBuffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, cameraDescriptor, querryBuffer.m.offset);
	memset(buf, 0, querryBuffer.length);

	return 0;
}

int makeFrame(int cameraDescriptor)
{
	v4l2_buffer frameBuffer = { 0 };
	frameBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	frameBuffer.memory = V4L2_MEMORY_MMAP;
	frameBuffer.index = 0;
	if (ioctl(cameraDescriptor, VIDIOC_QBUF, &frameBuffer) < 0) {
		perror("ONE");
		return -1;
	}
	if (ioctl(cameraDescriptor, VIDIOC_STREAMON, &frameBuffer.type) < 0) {
		perror("TWO");
		return -1;
	}
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(cameraDescriptor, &fd);
	timeval ctime = { 0 };
	ctime.tv_sec = 2;

	int cap = select(cameraDescriptor + 1, &fd, NULL, NULL, &ctime);
	if (ioctl(cameraDescriptor, VIDIOC_DQBUF, &frameBuffer) < 0) {
		perror("Invalid data");
		return -1;
	}
	int output = open("camerasnap.jpg", O_RDWR | O_CREAT);
	write(output,buf,frameBuffer.bytesused);
	close(output);
	return 0;
}
