#include <cstdio>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include "Image.h"

int main(int argc, char* argv[])
{

    Image test("/home/pi/projects/bcappv1/PD.png");
    test.grayscale();
    test.write("PDGRAY.png");

    printf("hello from %s!\n", "bcappv1");
    return 0;
}