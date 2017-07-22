/*
 * 2017.07.05：Designed by 何晟宇
 * */
#include <iostream>
#include <fstream>
#include "filesys.h"


extern unsigned short Current_Uid;     //当前操作文件系统的用户
extern unsigned short Current_Gid;       //当前操作系统的用户level
extern char Current_Dir_Name[10];   //当前所在目录表
extern struct BasicDir Dir_Basic;  //基本目录表
extern struct SubDir Dir_Current;  //当前目录表
extern Direct Copy_Item;
extern byte * disk ;

using namespace std;

/*全局变量区*/
byte * disk_it;//内存虚拟磁盘起始物理地址

//fstream it;//磁盘文件的内部位移指针

/*函数声明区*/
/*内存内部格式化*/
void Format_Superblock(byte *disk);//格式化超级块：初始化
void Format_Superblock_Initfreestack(unsigned int *data_freestack,unsigned int begin_address,unsigned int num_rest_block);
//格式化超级块：初始化成组链接法堆栈
void Format_Dinode(byte *disk);
void Format_DIR(byte *disk);
void Format_Superblock_Initbitmap(bool **s_inode_bitmap);/*格式化超级块：初始化位示图*/
//void Format_Superblock_Initfreestack(unsigned int *data_freestack);
//void Format_Datafreestack(unsigned int begin_block, unsigned int num_rest_block);
///*针对磁盘空间格式化*/
//void Format_Superblock();
//void Format_Superblock_Initfreestack(unsigned int *data_freestack);
//void Format_Datafreestack(unsigned int begin_block, unsigned int num_rest_block);

/*格式化虚拟磁盘*/
void Format(byte *disk)
{
    disk_it = disk;//获取虚拟磁盘在内存中的起始位置
    Format_Superblock(disk);//超级块格式化
    Format_Dinode(disk);
    Format_DIR(disk);//DIR区格式化

    /*初始化磁盘i节点区*/
}

/*格式化：初始化超级块*/
void Format_Superblock(byte *disk)
{
/*初始化超级块*/
    Sblock* temp = (Sblock*)(disk + START_OF_SUPER);
    temp->s_inode_size = NUM_OF_DINODEBLOCK;// 磁盘i节点块数
    temp->s_inode_freesize = NUM_OF_DINODEBLOCK;
    temp->s_rinode = 0;
    temp->s_data_size = NUM_OF_DATABLOCK;
    temp->s_data_freesize = NUM_OF_DATABLOCK;
    temp->s_pfree = 0;
    temp->s_ninode = NUM_OF_DINODEBLOCK;
    temp->s_data_usedblocknum = 0;
    temp->s_pinode = 0;
    temp->s_smod = 0;
    /*格式化：位示图初始化填0*/
    for (int i = 0; i < BITMAP_ROW; ++i)
        for (int j = 0; j < BITMAP_COLUMN; ++j)
            temp->s_inode_bitmap[i][j] = 0;
    Format_Superblock_Initfreestack(temp->s_data_freestack,START_OF_DATA,NUM_OF_DATABLOCK);//格式化：初始化超级块成组链接法堆栈
   // Format_Superblock_Initbitmap((bool **)(temp->s_inode_bitmap));//格式化：初始化位示图
   // Format_Superblock_Initfreestack(temp->s_data_freestack);
    //memmove(disk_it+START_OF_SUPER,&temp,sizeof(Sblock));//移动到虚拟磁盘空间
    delete(&temp);
    /*初始化超级块完成*/
}

/*格式化：初始化超级块成组链接法堆栈*/
void Format_Superblock_Initfreestack(unsigned int *data_freestack, unsigned begin_address,unsigned int num_rest_block)
{
    if(num_rest_block <= NUM_OF_FREESTACK)//如果是最后一组
    {
        data_freestack[0] = num_rest_block;
        for (int i = 1; i <= num_rest_block; ++i)
        {
            data_freestack[i] = begin_address ;
            begin_address += SIZE_OF_BLOCK;
        }
    }
    else//如果不是最后一组
    {
        data_freestack[0] = NUM_OF_FREESTACK;//0号单元用于存储栈中块个数
        for (int i = 1; i <= NUM_OF_FREESTACK; ++i)//从1号单元开始每一个从数据区的头单元填写其地址
        {
            data_freestack[i] = begin_address ;
            begin_address += SIZE_OF_BLOCK;
        }
        begin_address = data_freestack[NUM_OF_FREESTACK]+SIZE_OF_BLOCK;//末尾块地址+一个字节大小=后面块的开头
        num_rest_block = num_rest_block - NUM_OF_FREESTACK;
        Format_Superblock_Initfreestack((unsigned int *)(disk_it+data_freestack[1]),begin_address, num_rest_block);
    }
}

/*格式化：初始化超级块位示图*/
void Format_Superblock_Initbitmap(bool **s_inode_bitmap)
{
    /*格式化：位示图初始化填0*/
    for (int i = 0; i < BITMAP_ROW; ++i)
        for (int j = 0; j < BITMAP_COLUMN; ++j)
            s_inode_bitmap[i][j] = 0;

}

