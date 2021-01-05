#define STB_IMAGE_IMPLEMENTATION

#include "Image.h"
#include "stb_image.h"

Image::Image(const char* filename)
{
	if (read(filename)) {
		printf("Success reading %s \n", filename);
		size = width * height * channels;
	}
	else {
		printf("Error reading %s \n", filename);
	}
}

Image::Image(int width, int height, int channels) : width(width),height(height),channels(channels)
{
	size = width * height * channels;
	data = new uint8_t[size];
}

Image::Image(const Image& img) : Image(img.width,img.height,img.channels)
{
	memcpy(data, img.data, size);
}

Image::~Image()
{
	stbi_image_free(data);
}

bool Image::read(const char* filename)
{
	stbi_load(filename,&width,&height,&channels,0);
	return data != NULL;
}

bool Image::write(const char* filename)
{
	return false;
}
