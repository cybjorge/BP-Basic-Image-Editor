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
	int mean;
	int median;

	/* auto compute
	mean
	variance
	median
	*/
	Histogram();
	//~Histogram();
};

struct Image{
	uint8_t* data = NULL;
	size_t size = 0;
	ImageType type;
	int width;
	int height;
	int channels;


	Image(const char* filename);
	Image(uint8_t* buffer);
	Image(int width, int height, int channels);
	Image(const Image& img);
	~Image();
	//image stats
	void stats(const char* filename);

	//histogram
	Histogram histogram();
	Histogram histogram(int desired_channel);
	Histogram treshold(int treshVal);
	
	bool read(const char* filename);
	bool read(uint8_t* buffer);
	bool write(const char* filename);

	ImageType imageFileType(const char* filename);
	//point operations declarations
	Image& grayscale ();
	Image& filterChannel(float r, float g, float b);
	Image& adjustContrast(float value);
	Image& adjustBrightness(float value);
	Image& invert();
	Image& transform();
	Image& boost_color(char channel);
	//pixel


	//filter declarations
	Image& boxFilterTxT();

};
//uint8_t* pixel(Image img, int x, int y);