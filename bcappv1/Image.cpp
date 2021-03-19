#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define MAX_BUFF_LEN 1270*720*3

#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "cameraControls.h"
#include <algorithm>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//declarations for functions needed to perform some operations
int find_min(int arr[],int sz);
int find_min(uint8_t arr[], int sz,int channels);
int find_max(int arr[],int sz);

int find_max_at(int arr[], int sz);
int find_min_at(int arr[], int sz);

int clamp(int channelValue);
int sort();
uint8_t to2darray(uint8_t unsortedData);

Image::Image(const char* filename)
{
	if (read(filename)) {
		printf("Success reading %s \n", filename);
		size = width * height * channels;
		type = imageFileType(filename);
		
	}
	else {
		perror("read");
		printf("Error reading %s \n", filename);
	}
}

Image::Image(uint8_t* buffer,size_t buflen) {
	data = buffer;
	width = 1280;
	height = 720;
	channels = 3; 
	size =	buflen;
	type = JPG;
	from_buffer = true;
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
	if (from_buffer) {
	}
	else {
		stbi_image_free(data);
	}
}


bool Image::read(const char* filename)
{
	data=stbi_load(filename,&width,&height,&channels,0);
	return data != NULL;
}

bool Image::read(uint8_t* buffer)
{
	data = stbi_load_from_memory(buffer, MAX_BUFF_LEN,&width, &height, &channels, 0);
	return data != NULL;
}



bool Image::write(const char* filename)
{
	int done;
	ImageType type = imageFileType(filename);
	if (from_buffer) {
		mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
		int output = open(filename, O_RDWR | O_CREAT | O_TRUNC, mode);
		::write(output, data, size);
		close(output);
		return done != 0;
	}
	
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

Histogram Image::cumulative_histogram(int desired_channel,Histogram h) {

	int step;

	if (!desired_channel) {
		desired_channel = 1;
		step = 1;
	}
	else {
		step = channels;
	}

	for (int i = desired_channel - 1; i < size; i += step) {
		h.histogram_data[data[i]]++;
	}

	return h;
}



void Image::stats(const char* filename)
{
	printf("Image width x image heigth: %d  x %d \n", width, height);
	printf("Number of color channels: %d \n", channels);
	printf("Image size in Bites: %d B\n", size);
}

Histogram Image::histogram()
{
	return histogram(0);
}

Histogram::Histogram()
{
	for (int i = 0; i < HIST_ARRAY_SIZE; i++) {
		histogram_data[i] = 0;
	}
}


//point operations
Image& Image::grayscale()
{
	
	if (channels < 3) {
		printf("Sorry, but it seems that image has less than 3 (RGB) channels");
	}
	else {
		int i = 0;
		if (from_buffer) {
			i = 17;
		}
		for (i; i < size; i += channels) {
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
			data[i] = clamp(contrast*(data[i] -128)+128);
			data[i+1] = clamp(contrast * (data[i+1] - 128) + 128);
			data[i+2] = clamp(contrast * (data[i+2] - 128) + 128);
		}
	}
	return *this;
}

Image& Image::boost_color(char channel) {
	int min = find_min(data, size,channels);
	int i;
	switch (channel)
	{
	case 'r':
		i = 0;
		break;
	case 'g':
		i = 1;
		break;
	case 'b':
		i = 2;
		break;
	default:
		break;
	}
	for (i; i < size; i += channels) {
		if (data[i] > 125) {
			data[i] = 255;
			data[i+1] = 0;
			data[i+2] = 0;
		}
	}
	return *this;
}



Histogram Image::treshold()
{
	Histogram H_data;
	int treshVal = mid_range_tresh_value(H_data);
	for (int i = 0; i < width * height * channels; i++) {
		if (data[i] < treshVal) {
			H_data.histogram_data[0] += 1;
		}
		else {
			H_data.histogram_data[1] += 1;
		}
	}
	return H_data;
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


Image& Image::transform()
{
	// TODO: insert return statement here
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
Image& Image::brightness_by_treshold(int treshold)
{
	int value;

	if (channels < 3) {
		printf("Sorry, but it seems that image has less than 3 (RGB) channels");
	}
	else {
		for (int i = 0; i < size; i += channels) {
			if (data[i] < treshold) {
				value = treshold;
			}
			else if (data[i] > treshold) {
				value = -treshold;
			}
			else {
				value = 0;
			}
			data[i] = clamp(value + data[i]);
			data[i + 1] = clamp(value + data[i + 1]);
			data[i + 2] = clamp(value + data[i + 2]);
		}
	}
	return *this;
	// TODO: insert return statement here
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


//useful functions

int find_min(int arr[], int sz) {
	int min = find_max(arr,sz);
	for (int i = 0; i < sz; i++) {
		if (arr[i] < min) {
			min = arr[i];
		}
	}
	return min;
}

int find_min(uint8_t arr[], int sz,int channels) {
	int min = 256;
	for (int i = 0; i < sz; i+=channels) {
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

int find_max_at(int arr[], int sz) {
	int max = 0;
	int i;
	for (i = 0; i < sz; i++) {
		if (arr[i] > arr[max]) {
			max = i;
		}
	}
	return max;
}

int find_min_at(int arr[], int sz) {
	int min = 255;
	int i;
	for (i = 0; i < sz; i++) {
		if (arr[i] < arr[min]) {
			min = i;
		}
	}
	return min;
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

size_t Image::size_from_buffer(uint8_t* buffer) {
	int i = 0;

	for (i; i < width * height * channels; i++) {
		if (buffer[i]*1) {}
		else
		{
			return i;
		}
	}

}


//Histogram own methods
Histogram& Histogram::average_value() {
	for (int i = 0; i < 256; i++) {
		histogram_data[i] = histogram_data[i] / 5;
	}
	return *this;
}

Histogram& Histogram::statistics() { 
	for (int i = 0; i < 256; i++) {
		mean += histogram_data[i];
		median += histogram_data[i];
	}
	mean = mean / 255;

	//median
	return *this;
}
Histogram Histogram::equalisation_parameters()
{
	return Histogram();
}

float Histogram::diffs(Histogram a, Histogram b)
{
	float val_a;
	float val_b;
	for (int i = 0; i < 256; i++) {
		val_a += a.histogram_data[i];
		val_b += b.histogram_data[i];
	}
	return val_a / val_b;
}

float mid_range_tresh_value(Histogram c)
{
	return (find_max_at(c.histogram_data,256)- find_min_at(c.histogram_data, 256))/2;
}

