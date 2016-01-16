#ifndef MY_DEVICE_H
#define MY_DEVICE_H

#include <string>
#include <list>
using namespace std;


struct My_Device_Info
{
    string pixelformat; /* Format fourcc      */
    int _flag;          /* 'C' 'E' 'N'  */
    int _width;         /*resolution width */
    int _height;        /*resolution height */
    int _fps;           /*fps*/
};

class My_Video_Device
{
    
public:
    My_Video_Device();
    ~My_Video_Device();
    int init(const char* device_name);
    list<My_Device_Info>*  get_my_device_info()
    {
        return &_my_device_info;
    }
private:
	My_Video_Device(const My_Video_Device&);
	My_Video_Device& operator=(const My_Video_Device&);
    list<My_Device_Info> _my_device_info;
};

#endif // MY_DEVICE_H
