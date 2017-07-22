//
// Created by Earl on 2017/7/6.
//
#include <iostream>
#include "filesys.h"
using namespace std;

//int disk_address;
/*分配空闲块，返回值是地址*/
byte* Alloc_data_block(byte *disk)
{
    Sblock *disk_sblock = (Sblock *)(disk + START_OF_SUPER);//定位到超级块。
    byte * block = NULL;
    if(disk_sblock->s_data_freesize != 0)//如果还有空闲块的话
    {
        int last_unit = disk_sblock->s_data_freestack[0];//超级块数据区空闲块堆栈最后一个元素下标。
        block = (disk_sblock->s_data_freestack[last_unit] + disk);//取得一块空闲块，block指针指向它。
        disk_sblock->s_data_freestack[0]--;//栈中剩余空闲块计数-1
        disk_sblock->s_data_freesize--;//空闲数据块计数-1
        if(disk_sblock->s_data_freestack[0] == 0)//如果栈中剩余空闲块计数为0
        {
            unsigned int *next = (unsigned int *)(disk+disk_sblock->s_data_freestack[1]);//定位到下一组的物理地址。
            disk_sblock->s_data_freestack[0] = next[0];
            for (int i = 1; i <= next[0]; ++i)//把下一组空闲块的内容调到超级块堆栈中去。循环次数等于其0号单元。
            {
                disk_sblock->s_data_freestack[i] = next[i];
            }
        }
        else
        {
            disk_sblock->s_data_freestack[last_unit] = 0;//被取走的内容标记为0表示使用了。
        }
    }
    return block;
}

/*回收成为空闲块*/
void Free_data_block(byte *disk, byte *block)
{
    Sblock *disk_sblock = (Sblock *)(disk + START_OF_SUPER);//定位到超级块。
    disk_sblock->s_data_freesize++;
    if(disk_sblock->s_data_freestack[0] < NUM_OF_FREESTACK)//如果空闲块栈未满
    {
        int last = disk_sblock->s_data_freestack[0];
        disk_sblock->s_data_freestack[last+1] = (unsigned int)(block - disk);//相对于磁盘开头的偏移量存入最后一个单元
        disk_sblock->s_data_freestack[0]++;
    }
    else//如果空闲块栈满了
    {
        unsigned int * target = (unsigned int *)block;
        for (int i = 0; i <= disk_sblock->s_data_freestack[0] ; ++i)//将堆栈中内容放入该空闲块。
        {
            target[i] = disk_sblock->s_data_freestack[i];
            disk_sblock->s_data_freestack[i] = 0;
        }
        disk_sblock->s_data_freestack[0] = 1;//0号单元内容变为1
        disk_sblock->s_data_freestack[1] = (unsigned int)(block - disk);//该空闲块的偏移地址存入1号单元
    }
}

