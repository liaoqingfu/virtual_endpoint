#include <iostream>
#include "my_video_device_test.h"
#include <unistd.h>

My_Video_Device_Test::My_Video_Device_Test()
{
}

int My_Video_Device_Test::video_device_init_test()
{
    char filename[] = "/dev/video0";
    int ret = access( filename, F_OK);
    if(ret != 0){
        std::cout<<"device video not exist!"<<std::endl;
        return !ret;
    }
    My_Video_Device temp;
    int init = temp.init(filename);
    if(init == 0 ){
        std::cout<<"init succeed"<<std::endl;
        return 0;
    }else{
        std::cout<<"init error"<<std::endl;
        return -1;
    }
}

int My_Video_Device_Test::run_video_device_test()
{
    char filename[] = "/dev/video0";
    My_Video_Device my_video_device;
    my_video_device.init( filename );
    list<My_Device_Info>* info = NULL;
    info = my_video_device.get_my_device_info();

    if (info != NULL)
    {
        list<My_Device_Info>::iterator iter = info->begin();
        while( iter != info->end() )
        {
            cout << "type:\t" << iter->pixelformat << "\t" << iter->_flag << "\n";
            cout << "sizes:\t" << iter->_width << "x" << iter->_height << "\n";
            cout << "fps:\t" << iter->_fps << "\n\n";
            iter ++;
        }
        return 0;
    }
    else{
        std::cout<<"run video device error"<<std::endl;
        return -1;
    }
}

//int My_Audio_Device_Test::run()
//{
//    My_Audio_Device my_audio_device;
//    my_audio_device.init();

//    std::unordered_map<string, Audio_Device_Info>* my_audio_info = 0;
//    my_audio_info = my_audio_device.get_audio_device_info();
//    std::unordered_map<string, Audio_Device_Info>::iterator iter = my_audio_info->begin();
//    while (iter != my_audio_info->end())
//    {
//        std::cout << "\nDevice Name = " << iter->second.device_info.name << '\n';
//        if ( iter->second.device_info.probed == false )
//            std::cout << "Probe Status = UNsuccessful\n";
//        else
//        {
//            std::cout << "Probe Status = Successful\n";
//            std::cout << "Output Channels = " << iter->second.device_info.outputChannels << '\n';
//            std::cout << "Input Channels = " << iter->second.device_info.inputChannels << '\n';
//            std::cout << "Duplex Channels = " << iter->second.device_info.duplexChannels << '\n';
//            if ( iter->second.device_info.isDefaultOutput ) std::cout << "This is the default output device.\n";
//            else std::cout << "This is NOT the default output device.\n";
//            if ( iter->second.device_info.isDefaultInput ) std::cout << "This is the default input device.\n";
//            else std::cout << "This is NOT the default input device.\n";
//            if ( iter->second.device_info.nativeFormats == 0 )
//                std::cout << "No natively supported data formats(?)!";
//            else
//            {
//                std::cout << "Natively supported data formats:\n";
//                if ( iter->second.device_info.nativeFormats & RTAUDIO_SINT8 )
//                    std::cout << "  8-bit int\n";
//                if ( iter->second.device_info.nativeFormats & RTAUDIO_SINT16 )
//                    std::cout << "  16-bit int\n";
//                if ( iter->second.device_info.nativeFormats & RTAUDIO_SINT24 )
//                    std::cout << "  24-bit int\n";
//                if ( iter->second.device_info.nativeFormats & RTAUDIO_SINT32 )
//                    std::cout << "  32-bit int\n";
//                if ( iter->second.device_info.nativeFormats & RTAUDIO_FLOAT32 )
//                    std::cout << "  32-bit float\n";
//                if ( iter->second.device_info.nativeFormats & RTAUDIO_FLOAT64 )
//                    std::cout << "  64-bit float\n";
//            }
//            if ( iter->second.device_info.sampleRates.size() < 1 )
//                std::cout << "No supported sample rates found!";
//            else
//            {
//                std::cout << "Supported sample rates = ";
//                for (unsigned int j=0; j<iter->second.device_info.sampleRates.size(); j++)
//                    std::cout << iter->second.device_info.sampleRates[j] << " ";
//            }
//            std::cout << std::endl;
//        }
//        iter++;
//    }
//    std::cout << std::endl;
//	return 0;
//}


