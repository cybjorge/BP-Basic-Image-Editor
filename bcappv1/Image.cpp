#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG


#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <algorithm>

//declarations for functions needed to perform some operations
int find_min(int arr[],int sz);
int find_max(int arr[],int sz);
int clamp(int channelValue);
uint8_t to2darray(uint8_t unsortedData);

Image::Image(const char* filename)
{
	if (read(filename)) {
		printf("Success reading %s \n", filename);
		stats(filename);
		size = width * height * channels;
	}
	else {
		perror("read");
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
	data=stbi_load(filename,&width,&height,&channels,0);
	return data != NULL;
}

bool Image::write(const char* filename)
{
	ImageType type = imageFileType(filename);
	int done;
	switch (type)
	{
	case PNG:
		done = stbi_write_png(filename, width, height, channels, data, width * channels);
		break;
	case BMP:
		done = stbi_write_bmp(filename, width, height, channels, data);
		break;
	case JPG:
		done = stbi_write_jpg(filename, width, height, channels, data,100);
		break;
	case NONE:
		done = 0;
		break;
	default:
		break;
	}
	return done!=0;
}
ImageType Image::imageFileType(const char* filename) {
	const char* ext = strrchr(filename, '.');
	if (ext != nullptr) {
		if (strcmp(ext, ".png")==0) {
			return PNG;
		}
		else if (strcmp(ext, ".bmp") == 0) {
			return BMP;
		}
		else if (strcmp(ext, ".jpg") == 0) {
			return JPG;
		}
		else
		{
			printf("Uknown Image Format: %s \n", ext);
		}
	}
	return NONE;
}

Histogram Image::histogram(int desired_channel) {

	Histogram H_data;
	int step;

	if (!desired_channel) {
		desired_channel = 1;
		step = 1;
	}
	else {
		step = channels;
	}

	for (int i = desired_channel-1; i < size; i+=step) {
		H_data.histogram_data[data[i]]++;
	}

	return H_data;
}

void Image::stats(const char* filename)
{
	printf("Image width x image heigth: %d  x %d \n", width, height);
	printf("Number of color channels: %d \n", channels);
	printf("Image size inn Bites: %d \n", size);
	//printf("Image format: %s", imageFileType(filename));
}

Histogram Image::histogram()
{
	return histogram(NULL);
}

Histogram::Histogram()
{
	for (int i = 0; i < HIST_ARRAY_SIZE; i++) {
		histogram_data[i] = 0;
	}
}

Histogram::~Histogram()
{
	free(histogram_data);
}


//point operations
Image& Image::grayscale()
{
	if (channels < 3) {
		printf("Sorry, but it seems that image has less than 3 (RGB) channels");
	}
	else {
		for (int i = 0; i < size; i += channels) {
			int grayscale = (data[i] + data[i + 1] + data[i + 2]) / 3;
			memset(data + i, grayscale, 3);//if there are 4 channels, make channels-1
		}
	}
	return *this;

}

Image& Image::adjustContrast(float value)
{
	float contrast = ((255 * (value * 255)) / (255 * (255 - value)));
	if (channels < 3) {
		printf("Sorry, but it seems that image has less than 3 (RGB) channels");
	}
	else {
		for (int i = 0; i < size; i += channels) {
			//will be useful for auto contrast
			/*
			* 			
			int pixel[3] = { data[i],data[i + 1],data[i + 2] };
			int diff = (find_max(pixel, 3) - find_min(pixel, 3));
			int r = (255*(data[i]- find_max(pixel, 3)))/diff;
			int g = (255 * (data[i+1] - find_max(pixel, 3))) / diff;
			int b = (255 * (data[i+2] - find_max(pixel, 3))) / diff;
			*/

			/*
			* if (b > 255) {
				b = 255;
			}
			*/
			

			data[i] = clamp(contrast*(data[i] -128)+128);
			data[i+1] = clamp(contrast * (data[i+1] - 128) + 128);
			data[i+2] = clamp(contrast * (data[i+2] - 128) + 128);
			
			/*
			 int min = find_min(pixel,3);
			int max = find_max(pixel,3);
			*/
			

			//memset(data + i, b, 3);

		}
	}
	return *this;
}

Image& Image::invert()
{
	int maxVal=255;
	for (int i = 0; i < size; i += channels) {
		data[i] = maxVal-data[i];
		data[i + 1] = maxVal - data[i+1];
		data[i + 2] = maxVal - data[i + 2];
	}
	return *this;
}



Image& Image::boxFilterTxT()
{
	int m = width;
	int n = height;
	uint8_t* filterData = data;

	
	//next row offset is width
	//start at +1 pixel == start with value 3 (representing pixel) and at 2nd row
	for (int u = channels+width; u < size-(channels*width); u += channels) {
		int downOffset = (u - width);
		if (downOffset < 0) {
			downOffset *= -1;
		}
		int upOffset = (u + width);
		int avgRed = (data[downOffset - 3] + data[downOffset] + data[downOffset + 3] + data[u - 3] + data[u] + data[u + 3] + data[upOffset - 3] + data[upOffset] + data[upOffset + 3]) / 9;
		int avgGreen = (data[downOffset - 2] + data[downOffset + 1] + data[downOffset + 4] + data[u - 2] + data[u + 1] + data[u + 4] + data[upOffset - 2] + data[upOffset + 1] + data[upOffset + 4]) / 9;
		int avgBlue = (data[downOffset - 1] + data[downOffset + 2] + data[downOffset + 5] + data[u - 1] + data[u + 2] + data[u + 5] + data[upOffset - 1] + data[upOffset + 2] + data[upOffset + 5]) / 9;
		filterData[u] = avgRed;
		filterData[u + 1] = avgGreen;
		filterData[u + 2] = avgBlue;
	}
	/*for (int i = 0; i < width * height * channels; i++) {
		memset(data + i, (int)filterData + i, 1);
	}*/
	data = filterData;
	return *this;
}

Image& Image::adjustBrightness(float value)
{
	if (channels < 3) {
		printf("Sorry, but it seems that image has less than 3 (RGB) channels");
	}
	else {
		for (int i = 0; i < size; i += channels) {
			data[i] = clamp(value + data[i]);
			data[i + 1] = clamp(value + data[i+1]);
			data[i + 2] = clamp(value + data[i+2]);
		}
	}
	return *this;
}

Image& Image::filterChannel(float r, float g, float b)
{
	if (channels < 3) {
		printf("Sorry, but it seems that this image doesnt have enough color channels to preform this action");
	}
	else {
		for (int i = 0; i < size; i += channels) {
			data[i] = data[i] * r;
			data[i + 1] = data[i + 1] * g;
			data[i + 2] = data[i + 2] * b;
		}
	}
	return *this;
}

/*pridat autocontrast moznost*/

//useful functions

int find_min(int arr[], int sz) {
	int min = 256;
	for (int i = 0; i < sz; i++) {
		if (arr[i] < min) {
			min = arr[i];
		}
	}
	return min;
}
int find_max(int arr[], int sz) {
	int max = 0;
	for (int i = 0; i < sz; i++) {
		if (arr[i] > max) {
			max = arr[i];
		}
	}
	return max;
}

int clamp(int channelValue) {
	if (channelValue < 0) {
		return 0;
	}
	else if (channelValue > 255) {
		return 255;
	}
	return channelValue;
}

