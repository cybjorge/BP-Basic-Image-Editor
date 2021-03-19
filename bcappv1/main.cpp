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
    //calibration
    int camera = open("/dev/video0", O_RDWR);
    cout << "Starting calibration \n";
    auto start = chrono::high_resolution_clock::now();
    if (camera_check(camera) != -1) {       //if camera is conected, proceed
        Histogram cumulative_histogram;
        uint8_t* image;
        size_t image_size;

        //check camera capability, request buffer
        capability(camera);
        set_r_f(camera);
        set_buffer(camera);
        //recording loop, do 30 frames, save every 6th
        for (int i = 1; i <= 30; i++) {                 //camerasnap
            if (make_frame(camera, i,CALIBRATION) == 1) {
            };
            
        }
        cout << "Frames loaded\n";
        stop_stream(camera);
        cout << "Computing cumulative histogram \n";
        for (int i = 6; i <= 30; i += 6) {
            string path = "/home/pi/projects/bcappv1/bin/x86/Debug/";
            string name = "camerasnap";
            string numname = path+name + to_string(i) + ".jpg";
            const char* fullname = numname.c_str();

            Image calibration(fullname);
            calibration.grayscale();
            cumulative_histogram = calibration.cumulative_histogram(0, cumulative_histogram);
        }
        
        auto finish = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = finish - start;

        cout << "Calibration is done. Total time elapsed: "<<elapsed.count()<<" seconds \n";
        //histogram operaitons
        cumulative_histogram.average_value();
        /*mean treshold*/

        /*equalization parameters*/


        cout << "Starting main process";
        auto s = chrono::high_resolution_clock::now();
        for (int i = 1; i <= 30; i++) {                 //camerasnap
            if (make_frame(camera, i,MAIN) == 1) {


                string path = "/home/pi/projects/bcappv1/bin/x86/Debug/";
                string name = "capture";
                string numname = path + name + to_string(i) + ".jpg";
                const char* fullname = numname.c_str();
                
                Image main_image(fullname);
                main_image.grayscale();
                main_image.write("g.jpg");
                main_image.adjustContrast(1.5);
                main_image.write("con.jpg");
                main_image.brightness_by_treshold(mid_range_tresh_value(cumulative_histogram));
                main_image.invert();
                main_image.write("invert.jpg");
                main_image.filterChannel(1, 0, 0);
                main_image.write("final.jpg");
                /*auto brigtness by mean treshold*/

                /*contrast fix*/

                /*filtering out channels*/

                /*median filter*/
                
            };

        }
        auto f = chrono::high_resolution_clock::now();


    }



    cout << "BCappV1 has finished! \n";
    return 0;
}