#ifndef MY_VIDEO_CAPTURE_H
#define MY_VIDEO_CAPTURE_H

#include <functional>

class My_Video_Capture_Impl;
class My_Video_Capture
{
public:
    My_Video_Capture();
    ~My_Video_Capture();

    /**
     * @param flag : 'C' == comprase
     * 'N' == nataive
     * 'E' == emulate (This format is not native to the device
     * but emulated through software (usually libv4l2), where
     * possible try to use a native format instead for better
     * performance.)
     */
    /**
     * @brief init 初始化视频捕捉设备
     * @param flag 标志位：'C' == comprase,'N' == nataive,'E' == emulate
     * @param w 宽
     * @param h 高
     * @param fps 帧率
     * @param device 视频捕捉设备名称
     * @return 0：初始成功
     */
    int init(char flag = 'E',
             int w = 352, int h = 288,
             int fps = 24,
             char const *device = "/dev/video0");
    /**
     * @brief release 释放视频捕捉设备
     */
    void release();

    /**
     * @brief set_video_param 设置视频捕捉参数
     * @param flag 标志位：'C' == comprase,'N' == nataive,'E' == emulate
     * @param w 宽
     * @param h 高
     * @param fps 帧率
     * @param device 视频捕捉设备名称
     * @return 0：设置成功
     */
    int set_video_param(char flag = 'E',
             int w = 352, int h = 288,
             int fps = 24,
             char const *device = "/dev/video0");

    bool set_brightness(int value);
    bool set_hue(int value);
    bool set_saturation(int value);
    bool set_contrast(int value);

    /**
     * @brief capture 捕捉一帧视频数据，并将数据返回至指定回调
     * @param cb_func 数据回调
     * @return 0：返回成功
     */
    int capture(int(*cb_func)(int w, int h, char**, int*));
    /**
     * @brief operator () 捕捉一帧视频数据，并将数据返回至指定回调
     * @param cb_func 数据回调
     * @return 0：返回成功
     */
    int operator()(std::function< int (int w, int h, char**, int*) > *cb_func);

private:
	My_Video_Capture(const My_Video_Capture&);
	My_Video_Capture& operator=(const My_Video_Capture&);
    My_Video_Capture_Impl *_impl;
};

#endif // MY_VIDEO_CAPTURE_H
