#include <cstdio>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "Image.h"

using namespace std;

int main(int argc, char* argv[])
{
    bool menu = true;

 
    
    
    Image test("/home/pi/projects/bcappv1/JPGtest.jpg");
    Image gray=test;
    Image filter = test;
    Image contrast = test;
    Image brightness = test;

    brightness.adjustBrightness(-20);
    brightness.write("brtest.jpg");

    contrast.adjustContrast(1.5);
    contrast.write("contrastTest.jpg");


    gray.grayscale();
    gray.write("PDGRAY.jpg");

    filter.filterChannel(0, 1, 0);
    filter.write("FILTER.jpg");
    filter.histogram();


    printf("hello from %s!\n", "bcappv1");
    return 0;
}