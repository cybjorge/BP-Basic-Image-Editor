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
    if (camera_fd < 0) {
        //prepare camera
        capability(camera_fd);
        set_r_f(camera_fd);
        set_buffer(camera_fd);

        int i = 0;
        cout << "calibration\n" << flush;
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

           // Image img(buffer, size); //create an instance of the image
            Image img("/home/pi/projects/bcappv1/a.jpg");
            Histogram h;
            
            img.write("cap.jpg");
            img.write("before.jpg");
            /* preprocessing operations */
            img.median_filter();
            img.grayscale();
            h = img.histogram();
            h.statistics();
           
            int brightness = (h.min_index + h.max_index) / 2;
            img.adjustBrightness(brightness);
            
            h = img.histogram();
            h.statistics();
           
            img.write("bc.jpg");
            h.imagesize = img.size;
            h = img.treshold(h);
            
           if (h.binary_treshold[0] > h.binary_treshold[1]) {
                Image inverted = img;
                img.invert();
                //inverted.write("inverted.jpg");
                h = img.histogram();
           }
            
            img.median_filter();
            img.histogram_equalisation(h);
             /*this creates names for output images*/
            string numname = "capture" + to_string(capture_number) + ".jpg";
            const char* n = numname.c_str();

            img.write(n);
        }

        stop_stream(camera_fd);
        close(camera_fd);
    }
    
    //Image img("/home/pi/projects/bcappv1/img.JPG");
    //Histogram h;

    //img.grayscale();
    //h = img.histogram();
    //img.write("grayscaled.jpg");
    //img.histogram_equalisation(h);
    //img.write("equalised.jpg");

    return 0;
}