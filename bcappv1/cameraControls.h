// DECLARATION HEADER
#define CALIBRATION 1
#define MAIN 2
#define TEST 3

int camera_check(int cd);						//checks whether camera device is connected
uint8_t* camera_record_init(int cd,int flag);			//calls tools for setting resolutions, buffers, and capturing frame
int capability(int cd);					//retrieves device capability
int set_r_f(int cd); 					//set resolution and format of recorded picture
int set_buffer(int cd);					//requesting, setting and mapping buffer 
int make_frame(int cd,int loop,int flag);					//capturing the frame
int stop_stream(int cd);				//closes stream from device


uint8_t* return_buf();
size_t return_buf_size();

void free_buffer();