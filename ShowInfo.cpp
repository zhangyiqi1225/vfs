//
// Created by Earl on 2017/7/14.
//


#include "filesys.h"
using namespace std;
extern  byte *disk;

void Show_disk_info()
{
    cout <<endl;
    cout << "Disk information:" <<endl;
    cout << "Total disk size: \t"<< SIZE_OF_DISK << "B"<<" \t("<< SIZE_OF_DISK / SIZE_OF_BLOCK<<" blocks, "<<SIZE_OF_DISK / 1024 << " KB) "<<endl;
    cout << "Superblock size: \t"<< SIZE_OF_SUPERBLOCK << "B   \t"<< SIZE_OF_SUPERBLOCK / SIZE_OF_BLOCK<<" blocks" <<endl;
    cout << "Inode area size: \t"<< SIZE_OF_DINODE << "B    \t"<< SIZE_OF_DINODE / SIZE_OF_BLOCK<<" blocks" <<endl;
    cout << "Basic DIR area size: \t"<< SIZE_OF_DIR << "B   \t"<< SIZE_OF_DIR / SIZE_OF_BLOCK<<" blocks" <<endl;
    cout << "Data area size: \t"<< SIZE_OF_DATA<< "B   \t"<< SIZE_OF_DATA / SIZE_OF_BLOCK<<" blocks" <<endl;
    cout <<endl;
    cout << "Current users:"<<endl;
    BasicDir *it2 = (BasicDir *)(disk + START_OF_DIR);
    for (int i = 0; i < 8; ++i)
    {
        if(it2->basicdir[i].type == 1)
            cout << it2->basicdir[i].D_Name<<endl;
    }
    cout <<endl;
    Sblock *it = (Sblock *)(disk + START_OF_SUPER);
    cout <<"Total inode number:\t";
    cout << it->s_inode_size <<endl;
    cout <<"Used inode number:\t";
    cout << it->s_inode_size -  it->s_inode_freesize<<endl;
    cout <<"Idle inode number:\t";
    cout << it->s_inode_freesize <<endl;
    cout <<"inode bitmap:"<<endl;
    for (int i = 0; i < BITMAP_ROW; ++i)
    {
        for (int j = 0; j < BITMAP_COLUMN; ++j)
            cout << it->s_inode_bitmap[i][j];
        cout <<endl;
    }

    cout <<endl;
    cout <<"Total datablock number:";
    cout << it->s_data_size <<endl;
    cout <<"Used datablock number:";
    cout << it->s_data_size - it->s_data_freesize<<endl;
    cout <<"Idle datablock number:";
    cout << it->s_data_freesize<<endl;
    cout << "datablock stack:"<<endl;
    for (int k = 0; k <= NUM_OF_FREESTACK; ++k)
        cout<<k<< " : " <<it->s_data_freestack[k]<<endl;
    cout <<endl;

}