/*格式化：初始化i节点区*/
void Format_Dinode(byte *disk)
{
    disk_it = disk + START_OF_DINODE ;
    for (int i = 0; i < NUM_OF_DINODEBLOCK; ++i)
    {
        Dinode temp ;
        temp.di_mode = '4';
        temp.di_size = 0;
        temp.di_number =0;
        temp.di_gid = 0;
        temp.di_uid = 0;
        for (int j = 0; j < 10; ++j)
        {
            temp.di_addr[j] = 0;
        }
        memmove(disk_it,&temp,SIZE_OF_DINODEBLOCK);
        disk_it += SIZE_OF_DINODEBLOCK;
    }
}

/*格式化：初始化用户目录表区*/
void Format_DIR(byte *disk)
{
    disk_it = disk + START_OF_DIR;
    BasicDir temp ;
    for (int i = 0; i < NUM_OF_DIR; ++i)
    {
        temp.basicdir[i].type = 2;
    }
    //memmove(Dir_Basic, &temp, SIZE_OF_DIR);
    memmove(disk_it,&temp,SIZE_OF_DIR);
}

///*格式化：初始化超级块中，数据区成组链接法的堆栈*/
//void Format_Superblock_Initfreestack(unsigned int *data_freestack)
//{
//    unsigned int begin_address = START_OF_DATA;
//    data_freestack[0] = NUM_OF_FREESTACK;//0号单元用于存储栈中块个数
//    for (int i = 1; i <= NUM_OF_FREESTACK; ++i)//从1号单元开始每一个从数据区的头单元填写其地址
//    {
//        data_freestack[i] = begin_address;
//        begin_address += SIZE_OF_BLOCK;
//    }
//    disk_it = disk_it + data_freestack[1];
//    Format_Datafreestack(data_freestack[data_freestack[0]]+SIZE_OF_BLOCK, NUM_OF_DATABLOCK);
//}
//
///*格式化：数据区成组连接组长块填写。*/
//void Format_Datafreestack(unsigned int begin_block, unsigned int num_rest_block)//begin_block是需要开始填充的地址。
//{
//    //unsigned int temp [NUM_OF_FREESTACK + 1];
//    unsigned int temp[NUM_OF_FREESTACK +1];
//    //unsigned int * temp = (unsigned int *) disk_it;
//    if(num_rest_block < NUM_OF_FREESTACK)
//    {
//        temp[0] = num_rest_block;
//        for (int i = 1; num_rest_block != 0; ++i)
//        {
//            temp[i] = begin_block;
//            begin_block += SIZE_OF_BLOCK;
//            num_rest_block -= 1;
//        }
//        for (int i = num_rest_block +1; i <= NUM_OF_FREESTACK; ++i)
//        {
//            temp[i] = 0;
//        }
////        if(begin_block == SIZE_OF_DISK)
////            cout << "Info : From Format_Datafreestack last block : num_rest_block OK. "<<endl;
////        fwrite(&temp,SIZE_OF_BLOCK,1,it);
//    }
//    else
//    {
//        cout <<begin_block <<endl;
//        temp[0] = NUM_OF_FREESTACK;
//        for (int i = 1; i <= NUM_OF_FREESTACK; ++i)
//        {
//            temp[i] = begin_block;
//            begin_block += SIZE_OF_BLOCK;
//            num_rest_block -= 1;
//        }
////        fwrite(&temp,SIZE_OF_BLOCK,1,it);
////        fseek(it,(long)temp[1],0);//游标定位到当前块的1号单元中的地址。
//        cout <<"COPY"<<endl;
//        memmove(disk_it,temp, sizeof(temp));
//        disk_it = it + temp[1];
//        Format_Datafreestack(begin_block,num_rest_block);
//    }
//}

///*格式化：初始化用户目录表区*/
//void Format_DIR()
//{
//
//}

