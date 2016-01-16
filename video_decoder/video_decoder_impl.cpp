#ifdef _WIN32
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif

#include "video_decoder_impl.h"

Video_Decoder_Impl::Video_Decoder_Impl() :
    init_func(NULL), release_func(NULL),
    decode_func(NULL), impl(NULL)
{
}

Video_Decoder_Impl::~Video_Decoder_Impl()
{
}

bool   Video_Decoder_Impl::operator () (unsigned char *nalu, int nalu_len)
{
    return (*decode_func)(impl, nalu, nalu_len);
}

bool   Video_Decoder_Impl::init(YUV_CB *cb)
{
    return init_func(impl, cb);
}

int   Video_Decoder_Impl::release()
{
    int rc = (*release_func)(impl);
    //delete this;
    return rc;
}
