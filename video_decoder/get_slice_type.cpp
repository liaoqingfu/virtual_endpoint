#include "get_slice_type.h"
#include <string.h>
#include <stdio.h>
#define LOG_INFO  printf
#define LOG_ERROR printf
#define LOG_DEBUG printf
#define LOG_NOTICE printf



int BS_init(BS *bs, unsigned char *buf, int len)
{
    memset(bs, 0, sizeof(BS));

    bs->buf = buf;
    bs->totallen = len;
    bs->pos = 0;

    return 0;
}

const unsigned char rs[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };

int BS_showbits(BS *bs, int bits)
{
    int value = 0;

    int i;
    for ( i=0; i<bits; i++ )
    {
        int cur_pos = bs->pos + i;
        int pos8 = cur_pos/8;
        int left = cur_pos - pos8*8;

        value = (value<<1) | ((bs->buf[pos8]>>rs[left])&0x1);
    }

    return value;
}

int BS_getbits(BS *bs, int bits)
{
    int value = 0;

    value = BS_showbits(bs, bits);
    bs->pos += bits;

    return value;
}

int getslicetype(unsigned char *buf, int len)
{
    BS bs;
    int slicetype = 0;

    BS_init(&bs, buf, len);

    int i;

    /**/
    while (1)
    {
        int flag = 0;
        for ( i=0; i<len-3; i++ )
        {
            if (1==BS_showbits(&bs, 24))
            {
                BS_getbits(&bs, 24);
                flag = 1;
                break;
            }
            else
                BS_getbits(&bs, 8);
        }
        if (flag==0)
            break;
    }

    return slicetype;
}


unsigned int get_ue_value(uint8_t *pBuff, unsigned int nLen, unsigned int &nStartBit)
{
    //计算0bit的个数
    unsigned int nZeroNum = 0;
    while (nStartBit < nLen * 8)
    {
        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
        {
            break;
        }
        nZeroNum++;
        nStartBit++;
    }
    nStartBit ++;

    //计算结果
    int dwRet = 0;
    for (unsigned int i=0; i<nZeroNum; i++)
    {
        dwRet <<= 1;
        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
        {
            dwRet += 1;
        }
        nStartBit++;
    }
    return (1 << nZeroNum) - 1 + dwRet;
}


int get_nal_slice_type( unsigned char *nalu, int nalu_len )
{
    int len = nalu_len;
    unsigned char i_start[] = {0x00, 0x00, 0x01};
    unsigned char i_start1[] = {0x00, 0x00, 0x00, 0x01};

    uint8_t* nalu_buf = (uint8_t*)nalu;
    int v = -1;
    unsigned int slice_type = 0;

    if(0 == memcmp(nalu_buf, i_start, 3))
    {
        v = (*(nalu_buf + 3)) & 0x1F;
        if( v == 1 )
        {
            unsigned int index1 = 0;
            get_ue_value(nalu_buf + 4, len - 4, index1);

            slice_type = get_ue_value(nalu_buf + 4, len - 4, index1);

            if( (slice_type == 2) || (slice_type == 4) || (slice_type == 7) || (slice_type == 9)) //
            {
                LOG_NOTICE("first i found i v = %d slice_type = %u \n", v, slice_type);
                return v;
            }
        }
        else if( (v == 7)  ) //|| (v == 5)
        {

            return v;
        }
    }
    else if(0 == memcmp(nalu_buf, i_start1, 4))
    {
        //for(int i = 0; i < 11; i++)
        //{
            //printf("%.2x ", *(nalu_buf +i));
        //}
        //printf("\n");
        v = (*(nalu_buf + 4)) & 0x1F;
        if( v == 1 )
        {
            //哥伦布解码ue(2)
            unsigned int index1 = 0;
            get_ue_value(nalu_buf +5, len -5, index1);

            int slice_type = get_ue_value(nalu_buf +5, len -5, index1);
            LOG_INFO("slice_type = %u \n",slice_type);
            if( (slice_type == 2) || (slice_type == 4) || (slice_type == 7) || (slice_type == 9) )
            {
                LOG_NOTICE("first i found iii v = %d slice_type = %u \n", v, slice_type);
                return v;
            }
        }
        else if( (v == 7) )  //|| (v == 5)
        {
            LOG_NOTICE("first i found iiii v = %u\n", v);
            return v;
        }
    }
    return v;
}
