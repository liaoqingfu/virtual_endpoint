#ifndef MY_MSG_BLOCK_POOL_H
#define MY_MSG_BLOCK_POOL_H
#include <ace/Message_Queue.h>
#include <ace/Singleton.h>

#include <ace/Synch.h>
#include <ace/Synch_Traits.h>

class My_Msg_Block_Pool : public ACE_Message_Queue<ACE_MT_SYNCH, ACE_System_Time_Policy>
{
public:
    My_Msg_Block_Pool();
    int open(size_t mb_size, int mb_count);

    ACE_Message_Block *malloc();
    void free(ACE_Message_Block *);
};

class My_Msg_Block_Pool_Memcp: public My_Msg_Block_Pool
{};

typedef ACE_Singleton<My_Msg_Block_Pool, ACE_Recursive_Thread_Mutex> My_Msg_Block_Pool_Singleton;

typedef ACE_Singleton<My_Msg_Block_Pool_Memcp, ACE_Recursive_Thread_Mutex> My_Msg_Block_Pool_Memcp_Singleton;
#endif // MY_MSG_BLOCK_POOL_H
