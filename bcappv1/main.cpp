#include <cstdio>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "Image.h"
#include "cameraControls.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (check()) {
        return -1;
    }
    Image test("/home/pi/projects/bcappv1/image.jpg");
    /*Image gray=test;
    Image filter = test;
    Image contrast = test;
    Image brightness = test;
    Image inver = test;
    Image boxF = test;*/

    test.adjustBrightness(50);
    test.adjustContrast(1.5);
    test.write("image-br.jpg");

   /* gray.grayscale();
    gray.write("eGrayscale.jpg");
    cout << "Grayscale is done! \n";

    filter.filterChannel(1, 0, 0);
    filter.write("eFilterChannel.jpg");
    cout << "All channels except red are filtered! \n";

    contrast.adjustContrast(1.75);
    contrast.write("eContrastTest.jpg");
    cout << "Contrast adjusted by 75% !\n";


    brightness.adjustBrightness(70);
    brightness.write("eBrightness.jpg");
    cout << "Brightness adjusted by 70 points! \n";

    inver.invert();
    inver.write("eInverted.jpg");
    cout << "Image was inverted! \n";

    boxF.boxFilterTxT();
    boxF.write("eBoxFilterNOTDONE.jpg");
    cout << "3x3 box filter experiment is donw! \n";

    gray.histogram();
    filter.histogram();
    contrast.histogram();
    brightness.histogram();
    inver.histogram();
    boxF.histogram();
    cout << "Histograms were computed \n";


    cout << "BCappV1 has finished! \n";*/
    return 0;
}