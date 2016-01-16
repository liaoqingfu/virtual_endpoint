#ifndef __RTP_H__
#define __RTP_H__

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

#pragma pack(1)

typedef struct rtp_header
{
#ifdef RTP_BIG_ENDIAN
	unsigned short version     :2;  
	unsigned short padbit      :1;
	unsigned short extbit      :1;
	unsigned short cc          :4;
	unsigned short markbit     :1;
	unsigned short payload_type:7;
#else	
	unsigned short cc          :4;
	unsigned short extbit      :1;
	unsigned short padbit      :1;
	unsigned short version     :2;
	unsigned short payload_type:7;
	unsigned short markbit     :1;	
#endif
	unsigned short sequence;
	unsigned int   timestamp;
	unsigned int   ssrc;
	unsigned int   csrc[16];
} rtp_header_t;

typedef struct nalu_type
{
#ifdef RTP_BIG_ENDIAN
	unsigned char fbit:1;
	unsigned char nri :2;
	unsigned char type:5;
#else
	unsigned char type:5;
	unsigned char nri :2;
	unsigned char fbit:1;
#endif
} nalu_type_t;

typedef struct fu_indicator
{
#ifdef RTP_BIG_ENDIAN
	unsigned char fbit:1;
	unsigned char nri :2;
	unsigned char type:5;
#else
	unsigned char type:5;
	unsigned char nri :2;
	unsigned char fbit:1;
#endif
} fu_indicator_t;

typedef struct fu_header
{
#ifdef RTP_BIG_ENDIAN
	unsigned char start   :1;
	unsigned char end     :1;
	unsigned char reserved:1;
	unsigned char type    :5;
#else
	unsigned char type    :5;
	unsigned char reserved:1;
	unsigned char end     :1;
	unsigned char start   :1;
#endif
} fu_header_t;

#pragma pack()

#endif /* __RTP_H__ */
