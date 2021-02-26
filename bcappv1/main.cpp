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

using namespace std;

int main(int argc, char* argv[])
{

    int camera = open("/dev/video0", O_RDWR);
    if (camera_check(camera) != -1) {
        Image buftest(camera_record_init(camera)); ///ulozi sa priamo z fotenia
        Image test("/home/pi/projects/bcappv1/bin/x86/Debug/camerasnap.png");
        test.write("openedtest.png");//ulozi sa fotka otvorena z disku
       // buftest.filterChannel(1, 0, 0);
        int done=buftest.write("testfrombuffer2.png"); //ulozi sa fotka buffera
        cout << done;
    };
    



   
    

    ////cout<<pixel(test, 0, 1);

    //
    //test.filterChannel(1, 0, 0);
    //
    //test.adjustContrast(1.1);
    //test.adjustBrightness(75);
    //
    ////test.invert();
    ////test.boost_color('r');
    //test.write("image-pc-test.jpg");


    //Image gray=test;
    //Image filter = test;
    //Image contrast = test;
    //Image brightness = test;
    //Image inver = test;
    //Image boxF = test;


    cout << "BCappV1 has finished! \n";
    return 0;
}