#ifndef MY_V4L2_TEST_H_
#define MY_V4L2_TEST_H_

#include "v4l2device.h"

class my_v4l2_Test
{
public:
    my_v4l2_Test();
    int init_test_video0();
	int init_test_video1(); 
	int v4l2_enum();
	int start_capture_test();
	int stop_capture_test();
	int get_format_test();
	int get_fps_test();
	int set_fps_test(int fps = 20);
	int supported_io_test();
    int selected_io_test();
    int lock_frame_test();
    int unlock_frame_test();
    int set_format_false_test();
    int get_formats_test();
    int get_sizes_test();
    int get_intervals_test();

};

#endif // MY_V4L2_TEST_H
