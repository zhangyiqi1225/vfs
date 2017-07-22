//
// Created by Earl on 2017/7/7.
//

#include "filesys.h"
using namespace std;

byte* Alloc_inode(byte *disk)
{
    Sblock *disk_sblock = (Sblock *)(disk + START_OF_SUPER);//定位到超级块。
    byte *block = NULL;
    unsigned int block_num = 0;
    if(disk_sblock->s_inode_freesize != 0)//如果还有空闲i节点 的话
    {
        disk_sblock->s_inode_freesize--;
        for (int i = 0; i < BITMAP_ROW; ++i)
        {
            for (int j = 0; j < BITMAP_COLUMN; ++j)
            {
                if(disk_sblock ->s_inode_bitmap[i][j] == 0)
                {
                    block_num = (unsigned int)(BITMAP_COLUMN*(i) + 1 + j);
                    block = disk + ((block_num -1) * SIZE_OF_DINODEBLOCK + START_OF_DINODE);
                    disk_sblock ->s_inode_bitmap[i][j] = 1;
                    return block;
                }
            }
        }
    }
    return block;
}

void Free_inode(byte *disk, byte *block)
{
    Sblock *disk_sblock = (Sblock *)(disk + START_OF_SUPER);//定位到超级块。
    disk_sblock->s_inode_freesize++;
    unsigned int block_num = (block - disk - START_OF_DINODE) / SIZE_OF_DINODEBLOCK +1;
    unsigned int i = ((block_num - 1) / BITMAP_COLUMN);
    unsigned int j = (block_num - 1) % BITMAP_COLUMN ;
    disk_sblock ->s_inode_bitmap[i][j] = 0;
}