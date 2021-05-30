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
#include <fcntl.h>l
#include <unistd.h>


#include <chrono>

using namespace std;

int main(int argc, char* argv[])
{
    /*
    *     Image test("/home/pi/projects/bcappv1/example.jpg");
    Histogram h;
    test.grayscale();
    test.write("GS.jpg");
    h = test.histogram();

    test.boxFilterTxT();
    test.write("boxfiltertest.jpg");

    test.histogram_equalisation(h);
    test.write("equalisation.jpg");
    */
    /*
    */
    //cout << "Welcome to BCappV1 \n";
    //cout << "Starting capture of calibration images \n";

    //auto start = chrono::high_resolution_clock::now();

    ////start of capture/calibration sequence
    //int camera_fd = open("/dev/video0", O_RDWR);
    //if (camera_check(camera_fd) != 1) {
    //    capability(camera_fd);
    //    set_r_f(camera_fd);
    //    set_buffer(camera_fd);
    //    //loop set to capture 5 images
    //    for (int i = 1; i <= 30; i++) {
    //        if (make_frame(camera_fd, i, CALIBRATION) == 1) {
    //        };
    //    }
    //    stop_stream(camera_fd);
    //    cout << "Calibration images captured\n";
    //    cout << "Computation of data from calibration images starting\n";
    //    /*
    //    information from images are cumulative histogram, average of that histogram to determine background luminescence 
    //    */
    //    Histogram calibration_h;
    //    for (int i = 6; i <= 30; i += 6) {
    //        string path = "/home/pi/projects/bcappv1/bin/ARM/Debug/";
    //        string name = "camerasnap";
    //        string numname = path + name + to_string(i) + ".jpg";
    //        const char* fullname = numname.c_str();

    //        Image calibration(fullname);
    //        calibration.grayscale();

    //        calibration_h = calibration.cumulative_histogram(0, calibration.histogram());
    //    }

    //    auto finish = chrono::high_resolution_clock::now();
    //    chrono::duration<double> elapsed = finish - start;

    //    cout << "Calibration is done. Total time elapsed: " << elapsed.count() << " seconds \n";
    //    cout << "Starting main process\n";

    //}
    //else {
    //    cout << "\nConnect camera and restart the application\n";
    //}
    Histogram histogram;
    Image test("/home/pi/projects/bcappv1/bin/ARM/Debug/camerasnap6.jpg");
    test.grayscale();
    test.write("gray.jpg");


    histogram = test.histogram();
    test.histogram_equalisation(histogram);
    test.write("equalisation.jpg");
    test.boxFilterTxT();
    test.write("filter.jpg");
    test.invert();
    test.write("inverted.jpg");
    test.brightness_by_treshold(mid_range_tresh_value(histogram));
    test.write("tresh.jpg");
    cout << "BCappV1 has finished! \n";
    return 0;
}