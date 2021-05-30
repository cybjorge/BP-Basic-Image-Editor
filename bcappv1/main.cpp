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
    cout << "welcome\n";
    //open camera;
    int camera_fd = open("/dev/video0", O_RDWR);

    //prepare camera
    capability(camera_fd);
    set_r_f(camera_fd);
    set_buffer(camera_fd);
    
    int i = 0;
    cout << "calibration\n"<<flush;
    //auto focus calibration
    while (i < CALIBRATION_REPEAT) {
        make_frame(camera_fd, i, CALIBRATION);
        i++;
    }
    cout << "calibration finished\n";
    //calibration ended, caputre 5 images
    for (int capture_number = 0; capture_number < 5; capture_number++) {
        make_frame(camera_fd, i, CALIBRATION);
        //retrieve adress and size of image
        uint8_t* buffer = return_buf();
        size_t size = return_buf_size();

        Image img(buffer, size); //create an instance of the image
        Histogram h;
        img.write("before.jpg");
        /* preprocessing operations */
        img.median_filter();
        img.grayscale();
       // img.invert();
        h = img.histogram();
        img.adjustContrast(1.5);
       // img.histogram_equalisation(h);


        /*this creates names for output images*/
        string numname = "capture" + to_string(capture_number) + ".jpg";
        const char* n = numname.c_str();

        img.write(n);
    }

    stop_stream(camera_fd);

    return 0;
}