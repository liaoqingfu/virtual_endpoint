#include "yuv2rgb.h"

#include <ace/Log_Msg.h>

YUV2RGB::YUV2RGB()
{
}

YUV2RGB::~YUV2RGB()
{
	release();
}

bool YUV2RGB::init(int src_w, int src_h, int dst_w, int dst_h)
{
	bool fr = ffmpeg_lib::init();

	if(!fr || ffmpeg_lib::sws_getContext == NULL)
	{
		ACE_DEBUG((LM_ERROR
				   , ACE_TEXT("%s : ffmpeg_lib init error\n")
				   , __PRETTY_FUNCTION__));
		return false;
	}

	_src_width = src_w;
	_src_height = src_h;
	_dst_width = dst_w;
	_dst_height = dst_h;

	_sws_ctx = ffmpeg_lib::sws_getContext(_src_width,
										  _src_height,
										  PIX_FMT_YUV420P,
										  _dst_width, _dst_height, PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if (NULL == _sws_ctx)
    {
        ACE_DEBUG((LM_ERROR
                   , ACE_TEXT("%s : _sws_ctx is NULL\n")
                   , __PRETTY_FUNCTION__));
        return false;
    }

	return true;
}

void YUV2RGB::release()
{
    ffmpeg_lib::sws_freeContext( _sws_ctx);
    _sws_ctx =NULL;
}

bool YUV2RGB::operator () (u_int8_t ** yuv, int *linesize, u_int8_t ** rgb, int *linesize_rgb) {
	ffmpeg_lib::sws_scale(_sws_ctx, yuv, linesize, 0, _src_height, rgb, linesize_rgb);
	return true;
}
