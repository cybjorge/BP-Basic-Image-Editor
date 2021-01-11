#include <cstdio>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include "Image.h"

int main(int argc, char* argv[])
{

    Image test("/home/pi/projects/bcappv1/JPGtest.jpg");
    Image gray=test;
    Image filter = test;
    gray.grayscale();
    gray.write("PDGRAY.jpg");

    filter.filterChannel(0, 1, 0);
    filter.write("FILTER.jpg");
    filter.histogram();


    printf("hello from %s!\n", "bcappv1");
    return 0;
}