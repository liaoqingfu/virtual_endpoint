#ifndef __PROTOCOL_TYPES_H__
#define __PROTOCOL_TYPES_H__

#pragma pack(1)

typedef struct
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
} Rtp_Header;

typedef struct
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
} Nalu_Type;

typedef struct
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
} Fu_Indicator;

typedef struct
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
} Fu_Header;

#pragma pack()

#endif /* __PROTOCOL_TYPES_H__ */