///*格式化虚拟磁盘*/
//void Format()
//{
//   // disk_it = disk;//获取虚拟磁盘在内存中的起始位置
//    Format_Superblock();//超级块格式化
//
//    // Save_disk(disk);
//    /*初始化磁盘i节点区*/
//}
//
///*格式化：初始化引导块*/
//void Format_Bootblock()
//{
//    //fseek(it,0,0);//将文件内部指针移动到第一块的位置。
//    for (int i = 0; i < SIZE_OF_BLOCK; ++i)//填充0
//    {
//        char j = 0;
//      //  fwrite(&j, sizeof(char),1,it);
//        //fseek(it,i+1,0);
//    }
//    //fseek(it,0,0);//将文件内部指针移动到第一块的位置。
//}
//
///*格式化：初始化超级块*/
//void Format_Superblock()
//{
///*初始化超级块*/
//    it.seekg(START_OF_SUPER,ios::beg );
//   // fseek(it,START_OF_SUPER,0);//将文件内部指针移动到超级块起始位置。
//    Sblock *temp = (Sblock *)malloc(SIZE_OF_SUPERBLOCK);
//  //  Format_Superblock_Initfreestack(temp->s_data_freestack);//格式化：初始化超级块中，数据区成组链接法的堆栈
//    temp->s_inode_size = NUM_OF_DINODEBLOCK;// 磁盘i节点块数
//    temp->s_inode_freesize = NUM_OF_DINODEBLOCK;
//    temp->s_rinode = 0;
//    temp->s_data_size = NUM_OF_DATABLOCK;
//    temp->s_data_freesize = NUM_OF_DATABLOCK;
//    temp->s_pfree = 0;
//    temp->s_ninode = NUM_OF_DINODEBLOCK;
//    temp->s_data_usedblocknum = 1;
//    temp->s_pinode = 1;
//    temp->s_smod = 1;
//
//    //ofstream test;
//    //test.open("testlog_Initfreestack",ios_base::out|ios_base::trunc);
//
//
//  //  Format_Superblock_Initfreestack(temp->s_data_freestack);//格式化：初始化超级块成组链接法堆栈
////    Format_Superblock_Initfreestack(temp->s_data_freestack,START_OF_DATA,NUM_OF_DATABLOCK);//格式化：初始化超级块成组链接法堆栈
// //   Format_Superblock_Initbitmap((**temp->s_inode_bitmap));//格式化：初始化位示图
//    it.seekg(START_OF_SUPER,ios::beg );
//    it.write((char*)temp, sizeof(Sblock));
////    char *t = (char*)temp;
////    for (int j = 0; j < LEN_OF_SUPER; ++j)
////    {
////        it.write(t,SIZE_OF_BLOCK);
////        t += SIZE_OF_BLOCK;
////        it.seekg(SIZE_OF_BLOCK,ios::cur );
////    }
//
//  //  Format_Datafreestack(temp->s_data_freestack[NUM_OF_FREESTACK]+SIZE_OF_BLOCK,temp->s_data_freesize - temp->s_data_freestack[0]);
//    //把堆栈中最后一个块的地址再往后一个扇区，作为需要开始填充的数据块编号。剩余未初始化块数量为总空闲块-堆栈中空闲块数量
//
//    /*初始化超级块完成*/
//}
//
///*格式化：初始化超级块中，数据区成组链接法的堆栈*/
//void Format_Superblock_Initfreestack(unsigned int *data_freestack)
//{
//    unsigned int begin_address = START_OF_DATA;
//    ofstream test;
//    test.open("testlog_Initfreestack",ios_base::out|ios_base::trunc);
//    data_freestack[0] = NUM_OF_FREESTACK;//0号单元用于存储栈中块个数
//    test <<"Line"<< 0 <<" : "<<data_freestack[0]<<endl;
//    for (int i = 1; i <= NUM_OF_FREESTACK; ++i)//从1号单元开始每一个从数据区的头单元填写其地址
//    {
//        data_freestack[i] = begin_address;
//        begin_address += SIZE_OF_BLOCK;
//        test <<"Line"<< i <<" : "<<data_freestack[i]<<endl;
//    }
//    test.close();
//}
//
///*格式化：数据区成组连接组长块填写。*/
//void Format_Datafreestack(unsigned int begin_block, unsigned int num_rest_block)//begin_block是需要开始填充的地址。
//{
//    unsigned int temp [NUM_OF_FREESTACK + 1];
//
//    if(num_rest_block < NUM_OF_FREESTACK)
//    {
//        temp [0] = num_rest_block;
//        for (int i = 1; num_rest_block != 0; ++i)
//        {
//            temp[i] = begin_block;
//            begin_block += SIZE_OF_BLOCK;
//            num_rest_block -= 1;
//        }
//        for (int i = num_rest_block +1; i <= NUM_OF_FREESTACK; ++i)
//        {
//            temp[i] = 0;
//        }
////        if(begin_block == SIZE_OF_DISK)
////            cout << "Info : From Format_Datafreestack last block : num_rest_block OK. "<<endl;
////        fwrite(&temp,SIZE_OF_BLOCK,1,it);
//    }
//    else
//    {
//        temp [0] = NUM_OF_FREESTACK;
//        for (int i = 1; i <= NUM_OF_FREESTACK; ++i)
//        {
//            temp[i] = begin_block;
//            begin_block += SIZE_OF_BLOCK;
//            num_rest_block -= 1;
//        }
////        fwrite(&temp,SIZE_OF_BLOCK,1,it);
////        fseek(it,(long)temp[1],0);//游标定位到当前块的1号单元中的地址。
//        Format_Datafreestack(begin_block,num_rest_block);
//    }
//}
//
///*格式化：初始化超级块位示图*/
//void Format_Superblock_Initbitmap(bool *s_inode_bitmap[])
//{
//    /*格式化：位示图初始化填0*/
//    for (int i = 0; i < BITMAP_ROW; ++i)
//        for (int j = 0; j < BITMAP_COLUMN; ++j)
//            s_inode_bitmap[i][j] = 0;
//}
//
///*格式化：初始化用户目录表区*/
//void Format_DIR()
//{
//
//}
