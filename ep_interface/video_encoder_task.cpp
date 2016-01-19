#include <cstring>
#include "video_encoder_task.h"

Video_Encoder_Task::Video_Encoder_Task(const Encoder_Param &encode_param, Video_Encoder::NALU_CB *cb)
    :_allocator(8)
{
    _video_encoder = Video_Encoder_Factory::create(encode_param, cb);
}

Video_Encoder_Task::~Video_Encoder_Task()
{
    Video_Encoder_Factory::destroy(_video_encoder);
}

int Video_Encoder_Task::start()
{
    msg_queue()->activate();
    activate();

    return 0;
}

void Video_Encoder_Task::stop()
{
    msg_queue()->deactivate();
    wait();
}

int Video_Encoder_Task::svc()
{
    ACE_DEBUG( (LM_ERROR, ACE_TEXT("%s: ************\n"), __PRETTY_FUNCTION__) );
    Video_Encoder_Buf* encoder_buf = NULL;
    int rc = -1;
    unsigned char* yuv[3];
    while( true )
    {
        rc = getq(encoder_buf);
        if(-1 == rc )
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:getq (deactived) error!\n"), __PRETTY_FUNCTION__));
            break;
        }

        if( !encoder_buf )
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:active close msg(encoder_buf == NULL) error!\n"), __PRETTY_FUNCTION__));
            break;
        }

        yuv[0] = (unsigned char*)encoder_buf->buf_0;
        yuv[1] = (unsigned char*)encoder_buf->buf_1;
        yuv[2] = (unsigned char*)encoder_buf->buf_2;
        (*_video_encoder)(yuv, encoder_buf->linesize);
        _allocator.free(encoder_buf);
    }

    return 0;
}

int Video_Encoder_Task::operator ()(int w, int h, char **yuv, int *linesize)
{
    Video_Encoder_Buf* encoder_buf = (Video_Encoder_Buf*)_allocator.malloc();
    if( !encoder_buf || !yuv || !linesize )
    {
        ACE_DEBUG( (LM_ERROR, ACE_TEXT("%s:!!!!! err\n"), __PRETTY_FUNCTION__) );
        return -1;
    }

    memcpy(encoder_buf->buf_0, yuv[0], h * linesize[0]);
    memcpy(encoder_buf->buf_1, yuv[1], h * linesize[1] / 2);
    memcpy(encoder_buf->buf_2, yuv[2], h * linesize[2] / 2);
    encoder_buf->w = w;
    encoder_buf->h = h;
    encoder_buf->linesize[0] = linesize[0];
    encoder_buf->linesize[1] = linesize[1];
    encoder_buf->linesize[2] = linesize[2];

    putq(encoder_buf);

    return 0;
}

