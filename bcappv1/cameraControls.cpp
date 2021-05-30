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
#include <errno.h>

#include <tuple>

#include "cameraControls.h"

using namespace std;
uint8_t* buf;
int buflen;


struct image_buf {
	uint8_t* bufer;
};

int camera_check(int cd)
{	
	//check whether camera is connected
	//if camera is connected, proceeds to initliaze buffers, sets resolution and captures the requested frame


	if (cd<0) {
		cout << "No camera is connected \n";
		close(cd);
		return -1;
	}
	else {
		cout << "Camera is connected \n";
	}
	return cd;
}
//uint8_t* camera_record_init(int cd,int flag) {
//	if (capability(cd) < 0) {
//		cout << "unable to retrieve device capability";
//		return nullptr;
//	}
//	if (set_r_f(cd) < 0) {
//		cout << "unable to set resolution";
//		return nullptr;
//	}
//	if (set_buffer(cd) < 0) { ///buflen
//		cout << "buffer not set";
//		return nullptr;
//	}
//	for (int i = 0; i < 5; i++) {
//		if (make_frame(cd, i) < 0) {
//			cout << "unable to capture frame";
//			return nullptr;
//		}
//	}
//	if (stop_stream(cd) < 0) {
//		cout << "unable to stop stream";
//		return nullptr;
//	}
//	close(cd);
//	return buf;
//}

int capability(int cd) {
	v4l2_capability capability;
	if (ioctl(cd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		return -1;
	}
	return 0;
}

int set_r_f(int cd) {
	
	v4l2_format imageRF;
	imageRF.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	imageRF.fmt.pix.width = 1280;
	imageRF.fmt.pix.height = 720;
	imageRF.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;
	imageRF.fmt.pix.field = V4L2_FIELD_NONE;

	if (ioctl(cd, VIDIOC_S_FMT, &imageRF) < 0) {
		cout << " VIDIOC_S_FMT error, unable to set resolution";
		return -1;
	}
	return 0;
}

int set_buffer(int cd)
{
	//requesting buffer from device and allocating space
	v4l2_requestbuffers requestedBuffer = { 0 };
	requestedBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	requestedBuffer.memory = V4L2_MEMORY_MMAP;
	requestedBuffer.count = 1;

	if (ioctl(cd, VIDIOC_REQBUFS, &requestedBuffer) < 0) {
		perror("VIDIOC_REQBUFS error");
		return -1;
	}



	//query buffer and alloc space
	v4l2_buffer querryBuffer = { 0 };
	querryBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	querryBuffer.memory = V4L2_MEMORY_MMAP;
	querryBuffer.index = 0;

	if (ioctl(cd, VIDIOC_QUERYBUF, &querryBuffer) < 0) {
		perror("VIDIOC_QUERYBUF error");
		return -1;
	}
	buf = (uint8_t*) mmap(NULL, querryBuffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, cd, querryBuffer.m.offset);
	memset(buf, 0, querryBuffer.length);

	return buflen;
}

int make_frame(int cd,int loop,int flag)
{
	v4l2_buffer frameBuffer = { 0 };
	frameBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	frameBuffer.memory = V4L2_MEMORY_MMAP;
	frameBuffer.index = 0;
	if (ioctl(cd, VIDIOC_QBUF, &frameBuffer) < 0) {
		perror("VIDIOC_QBUF");
		return -1;
	}
	//start streaming
	if (ioctl(cd, VIDIOC_STREAMON, &frameBuffer.type) < 0) {
		perror("VIDIOC_STREAMON");
		return -1;
	}

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(cd, &fd);
	timeval ctime = { 0 };
	ctime.tv_sec = 2;

	int cap = select(cd + 1, &fd, NULL, NULL, &ctime);

	if (ioctl(cd, VIDIOC_DQBUF, &frameBuffer) < 0) {//dequeue buffer
		perror("VIDIOC_DQBUF");
		return -1;
	}

	else
		if (flag == CALIBRATION){}
		else {
			if (loop % 6 == 0) {

				mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
				string name = "";

				if (flag == CALIBRATION) { name = "camerasnap"; }
				if (flag == MAIN) { name = "capture"; }
				if (flag == TEST) { name = "test"; }

				string numname = name + to_string(loop) + ".jpg";
				const char* n = numname.c_str();
				int output = open(n, O_RDWR | O_CREAT | O_TRUNC, mode);
				write(output, buf, frameBuffer.bytesused);
				close(output);
				return 1;
			}
		}
	buflen = frameBuffer.bytesused;
	return 0;
}
int stop_stream(int cd) {
	v4l2_buffer frameBuffer = { 0 };
	frameBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(cd, VIDIOC_STREAMOFF, &frameBuffer) == -1) {
		perror("VIDIOC_STREAMOFF");
		return -1;
	}
	return 0;
}
uint8_t* return_buf() {
	return buf;
}
size_t return_buf_size() {
	return buflen;
}
