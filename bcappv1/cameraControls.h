// DECLARATION HEADER

int camera_check(int cd);						//checks whether camera device is connected
uint8_t* camera_record_init(int cd);			//calls tools for setting resolutions, buffers, and capturing frame
int capability(int cd);					//retrieves device capability
int set_r_f(int cd); 					//set resolution and format of recorded picture
int set_buffer(int cd);					//requesting, setting and mapping buffer 
int make_frame(int cd,int loop);					//capturing the frame
int stop_stream(int cd);				//closes stream from device