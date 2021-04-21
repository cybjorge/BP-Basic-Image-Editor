#include <cstdio>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "Image.h"
#include "cameraControls.h"


#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/v4l2-common.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


#include <chrono>

using namespace std;

int main(int argc, char* argv[])
{
    /*
    Image test("/home/pi/projects/bcappv1/mj.jpg");
    Histogram h;
    test.grayscale();
    test.write("GS.jpg");
    h = test.histogram();
    test.histogram_equalisation(h);
    test.write("equalisation.jpg");
    */



    
    cout << "BCappV1 has finished! \n";
    return 0;
}