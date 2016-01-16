#include <iostream>
#include <cstdlib>
//#include "my_audio_device_test.h"
//#include "my_v4l2_test.h"
#include "my_video_device_test.h"
#include <stdlib.h>
//#include "multicast_conference_test.h"

int main(int argc, char *argv[])
{
    if(argc < 2){
        cout<<"return -1"<<endl;
        return -1;
    }
    int option = atoi(argv[1]);
//    My_Audio_Device_Test temp;
    switch (option) {


    case 19:{
        My_Video_Device_Test temp;
        return temp.video_device_init_test();
        break;
    }
    case 20:{
        My_Video_Device_Test temp;
        return temp.run_video_device_test();
        break;
    }

    }


	return 0;
}
