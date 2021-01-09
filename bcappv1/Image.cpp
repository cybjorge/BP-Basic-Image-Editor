#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"

Image::Image(const char* filename)
{
	if (read(filename)) {
		printf("Success reading %s \n", filename);
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
