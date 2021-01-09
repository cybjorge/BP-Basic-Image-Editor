#include <cstdio>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include "Image.h"

int main(int argc, char* argv[])
{

    Image test("/home/pi/projects/bcappv1/image4.bmp");
    Image shit = test;
    for (int i = 0; i < shit.width * shit.channels*shit.height;i++) {
        shit.data[i] = 255;
    }
    Histogram hg;

    hg=test.histogram(1/*nechat prazdne ak cely histogram, zadat cislo pre RGB*/);
    shit.write("test34.png");
    printf("hello from %s!\n", "bcappv1");
    return 0;
}