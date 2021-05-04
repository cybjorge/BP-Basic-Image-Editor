#include <cstdio>
#include <stdint.h>

#define HIST_ARRAY_SIZE 256

enum ImageType {
	PNG, 
	BMP,
	JPG,
	NONE
};
struct Histogram
{
	int histogram_data[HIST_ARRAY_SIZE];
	int cumulative_histogram_data[HIST_ARRAY_SIZE];
	int mean;
	int median;
	int max;

	float diffs(Histogram a, Histogram b);
	Histogram& average_value();
	Histogram& statistics();
	Histogram equalisation_parameters();
	Histogram();
};

struct Image{
	uint8_t* data = NULL;
	size_t size = 0;
	ImageType type;

	int width;
	int height;
	int channels;

	bool from_buffer = false;

	Image(const char* filename);
	Image(uint8_t* buffer,size_t buflen);
	Image(int width, int height, int channels);
	Image(const Image& img);
	~Image();
	//image stats
	void stats(const char* filename);

	//histogram
	Histogram histogram();
	Histogram histogram(int desired_channel);
	Histogram cumulative_histogram(int desired_channel, Histogram h);
	Histogram treshold();
	
	
	bool read(const char* filename);
	bool read(uint8_t* buffer);
	bool write(const char* filename);

	ImageType imageFileType(const char* filename);
	//point operations declarations
	Image& grayscale ();
	Image& filterChannel(float r, float g, float b);
	Image& adjustContrast(float value);
	Image& brightness_by_treshold(int treshold);
	Image& adjustBrightness(float value);
	Image& invert();
	Image& transform();
	Image& boost_color(char channel);
	//pixel
	//histogram operation
	Image& histogram_equalisation(Histogram h);
	//tools
	size_t size_from_buffer(uint8_t* buffer);

	//filter declarations
	Image& boxFilterTxT();

};
float mid_range_tresh_value(Histogram c);

//uint8_t* pixel(Image img, int x, int y);