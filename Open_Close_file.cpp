//
// Created by Earl on 2017/7/14.
//

#include <processthreadsapi.h>
#include <sstream>
#include <cstdio>
#include <rpc.h>
#include "filesys.h"
using namespace std;

extern byte* disk;
extern SubDir Dir_Current;
extern BasicDir Dir_Basic;
void Openfile(string name)//将文件打开，并且将其内容读取到一个string内。
{
    int k;
    for (k = 2; k < 32; ++k)
    {
        string temp = Dir_Current.subdir[k].D_Name;
        if(temp == name)
            break;
    }
    if(k == 32)
        cout <<"No such a file." <<endl;
    else
    {
        int flag = Dir_Current.subdir[k].i_ino - 1;             //flag是此目录在基本目录表中的标号，行数-1
        Dinode *file = (Dinode *) (disk + Dir_Basic.basicdir[flag].i_ino);
        string result = "";
        for (int i = 0; i < 10; ++i)//从数组第0号单元开始找
        {
            if(file->di_addr[i] != 0)//如果某个单元内容存的不是0，说明存的是偏移地址，读取。
            {
                byte *it = disk + file->di_addr[i];//定位到使用的数据块。
                for (int j = 0; j < SIZE_OF_BLOCK; j++)
                {
                    if(*it == 0)//由于在初始化时初始值全为字符型的0（'\000'），该值用户不会输入，所以用来判断数据块是否读到尽头
                        break;
                    result.push_back(*it);//指针取到的字节内容放入结果。
                    it ++ ;
                }
            }
        }
//        stringstream ss ;
//        ss << (byte *)file - disk;
//        char name[20];
//        ss>> name;
//        string name_str = name;
        FILE *fp =fopen("temp","w+");
        for(char &t:result)
            fwrite(&t,1,1,fp);
        fclose(fp);
        //使用记事本作为文本编辑框-
        WinExec( "notepad.exe temp", SW_SHOW );
        /*STARTUPINFO StartInfo={0};
        PROCESS_INFORMATION ProcessInfo={0};
        CreateProcess("c:\\windows\\system32\\notepad.exe",NULL,NULL,NULL,FALSE,0,NULL,NULL,&StartInfo,&ProcessInfo);*/
    }
}

void Savefile(string name)
{
    FILE *fp =fopen("temp","r");
    char temp[SIZE_OF_BLOCK+1];
    for (int j = 0; j < SIZE_OF_BLOCK+1; ++j)
        temp [j] = '\0';
    int k;
    for (k = 2; k < 32; ++k)
    {
        string temp1 = Dir_Current.subdir[k].D_Name;
        if(temp1 == name)
            break;
    }
    if(k == 32)
        cout <<"No such a file." <<endl;
    else
    {
        int flag = Dir_Current.subdir[k].i_ino - 1;             //flag是此目录在基本目录表中的标号，行数-1
        Dinode *file = (Dinode *) (disk + Dir_Basic.basicdir[flag].i_ino);
        fseek(fp,0,SEEK_END);
        int length=ftell(fp);
        fseek(fp,0,SEEK_SET);
        for (int i = 0; i <= length / SIZE_OF_BLOCK; ++i)
        {
            fread(temp,1,SIZE_OF_BLOCK,fp);
            if(file->di_addr[i] == 0 )
                file->di_addr[i] = (int)(Alloc_data_block(disk) - disk);
            memmove(disk + file->di_addr[i] ,temp,SIZE_OF_BLOCK);
        }
    }
    fclose(fp);
    remove("temp");
}

