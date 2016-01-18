#ifndef VIDEO_CAPTURE_TASK_H
#define VIDEO_CAPTURE_TASK_H

#include <ace/Task.h>

class My_Video_Capture;
class Video_Capture_Task : public ACE_Task<ACE_NULL_SYNCH>
{
public:
    typedef std::function< int (int w, int h, char**, int*) > VCapture_Cb;
    Video_Capture_Task(VCapture_Cb* cb);
    ~Video_Capture_Task();

    int init(char flag = 'E',  int w = 352, int h = 288, int fps = 24, char const *device = "/dev/video0");

    int start();
    void stop();

    int svc(void);

private:
    bool        _is_run;
    VCapture_Cb*    _handler_cb;
    My_Video_Capture*   _video_capture;
};

#endif // VIDEO_CAPTURE_TASK_H
