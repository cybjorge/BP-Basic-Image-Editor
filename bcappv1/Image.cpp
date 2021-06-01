#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define MAX_BUFF_LEN 1270*720*3

#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "cameraControls.h"
#include <algorithm>
#include <bits/stdc++.h>

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
//sorting
void swap_sort(int* x, int* y);
void bubble_sort(int array[], int array_size);
int clamp(int channelValue);
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
	if (read(buffer, buflen)) {
		printf("Image was loaded from buffer");
		size = width * height * channels;
	}
	else {
		perror("Not able to read image");
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

bool Image::read(uint8_t* buffer,size_t bufflen)
{
	data = stbi_load_from_memory(buffer, bufflen,&width, &height, &channels, 0);
	return data != NULL;
}



bool Image::write(const char* filename)
{
	int done;
	ImageType type = imageFileType(filename);	
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

	for (int i = desired_channel-1; i < size; i+=channels) {
		H_data.histogram_data[data[i]]++;
	}

	return H_data;
}

Histogram Image::cumulative_histogram(Histogram h) {

	int sum = 0;
	for (int i = 0; i < 256; i++) {
		sum += h.histogram_data[i];
		h.cumulative_histogram_data[i] = sum;
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

Image& Image::auto_local_contrast_8bit(Histogram h) {
	int range_max = 255;
	int range_min = 0;
	for (int i = 0; i < size; i += channels) {
		float contrast = (((data[i]) - h.max_index) / (h.min_index - h.max_index)) * range_max;
		data[i] = clamp(contrast * data[i] + 0.5);
		data[i+1] = clamp(contrast * data[i] + 0.5);
		data[i+2] = clamp(contrast * data[i] + 0.5);
	}
	return *this;
}
Image& Image::boost_color(char channel) {
	//int min = find_min(data, size);
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



Histogram Image::treshold(Histogram h)
{
	
	int treshVal = h.median;
	for (int i = 0; i < width * height * channels; i++) {
		if (data[i] < treshVal) {
			h.binary_treshold[0] += 1;
		}
		else {
			h.binary_treshold[1] += 1;
		}
	}
	return h;
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


Image& Image::median_filter()
{

	uint8_t* stored_data;
	stored_data =data;
	int row_len = width * channels;
	int row_offset = 0;
	int window[9] = { 0 };
	for (int i = 0; i < height; i++) {
		if (i == 0 || i == height - 1) {
			continue;
		}
		else {
			row_offset = i * row_len;
			for (int j = 0; j < row_len; j += channels) {
				//top row
				window[0] = data[row_offset - row_len];
				window[1] = data[row_offset - row_len + channels];
				window[2] = data[row_offset - row_len + 2 * channels];
				//middle row
				window[3] = data[row_offset];
				window[4] = data[row_offset + channels];
				window[5] = data[row_offset + 2 * channels];
				//bottom row
				window[6] = data[row_offset + row_len];
				window[7] = data[row_offset + row_len + channels];
				window[8] = data[row_offset + row_len + 2 * channels];
				//sort the array
				bubble_sort(window, 9);
				//add the median value to stored_data
				memset(stored_data + row_offset, window[4], 3);
				//increment the offset by channels, in this case, by 3
				row_offset += channels;
			}
		}
	}
	memcpy(data, stored_data, size);
	return *this;
}

Image& Image::adjustBrightness(float value)
{
	if (channels < 3) {
		perror("Sorry, but it seems that image has less than 3 (RGB) channels");
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
		perror("Sorry, but it seems that image has less than 3 (RGB) channels");
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
		perror("Sorry, but it seems that this image doesnt have enough color channels to preform this action");
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

Image& Image::auto_local_brightness(Histogram h) {
	int value = h.max_index - h.min_index;
	int brightness;
	if (value < 0) { value *= -1; };

	for (int i = 0; i < size; i += channels) {
		if(data[i]>data[h.min_index] && data[i] > data[h.max_index]){
			brightness = data[i] - value;
			if (brightness < 0) { brightness *= -1; };
			data[i] = clamp(value + data[i]);
			data[i + 1] = clamp(value + data[i + 1]);
			data[i + 2] = clamp(value + data[i + 2]);
		}

	}

	return *this;
}

//useful functions
int find_min(uint8_t arr[], size_t sz) {
	int min = 256;
	int i = 0;
	for (i; i < sz; i++) {
		if (arr[i] < min) {
			min = arr[i];
		}
	}
	return i;
}

int find_min(int arr[], int sz) {
	int min = 256;
	int index_min = 0;
	int i = 0;
	for (i; i < sz; i++) {
		if (arr[i] < min) {
			min = arr[i];
			index_min = i;
		}
	}
	return index_min;
}

int find_max(int arr[], int sz) {
	int max = 0;
	int index_max = 0;
	int i = 0;
	for (i; i < sz; i++) {
		if (arr[i] > max) {
			max = arr[i];
			index_max = i;
		}
	}
	return index_max;
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


Image& Image::histogram_equalisation(Histogram h)
{
	int new_values[256] = { 0 };
	int cumulative_frequency = 0;
	for (int i = 0; i < 256; i++) {
		cumulative_frequency += h.histogram_data[i];
		new_values[i] = clamp((cumulative_frequency * 255) / (width * height));
	}

	for (int j = 0; j < size; j++) {
		data[j] = new_values[data[j]];
	}

	return *this;
}



Histogram& Histogram::statistics() { 
	//median
	int median_array[HIST_ARRAY_SIZE];
	for (int i = 0; i < HIST_ARRAY_SIZE; i++) {
		median_array[i] = histogram_data[i];
	}
	bubble_sort(median_array, HIST_ARRAY_SIZE);
	median = median_array[127];
	
	//mean
	for (int i = 0; i < HIST_ARRAY_SIZE; i++) {
		mean += histogram_data[i]*i;
	}
	int mean = mean / (imagesize / 3);
	/*for (int i = 0; i < HIST_ARRAY_SIZE; i++) {
		if (histogram_data[i] == mean_index) {
			mean = i;
		}
	}*/
	//max
	max = histogram_data[find_max(histogram_data, HIST_ARRAY_SIZE)];
	max_index = find_max(histogram_data, HIST_ARRAY_SIZE);
	//mix
	min = histogram_data[find_min(histogram_data, HIST_ARRAY_SIZE)];
	min_index = find_min(histogram_data, HIST_ARRAY_SIZE);
	return *this;
}

float mid_range_tresh_value(Histogram c)
{
	return (find_max(c.histogram_data,256)+ find_min(c.histogram_data, 256))/2;
}

void swap_sort(int* x, int* y) {
	int tmp = *x;
	*x = *y;
	*y = tmp;
}
void bubble_sort(int array[], int array_size) {
	for (int i = 0; i < array_size - 1; i++) {
		for (int j = 0; j < (array_size - i - 1); j++) {
			if (array[j] > array[j + 1]) {
				swap_sort(&array[j], &array[j + 1]);
			}
		}
	}
}