#include "my_msg_block_pool.h"

My_Msg_Block_Pool::My_Msg_Block_Pool()
{
}

int My_Msg_Block_Pool::open(size_t mb_size, int mb_count)
{
    size_t sss = mb_size * mb_count;

    int rc = ACE_Message_Queue<ACE_MT_SYNCH, ACE_System_Time_Policy>::open(sss, sss);
    if(rc == 0)
    {
        for (int i = 0; i < mb_count; i++)
        {
            ACE_Message_Block* msg_block = new ACE_Message_Block( mb_size );
            if (NULL == msg_block)
            {
                return -1;
            }
            rc = enqueue( msg_block );
            if (rc <= -1)
            {
                delete msg_block;
                msg_block = NULL;
                return rc;
            }
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

ACE_Message_Block *My_Msg_Block_Pool::malloc()
{
    ACE_Message_Block *mb = NULL;
    if (is_empty())
        return NULL;
    dequeue(mb, NULL);
    return mb;
}
void My_Msg_Block_Pool::free(ACE_Message_Block *mb)
{
    if(mb != NULL)
    {
        mb->reset();
        int rc = enqueue(mb);
        if (rc < 0)
        {}
    }
}
