#ifndef PROTOCOL_DEFINES
#define PROTOCOL_DEFINES

#define UDP_RECV_BUF_SIZE   1024*1024*16
#define UDP_SEND_BUF_SIZE   1024*1024*16

#define RTP_MAX_BUF_SIZE   (1920 * 1080 * 4)

#define IPMAXLEN              20
#define UDP_MAX_SIZE          65536
#define RTP_FIXED_HEADER_SIZE 12
#define RTP_PAYLOAD_TYPE_H264 96
//109
#define RTP_FRAGMENT_SIZE     1400

#define RTP_H264_FU_A         28
#define RTP_H264_FU_B         29


/*h265---------------------------*/
#define RTP_HEVC_PAYLOAD_HEADER_SIZE       2
#define RTP_HEVC_FU_HEADER_SIZE            1
#define RTP_HEVC_DONL_FIELD_SIZE           2
#define RTP_HEVC_DOND_FIELD_SIZE           1
#define RTP_HEVC_AP_NALU_LENGTH_FIELD_SIZE 2
#define HEVC_SPECIFIED_NAL_UNIT_TYPES      48
/*h265 end--------------------*/

#endif // PROTOCOL_DEFINES

