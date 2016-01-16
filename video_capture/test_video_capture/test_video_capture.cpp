#include <iostream>
#include <../video_capture/my_video_capture_impl.h>

using namespace std;

int my_video_capture_init()
{
    cout << "this is " << __FUNCTION__ << endl;
    return 0;
}

int my_video_capture_release()
{
    cout << "this is " << __FUNCTION__ << endl;
    return 0;
}

int my_video_capture_set_video_param()
{
    cout << "this is " << __FUNCTION__ << endl;
    return 0;
}

int my_video_capture_capture()
{
    cout << "this is " << __FUNCTION__ << endl;
    return 0;
}

int my_video_capture_operator()
{
    cout << "this is " << __FUNCTION__ << endl;
    return 0;
}

int my_video_capture_impl_init()
{
    char flag = 'c';
    int w = 345;
    int h = 222;
    int fps = 223;
    char const *device;

    My_Video_Capture_Impl *impl;
    //impl->init(flag, w, h ,fps);
    return 0;
 }
    
#if 0    
int my_video_capture_impl_release()
{
	cout << "this is " << __FUNCTION__ << endl;
    return 0;
}
    
int my_video_capture_impl_set_video_param()
{
    cout << "this is " << __FUNCTION__ << endl;
   	return 0;
}
    
int my_video_capture_impl_capture()
 {
    cout << "this is " << __FUNCTION__ << endl;
	return 0;
 }
#endif
    
int main(int argc, char * argv[])
{
	int result = 0;

	if (0 == strcmp(argv[1], "capture_init"))
    {
    	result = my_video_capture_init();
        return result;
    }
    else if (0 == strcmp(argv[1], "capture_release"))
    {
        my_video_capture_release();
        return 0;
	}
   	else if (0 == strcmp(argv[1], "capture_set_video_param"))
    {   
		my_video_capture_set_video_param();
        return 0;
    }  
   	else if (0 == strcmp(argv[1], "capture_capture"))
    {   
		my_video_capture_capture();
        return 0;
    }  
   	else if (0 == strcmp(argv[1], "capture_operator"))
    {   
		my_video_capture_operator();
        return 0;
    }  

	return 0;
}

