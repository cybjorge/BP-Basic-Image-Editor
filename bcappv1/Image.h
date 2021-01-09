#include <cstdio>
#include <stdint.h>

#define HIST_ARRAY_SIZE 256

enum ImageType {
	PNG, BMP, JPG,NONE
};
struct Histogram
{
	int histogram_data[HIST_ARRAY_SIZE];

	Histogram();
	~Histogram();
};

struct Image{
	uint8_t* data = NULL;
	size_t size = 0;
	int width;
	int height;
	int channels;

	Image(const char* filename);
	Image(int width, int height, int channels);
	Image(const Image& img);
	~Image();

	//histogram
	Histogram histogram();
	Histogram histogram(int desired_channel);
	
	bool read(const char* filename);
	bool write(const char* filename);

	ImageType imageFileType(const char* filename);
};