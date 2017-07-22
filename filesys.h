#ifndef FILESYS_filesys_H
#define FILESYS_filesys_H
/*
 * 2017.07.05：Designed by 何晟宇
 * */


#include <iostream>
#include <fstream>
#include <malloc.h>

using namespace std;
/*
    SIZE开头，表示“XXX的大小”，单位：字节
    NUM开头，表示“XXX用了多少个 物理磁盘块”，单位：个
    START开头，表示“XXX的起始地址”，单位：字节
    LEN开头，表示“使用了多少扇区”，单位个
*/

#define NAME_DISK "filesys.disk"//文件名
#define byte unsigned char    //一个char代表一个字节

#define SIZE_OF_BLOCK 512   //每个扇区：512字节
#define NUM_OF_BLOCK 1024   //扇区的个数：1024个
#define SIZE_OF_DISK (SIZE_OF_BLOCK * NUM_OF_BLOCK)      //整个虚拟磁盘大小：扇区大小*扇区数 字节

#define LEN_OF_SUPER 3                      //超级块区域使用的扇区个数
#define SIZE_OF_SUPERBLOCK (SIZE_OF_BLOCK*3)  //超级块总长度
#define START_OF_SUPER (SIZE_OF_BLOCK*1)  //超级块起始地址：第1块扇区

#define SIZE_OF_DINODEBLOCK 64    //每个磁盘i节点大小：64字节
#define NUM_OF_DINODEBLOCK 512        //磁盘i节点块数：512个
#define LEN_OF_DINODE (SIZE_OF_DINODEBLOCK*NUM_OF_DINODEBLOCK/SIZE_OF_BLOCK) //i节点区使用扇区个数
#define SIZE_OF_DINODE (NUM_OF_DINODEBLOCK*SIZE_OF_DINODEBLOCK)    //磁盘i节点区大小：块大小*块数个，单位字节
#define START_OF_DINODE (START_OF_SUPER+SIZE_OF_SUPERBLOCK)   //i节点区起始地址：超级块起始地址+超级块长度，单位字节
#define NUM_OF_FREESTACK (SIZE_OF_BLOCK/sizeof(int) - 1) //成组连接中堆栈大小，单位个
#define BITMAP_ROW  32            //位示图行下标
#define BITMAP_COLUMN 16          //位示图行下标

#define SIZE_OF_DIRBLOCK 16     //一个DIR表项：16字节
#define NUM_OF_DIR  128         //一共有128个DIR表项
#define SIZE_OF_DIR (SIZE_OF_DIRBLOCK * NUM_OF_DIR)        //DIR区域字节长度
#define LEN_OF_DIR (SIZE_OF_DIR / SIZE_OF_BLOCK)        //DIR区域使用了多少个扇区。
#define START_OF_DIR (START_OF_DINODE+SIZE_OF_DINODE)//DIR区域起始位置

#define SIZE_OF_DATABLOCK SIZE_OF_BLOCK   //每个数据块：同物理扇区
#define NUM_OF_DATABLOCK (NUM_OF_BLOCK-1-SIZE_OF_SUPERBLOCK/SIZE_OF_BLOCK-(SIZE_OF_DINODE/SIZE_OF_BLOCK)-(SIZE_OF_DIR/SIZE_OF_BLOCK))
//磁盘数据块块数：总磁盘块-1个引导块-超级块所用块数量-i节点区块数-DIR区所用块数量
#define SIZE_OF_DATA (NUM_OF_DATABLOCK*SIZE_OF_DATABLOCK)    //数据区长度
#define START_OF_DATA (START_OF_DIR+SIZE_OF_DIR)//磁盘数据区起始地址：DIR区起始地址+DIR区长度
#define DIRSIZE 10        //目录名大小为10
#define BASIC_DIRNUM 128     //基本目录表包含的最大目录项数为128，即文件数
#define SUB_DIRNUM 32     //子目录表包含的最大目录项数为32，即文件数

void Format(byte *disk);//格式化磁盘
//void Format();//格式化磁盘
byte * Load_disk();//从实际硬盘加载虚拟磁盘文件，如果不存在则指针为空。
void Save_disk(byte *disk);//保存到硬盘中
byte* Alloc_inode(byte *disk);
void Free_inode(byte *disk, byte *block);
byte* Alloc_data_block(byte *disk);//分配空闲数据块
void Free_data_block(byte *disk, byte *block);//回收空闲数据块
byte* Alloc_inode(byte *disk);
void Free_inode(byte *disk, byte *block);

