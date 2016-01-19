#include "video_decoder_task.h"

Video_Decoder_Task::Video_Decoder_Task(Video_Decoder::YUV_CB *cb,Video_Decoder::Codec_Type codec_type)
    :_codec_type(codec_type)
    ,_yuv_cb(cb)
    ,_allocator(8)
{
    _video_decoder = Video_Decoder_Factory::create(_codec_type, _yuv_cb);
}

Video_Decoder_Task::~Video_Decoder_Task()
{
    delete _video_decoder;
}

int Video_Decoder_Task::start()
{
    if( !_video_decoder )
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:create video decoder fail!\n"), __PRETTY_FUNCTION__));
        return -1;
    }

    msg_queue()->activate();
    activate();

    return 0;
}

void Video_Decoder_Task::stop()
{
    putq(NULL);
    msg_queue()->deactivate();
    wait();
}

int Video_Decoder_Task::svc()
{
    int rc = -1;
    Decoder_Nalu_Buf* nalu = NULL;


    while(true)
    {
        rc = getq(nalu);
        if( -1 == rc )
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:getq (deactived) error!\n"), __PRETTY_FUNCTION__));
            break;
        }

        if( !nalu )
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:active close msg(nalu_buf == NULL) error!\n"), __PRETTY_FUNCTION__));
            break;
        }

        (*_video_decoder)(nalu->buf, nalu->len);
        _allocator.free(nalu);
    }

    ACE_DEBUG( (LM_DEBUG, ACE_TEXT("%s: thread exit!!!!!!!!!!!!!!!!!!!!\n"), __PRETTY_FUNCTION__) );

    return true;
}

bool Video_Decoder_Task::operator ()(unsigned char *nalu_buf, int nalu_len)
{
    if( !nalu_buf )
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:nalu_buf is NULL!\n"), __PRETTY_FUNCTION__));
        return false;
    }

    Decoder_Nalu_Buf* nalu_buf_ = (Decoder_Nalu_Buf*)_allocator.malloc();
    if( !nalu_buf_ )
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s:Decoder_Nalu_Buf malloc fail!\n"), __PRETTY_FUNCTION__));
        return false;
    }

    nalu_buf_->len = nalu_len;
    ACE_OS::memcpy(nalu_buf_->buf, nalu_buf, nalu_len);
    putq(nalu_buf_);

    return true;
}

