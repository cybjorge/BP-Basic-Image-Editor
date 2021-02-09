#include <cstdio>
#include <iostream>
#include "cameraControls.h"
#include <usb.h>

using namespace std;

int check()
{	//check whether camera is connected
	FILE* fv0;
	FILE* fv1;

	fv0 = (fopen("/dev/video0", "r"));
	fv1 = (fopen("/dev/video1", "r"));
	if (!fv0 && !fv1) {
		cout << "No camera is connected \n";
		return -1;
	}
	cout << "Camera is connected \n";
	return 0;

	/*if (system("v4l2-ctl --list-devices")!=0) {
		cout<< "No camera is connected \n";
		return -1;
	}

	cout << "Camera is connected \n";
	return 0;*/
}

void getFrame(int qps) {
	//
}