void CreatSubDir(string name_add);
void Display();
void CreatFile(string name);
void DeleteSubDir(string name);
void Cd_Dir(string name);
void CreatFile(string name);
void Deletefile(string name);
void Copyfile(string name);
void Openfile(string name);
void Savefile(string name);
void ReNameDir(string name);
void ReNameFile(string name);
void PasteFile();
void Show_disk_info();

struct Sblock
{
    unsigned short s_inode_size;                     // 磁盘i节点块数
    unsigned short s_inode_freesize;                    // 空闲磁盘i节点块数
    bool s_inode_bitmap[BITMAP_ROW][BITMAP_COLUMN];             // 512个dinode块
    unsigned int s_rinode;                      //  铭记索引节点
    unsigned int s_data_size;                   // 数据快快数
    unsigned int s_data_freesize;               //数据快空闲快数
    unsigned short s_pfree;                     // 空闲块指针
    unsigned int s_data_freestack[NUM_OF_FREESTACK+1];     // 空闲块堆栈
    unsigned int s_ninode;                      //空闲索引节点数
    unsigned int  s_data_usedblocknum;           //使用了磁盘数据块数量
    unsigned short s_pinode;                    //空闲索引节点指针
    unsigned char s_smod;                       //超级快修改标志，1修改，0未修改
};

//磁盘i节点
struct Dinode
{
    unsigned short di_number;  //      关联文件数，当==0时，删除该文件
    unsigned char  di_mode;   // 存取权限 ,0是不能向其操作，1是读，
    //2是写，3是创立者（只有3才能删除文件）
    unsigned short di_uid;     //      磁盘索引节点用户ID
    unsigned short di_gid;     //      磁盘索引节点用户组id
    unsigned short di_size;    //      关联文件大小（所占的块数）
    int di_addr[10];
    // unsigned short di_addr[10];  //   物理块号
};

/*目录项结构*/
struct Direct{
    char D_Name[8];  //名8
    unsigned short id;
    short type;       //0指向文件，1指向目录，2当前未使用
    int i_ino;
    // unsigned short i_ino;    //对应的磁盘i节点
};//16个字节
/*基本目录表*/



/*目录项结构*/

/*基本目录表*/
struct BasicDir{
    struct Direct basicdir[BASIC_DIRNUM];
    //int size;
};//2048个字节
/*子目录表*/

struct SubDir{
    struct Direct subdir[SUB_DIRNUM];
    //int size;
};//512个字节




#define PWDSIZ   12  //口令字
#define PWDNUM   32  //最多可设32个口令登录
#define NOFILE  20  //每个用户最多可打开20个文件，即用户打开文件表最大项数
#define NADDR 10  //每个i节点最多指向10块，addr[0]~addr[9]
#define NHINO  128  //共128个Hash链表，提供索引i节点
#define USERNUM  10  //最多允许10个用户登录
#define USERSIZ  8  //用户名长度限制
#define DINODESIZ  32 //每个磁盘i节点所占字节
#define DINODEBLK  32 // 所有磁盘i节点共占32个物理块
#define FILEBLK  512  //共有512个目录文件物理块
#define NICFREE  50  //超级块中空闲块数组的最大项数
#define NICINOD  50  //超级块中空闲i节点的最大项数
#define DINODESTART //2*BLOCKSIZ  i节点起始地址
#define DATASTART //(2+DINODEBLK)*BLOCKSIZ 目录、文件区起始地址

//用户结构
struct USERNAME{
    unsigned short u_mode;			//用户类别
    unsigned short u_uid;            // 用户ID
    unsigned short u_ofile[NOFILE]; //  用户打开文件表
};

//用户密码
struct PWD{
    unsigned short u_uid;    //用户id  2
    char  username[USERSIZ];	//用户名
    char password[PWDSIZ];   //密码
};


class User{
public:
    //变量
    USERNAME username[USERNUM];
    PWD pwd[PWDNUM];
    int nPwdNum = 0; //当前存在的密码数量
    //方法
    User();
    void fileRead();
    string inputPassword();
    bool login();
    int  signInCheck(string strUserName);
    bool addUser(string strUserName, string strPassword);
    void signup();
    void logout();
};


// 命令识别
int command_recognize(string command);


#endif //FILESYS_filesys_H