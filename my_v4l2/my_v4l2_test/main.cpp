#include <iostream>
#include <cstdlib>
#include "my_v4l2_test.h"
#include <stdlib.h>
using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2){
        cout<<"return -1"<<endl;
        return -1;
    }
    int option = atoi(argv[1]);

    switch (option) {

    case 3:{
        my_v4l2_Test temp;
        return temp.init_test_video0();
        break;
    }
    case 4:{
        my_v4l2_Test temp;
        return temp.start_capture_test();
        break;
    }
    case 5:{
        my_v4l2_Test temp;
        return temp.v4l2_enum();
        break;
    }
    case 6:{
        my_v4l2_Test temp;
        return temp.init_test_video1();
        break;
    }
    case 7:{
        my_v4l2_Test temp;
        return temp.stop_capture_test();
        break;
    }
    case 8:{
        my_v4l2_Test temp;
        return temp.get_format_test();
        break;
    }
    case 9:{
        my_v4l2_Test temp;
        return temp.get_fps_test();
        break;
    }
    case 10:{
        my_v4l2_Test temp;
        return temp.set_fps_test(25);
        break;
    }
    case 11:{
        my_v4l2_Test temp;
        return temp.supported_io_test();
        break;
    }
    case 12:{
        my_v4l2_Test temp;
        return temp.selected_io_test();
        break;
    }
    case 13:{
        my_v4l2_Test temp;
        return temp.lock_frame_test();
        break;
    }
    case 14:{
        my_v4l2_Test temp;
        return temp.unlock_frame_test();
        break;
    }
    case 15:{
        my_v4l2_Test temp;
        return temp.set_format_false_test();
        break;
    }
    case 16:{
        my_v4l2_Test temp;
        return temp.get_formats_test();
        break;
    }
    case 17:{
        my_v4l2_Test temp;
        return temp.get_sizes_test();
        break;
    }
    case 18:{
        my_v4l2_Test temp;
        return temp.get_intervals_test();
        break;
    }
    default:
        break;
    }

	return 0;
}
