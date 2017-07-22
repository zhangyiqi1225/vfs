#ifndef FILESYS_filesys_H
#define FILESYS_filesys_H
/*
 * 2017.07.05��Designed by ������
 * */


#include <iostream>
#include <fstream>
#include <malloc.h>

using namespace std;
/*
    SIZE��ͷ����ʾ��XXX�Ĵ�С������λ���ֽ�
    NUM��ͷ����ʾ��XXX���˶��ٸ� ������̿顱����λ����
    START��ͷ����ʾ��XXX����ʼ��ַ������λ���ֽ�
    LEN��ͷ����ʾ��ʹ���˶�������������λ��
*/

#define NAME_DISK "filesys.disk"//�ļ���
#define byte unsigned char    //һ��char����һ���ֽ�

#define SIZE_OF_BLOCK 512   //ÿ��������512�ֽ�
#define NUM_OF_BLOCK 1024   //�����ĸ�����1024��
#define SIZE_OF_DISK (SIZE_OF_BLOCK * NUM_OF_BLOCK)      //����������̴�С��������С*������ �ֽ�

#define LEN_OF_SUPER 3                      //����������ʹ�õ���������
#define SIZE_OF_SUPERBLOCK (SIZE_OF_BLOCK*3)  //�������ܳ���
#define START_OF_SUPER (SIZE_OF_BLOCK*1)  //��������ʼ��ַ����1������

#define SIZE_OF_DINODEBLOCK 64    //ÿ������i�ڵ��С��64�ֽ�
#define NUM_OF_DINODEBLOCK 512        //����i�ڵ������512��
#define LEN_OF_DINODE (SIZE_OF_DINODEBLOCK*NUM_OF_DINODEBLOCK/SIZE_OF_BLOCK) //i�ڵ���ʹ����������
#define SIZE_OF_DINODE (NUM_OF_DINODEBLOCK*SIZE_OF_DINODEBLOCK)    //����i�ڵ�����С�����С*����������λ�ֽ�
#define START_OF_DINODE (START_OF_SUPER+SIZE_OF_SUPERBLOCK)   //i�ڵ�����ʼ��ַ����������ʼ��ַ+�����鳤�ȣ���λ�ֽ�
#define NUM_OF_FREESTACK (SIZE_OF_BLOCK/sizeof(int) - 1) //���������ж�ջ��С����λ��
#define BITMAP_ROW  32            //λʾͼ���±�
#define BITMAP_COLUMN 16          //λʾͼ���±�

#define SIZE_OF_DIRBLOCK 16     //һ��DIR���16�ֽ�
#define NUM_OF_DIR  128         //һ����128��DIR����
#define SIZE_OF_DIR (SIZE_OF_DIRBLOCK * NUM_OF_DIR)        //DIR�����ֽڳ���
#define LEN_OF_DIR (SIZE_OF_DIR / SIZE_OF_BLOCK)        //DIR����ʹ���˶��ٸ�������
#define START_OF_DIR (START_OF_DINODE+SIZE_OF_DINODE)//DIR������ʼλ��

#define SIZE_OF_DATABLOCK SIZE_OF_BLOCK   //ÿ�����ݿ飺ͬ��������
#define NUM_OF_DATABLOCK (NUM_OF_BLOCK-1-SIZE_OF_SUPERBLOCK/SIZE_OF_BLOCK-(SIZE_OF_DINODE/SIZE_OF_BLOCK)-(SIZE_OF_DIR/SIZE_OF_BLOCK))
//�������ݿ�������ܴ��̿�-1��������-���������ÿ�����-i�ڵ�������-DIR�����ÿ�����
#define SIZE_OF_DATA (NUM_OF_DATABLOCK*SIZE_OF_DATABLOCK)    //����������
#define START_OF_DATA (START_OF_DIR+SIZE_OF_DIR)//������������ʼ��ַ��DIR����ʼ��ַ+DIR������
#define DIRSIZE 10        //Ŀ¼����СΪ10
#define BASIC_DIRNUM 128     //����Ŀ¼����������Ŀ¼����Ϊ128�����ļ���
#define SUB_DIRNUM 32     //��Ŀ¼����������Ŀ¼����Ϊ32�����ļ���

void Format(byte *disk);//��ʽ������
//void Format();//��ʽ������
byte * Load_disk();//��ʵ��Ӳ�̼�����������ļ��������������ָ��Ϊ�ա�
void Save_disk(byte *disk);//���浽Ӳ����
byte* Alloc_inode(byte *disk);
void Free_inode(byte *disk, byte *block);
byte* Alloc_data_block(byte *disk);//����������ݿ�
void Free_data_block(byte *disk, byte *block);//���տ������ݿ�
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
    unsigned short s_inode_size;                     // ����i�ڵ����
    unsigned short s_inode_freesize;                    // ���д���i�ڵ����
    bool s_inode_bitmap[BITMAP_ROW][BITMAP_COLUMN];             // 512��dinode��
    unsigned int s_rinode;                      //  ���������ڵ�
    unsigned int s_data_size;                   // ���ݿ����
    unsigned int s_data_freesize;               //���ݿ���п���
    unsigned short s_pfree;                     // ���п�ָ��
    unsigned int s_data_freestack[NUM_OF_FREESTACK+1];     // ���п��ջ
    unsigned int s_ninode;                      //���������ڵ���
    unsigned int  s_data_usedblocknum;           //ʹ���˴������ݿ�����
    unsigned short s_pinode;                    //���������ڵ�ָ��
    unsigned char s_smod;                       //�������޸ı�־��1�޸ģ�0δ�޸�
};

//����i�ڵ�
struct Dinode
{
    unsigned short di_number;  //      �����ļ�������==0ʱ��ɾ�����ļ�
    unsigned char  di_mode;   // ��ȡȨ�� ,0�ǲ������������1�Ƕ���
    //2��д��3�Ǵ����ߣ�ֻ��3����ɾ���ļ���
    unsigned short di_uid;     //      ���������ڵ��û�ID
    unsigned short di_gid;     //      ���������ڵ��û���id
    unsigned short di_size;    //      �����ļ���С����ռ�Ŀ�����
    int di_addr[10];
    // unsigned short di_addr[10];  //   ������
};

/*Ŀ¼��ṹ*/
struct Direct{
    char D_Name[8];  //��8
    unsigned short id;
    short type;       //0ָ���ļ���1ָ��Ŀ¼��2��ǰδʹ��
    int i_ino;
    // unsigned short i_ino;    //��Ӧ�Ĵ���i�ڵ�
};//16���ֽ�
/*����Ŀ¼��*/



/*Ŀ¼��ṹ*/

/*����Ŀ¼��*/
struct BasicDir{
    struct Direct basicdir[BASIC_DIRNUM];
    //int size;
};//2048���ֽ�
/*��Ŀ¼��*/

struct SubDir{
    struct Direct subdir[SUB_DIRNUM];
    //int size;
};//512���ֽ�




#define PWDSIZ   12  //������
#define PWDNUM   32  //������32�������¼
#define NOFILE  20  //ÿ���û����ɴ�20���ļ������û����ļ����������
#define NADDR 10  //ÿ��i�ڵ����ָ��10�飬addr[0]~addr[9]
#define NHINO  128  //��128��Hash�����ṩ����i�ڵ�
#define USERNUM  10  //�������10���û���¼
#define USERSIZ  8  //�û�����������
#define DINODESIZ  32 //ÿ������i�ڵ���ռ�ֽ�
#define DINODEBLK  32 // ���д���i�ڵ㹲ռ32�������
#define FILEBLK  512  //����512��Ŀ¼�ļ������
#define NICFREE  50  //�������п��п�������������
#define NICINOD  50  //�������п���i�ڵ���������
#define DINODESTART //2*BLOCKSIZ  i�ڵ���ʼ��ַ
#define DATASTART //(2+DINODEBLK)*BLOCKSIZ Ŀ¼���ļ�����ʼ��ַ

//�û��ṹ
struct USERNAME{
    unsigned short u_mode;			//�û����
    unsigned short u_uid;            // �û�ID
    unsigned short u_ofile[NOFILE]; //  �û����ļ���
};

//�û�����
struct PWD{
    unsigned short u_uid;    //�û�id  2
    char  username[USERSIZ];	//�û���
    char password[PWDSIZ];   //����
};


class User{
public:
    //����
    USERNAME username[USERNUM];
    PWD pwd[PWDNUM];
    int nPwdNum = 0; //��ǰ���ڵ���������
    //����
    User();
    void fileRead();
    string inputPassword();
    bool login();
    int  signInCheck(string strUserName);
    bool addUser(string strUserName, string strPassword);
    void signup();
    void logout();
};


// ����ʶ��
int command_recognize(string command);


#endif //FILESYS_filesys_H