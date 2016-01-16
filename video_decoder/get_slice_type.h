
#define LOG_INFO  printf
#define LOG_ERROR printf
#define LOG_DEBUG printf
#define LOG_NOTICE printf

typedef unsigned char uint8_t;
typedef struct BS_s
{
    unsigned char *buf;
    int totallen;

    int pos;
} BS;

int BS_init(BS *bs, unsigned char *buf, int len);

int BS_showbits(BS *bs, int bits);

int BS_getbits(BS *bs, int bits);

int getslicetype(unsigned char *buf, int len);

unsigned int get_ue_value(uint8_t *pBuff, unsigned int nLen, unsigned int &nStartBit);


int get_nal_slice_type( unsigned char *nalu, int nalu_len );
