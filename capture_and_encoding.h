#ifndef __CAPTURE_AND_ENCODING_H	
#define __CAPTURE_AND_ENCODING_H	

int capture_and_encoding(void); 
int destory(void);
int get_stream(int fd, int fd_low, int chn);

// implemented in imp-common.c
int set_cam_option(const char *option, int value);

#endif
