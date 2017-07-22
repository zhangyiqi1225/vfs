#include <type_traits>//初始化copyitem

#include <iostream>
#include <cstdio>
#include <cstring>
#include <rpc.h>
#include "filesys.h"

extern unsigned short Current_Uid;     //当前操作文件系统的用户
extern unsigned short Current_Gid;       //当前操作系统的用户level
extern char Current_Dir_Name[10];   //当前所在目录表
extern struct BasicDir Dir_Basic;  //基本目录表
extern struct SubDir Dir_Current;  //当前目录表
extern Direct Copy_Item;
extern byte *disk;

extern string Full_Dir;
using namespace std;

void CreatSubDir(string name) {
    int i, j, k;
    struct SubDir Dir_Add;
    char name_add[8];
    strcpy(name_add, name.c_str());
    struct Dinode Dinode_Add;
    for (i = 8; i < 128; i++) {       //检测基本目录表中有无空闲行可用
        if (Dir_Basic.basicdir[i].type == 2) break;
    }
    if (i == 128) {
        cout << "基本目录表满，无法创建子目录" << endl;
    }
    for (j = 0; j < 32; j++) {       //
        if (Dir_Current.subdir[j].type == 2) break;
    }
    if (j == 32) {
        cout << "此目录表已满，无法创建子目录" << endl;
    } else {
        for (k = 2; k < 32; k++) {
            if (!strcmp(name_add, Dir_Current.subdir[k].D_Name) && Dir_Current.subdir[k].type == 1) break;
        }
        if (k < 32) {
            cout << "此目录下有重名文件，创建失败" << endl;
        } else {
            byte *pi = Alloc_inode(disk);       //i节点空闲快分配
            byte *pd = Alloc_data_block(disk);       //对目录表空闲块进行分配
            Dir_Basic.basicdir[i].i_ino = pi - disk;   //修改基本目录表的信息，并保存
            Dir_Basic.basicdir[i].id = (unsigned short) (i + 1);
            Dir_Basic.basicdir[i].type = 1;
            strcpy(Dir_Basic.basicdir[i].D_Name, name_add);

            Dir_Current.subdir[j].i_ino = Dir_Basic.basicdir[i].id;  //修改当前目录表的信息，并保存
            Dir_Current.subdir[j].id = (unsigned short) (j + 1);
            Dir_Current.subdir[j].type = 1;
            strcpy(Dir_Current.subdir[j].D_Name, name_add);
            Dinode_Add.di_number = 1;       //创建新的目录表i节点信息
            Dinode_Add.di_mode = 3;
            Dinode_Add.di_uid = Current_Uid;
            Dinode_Add.di_gid = Current_Gid;
            Dinode_Add.di_size = 1;

            Dinode_Add.di_addr[0] = pd - disk;       //所占的物理块号
            for (int l = 1; l < 10; ++l) {
                Dinode_Add.di_addr[l] = 0;
            }
            strcpy(Dir_Add.subdir[0].D_Name, name_add);          //0号索引代表的本级目录的名字和ino号
            Dir_Add.subdir[0].i_ino = Dir_Basic.basicdir[i].id;     //在此目录下的ino号对应的bfd中的id号
            Dir_Add.subdir[0].id = 1;
            Dir_Add.subdir[0].type = 1;
            strcpy(Dir_Add.subdir[1].D_Name, Dir_Current.subdir[0].D_Name);       //1号索引代表上级目录的名字和ino号
            Dir_Add.subdir[1].i_ino = Dir_Current.subdir[0].i_ino;
            Dir_Add.subdir[1].id = 2;
            Dir_Add.subdir[1].type = 1;
            // 初始化

            for (int l = 2; l < 32; ++l) {
                strcpy(Dir_Add.subdir[l].D_Name, "     ");
                Dir_Add.subdir[l].i_ino = 0;
                Dir_Add.subdir[l].id = l + 1;
                Dir_Add.subdir[l].type = 2;
            }
            unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
            Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[c_flag].i_ino);
            SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
            memmove(it2, &Dir_Current, sizeof(SubDir));      //将Dir_Current的信息覆盖

            memmove(pd, &Dir_Add, sizeof(SubDir));       //将新的目录表存到数据块里
            memmove(pi, &Dinode_Add, sizeof(Dinode));       //将新的i节点存到数据块里
            //分配数据块，写入数据块
        }
    }
}

void DeleteSubDir(string name) {
    int i, flag, number;
    char name_del[10];
    strcmp(name_del,name.c_str());
    strcpy(name_del, name.c_str());
    struct SubDir Dir_Del;
    //cout << "请输入要删除的目录：" << endl;
    //scanf("%s", name_del);
    for (i = 2; i < 32; i++) {
        if (!strcmp(Dir_Current.subdir[i].D_Name, name_del)) break;
    }
    if (i == 32) {
        cout << "未找到要删除的目录" << endl;
    } else {
        if (Dir_Current.subdir[i].type == 0) cout << "未找到要删除的目录" << endl;
        else {       //进入找到的目录（判断是否有文件还未删除）
            flag = Dir_Current.subdir[i].i_ino - 1;    //flag为此目录表在基本目录表中的行数
            Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[flag].i_ino);       //首地址加偏移量指向i节点区域
            SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
            memmove(&Dir_Del, it2, sizeof(SubDir));       //将此目录的信息复制给Dir_Current
            for (number = 2; number < 32; number++) {
                if (Dir_Del.subdir[number].type != 2) break;
            }
            if (number < 32) {
                cout << "此目录下还有文件，无法删除" << endl;
            } else {
                Dir_Current.subdir[i].type = 2;
                strcpy(Dir_Current.subdir[i].D_Name, "     ");
                Dir_Current.subdir[i].i_ino = Dir_Current.subdir[0].i_ino;
                flag = Dir_Current.subdir[i].i_ino - 1;
                Dir_Basic.basicdir[flag].i_ino = 0;
                Dir_Basic.basicdir[flag].type = 2;
                strcpy(Dir_Basic.basicdir[flag].D_Name, "     ");
                Free_inode(disk, Dir_Basic.basicdir[flag].i_ino + disk);
                Free_data_block(disk, (((Dinode *) (Dir_Basic.basicdir[flag].i_ino + disk))->di_addr[0] + disk));
                unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
                Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[c_flag].i_ino);
                SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
                memmove(it2, &Dir_Current, sizeof(SubDir));      //将Dir_Current的信息覆盖
                cout << "子目录释放成功" << endl;
            }
        }
    }
}

void Cd_Dir(string name) {
    int i;
    char route[10];
    strcpy(route, name.c_str());
    int choice;
    int flag;
    if (name == "-") {
        if (!strcmp(Dir_Current.subdir[1].D_Name, "Basic")) {
            cout << "根目录，无法返回" << endl;
        } else {//返回上一级
            string temp = Dir_Current.subdir[0].D_Name;
            Full_Dir = Full_Dir.substr(0, Full_Dir.size() - (temp.size() + 1));

            flag = Dir_Current.subdir[1].i_ino - 1;
            // (disk + Dir_Basic.basicdir[flag].i_ino);//磁盘开头物理地址 + Dir_Basic.basicdir[i].i_ino,得到要找的i节点的实际物理地址，可以定位到具体的i节点所在内存区域
            Dinode *it1 = (Dinode *) (disk +
                                      Dir_Basic.basicdir[flag].i_ino);//磁盘开头物理地址 + Dir_Basic.basicdir[i].i_ino,得到要找的i节点的实际物理地址，可以定位到具体的i节点所在内存区域，再强制类型转换，表示该地址是Dinode类型
            SubDir *it2 = (SubDir *) (disk +
                                      ((it1->di_addr)[0]));//it1指向的内存物理地址的内容中，addr数组的0号单元。取其内容，再加上磁盘开头物理地址， 可得到当前目录的实际内存物理地址。
            // SubDir it2;
            // memmove(&it2,disk + (it1->di_addr[0]), sizeof(SubDir));
            memmove(&Dir_Current, it2, sizeof(SubDir));//把SubDir的实际物理地址（it2）指向的内存的内容拷贝到Dir_Current的地址中去，大小为一个SubDir的大小。
            strcpy(Current_Dir_Name, Dir_Current.subdir[0].D_Name);
        }


    } else {        //进入下一级
        for (i = 2; i < 32; i++) {
            if (!strcmp(Dir_Current.subdir[i].D_Name, route)) break;
        }
        if (i == 32) {
            cout << "该目录下未找到所要进入的子目录名" << endl;
        } else {
            if (Dir_Current.subdir[i].type == 0) {
                cout << "该目录下未找到所要进入的子目录名" << endl;
            } else {
                flag = Dir_Current.subdir[i].i_ino - 1;             //flag是此目录在基本目录表中的标号，行数-1
                Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[flag].i_ino);
                SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
                memmove(&Dir_Current, it2, sizeof(SubDir));       //将此目录的信息复制给Dir_Current
                strcpy(Current_Dir_Name, Dir_Current.subdir[0].D_Name);

                Full_Dir += "/";
                Full_Dir += Current_Dir_Name;
            }
        }
    }
}

void CreatFile(string name) {
    int i, j, k;
    char name_add[10];
    strcpy(name_add, name.c_str());
    char s[] = "Empty File";
    struct Dinode Dinode_Add;
    for (i = 8; i < 128; i++) {
        if (Dir_Basic.basicdir[i].type == 2) break;
    }
    if (i == 128) {
        cout << "基本目录表满，无法创建文件";
    } else {
        for (j = 2; j < 32; j++) {
            if (Dir_Current.subdir[j].type == 2) break;
        }
        if (j == 32) {
            cout << "当前目录表满，无法创建文件" << endl;
        } else {
            for (k = 0; k < 32; k++) {
                if (!strcmp(name_add, Dir_Current.subdir[k].D_Name) && Dir_Current.subdir[k].type == 0) break;
            }
            if (k < 32) {
                cout << "与当前目录下已存在文件重名，创建失败" << endl;
            } else {
                byte *pi = Alloc_inode(disk);       //i节点空闲快分配
                byte *pf = Alloc_data_block(disk);       //对创建的文件分配空闲数据块

                Dir_Basic.basicdir[i].id = (i + 1);   //对基本目录下目录项的文件信息更新
                Dir_Basic.basicdir[i].type = 0;
                strcpy(Dir_Basic.basicdir[i].D_Name, name_add);
                Dir_Basic.basicdir[i].i_ino = pi - disk;

                Dir_Current.subdir[j].i_ino = Dir_Basic.basicdir[i].id; //对当前目录下目录项的文件信息更新
                Dir_Current.subdir[j].id = (unsigned short) (j + 1);
                Dir_Current.subdir[j].type = 0;
                strcpy(Dir_Current.subdir[j].D_Name, name_add);

                Dinode_Add.di_size = 1;
                Dinode_Add.di_addr[0] = pf - disk;
                for (int l = 1; l < 10; ++l) {
                    Dinode_Add.di_addr[l] = 0;
                }
                Dinode_Add.di_gid = Current_Gid;
                Dinode_Add.di_uid = Current_Uid;
                Dinode_Add.di_number = 1;
                Dinode_Add.di_mode = 3;
                unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
                Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[c_flag].i_ino);
                SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
                memmove(it2, &Dir_Current, sizeof(SubDir));      //将Dir_Current的信息覆盖
                memmove(pf, s, sizeof(s));       //将新的文件存到数据块里
                memmove(pi, &Dinode_Add, sizeof(Dinode));       //将新的i节点存到数据块里
            }
        }
    }
}

void Deletefile(string name) {
    int i, flag;
    char del_name[10];
    strcpy(del_name, name.c_str());
    Dinode Del_Dinode;
    for (i = 2; i < 32; i++) {
        if (!strcmp(Dir_Current.subdir[i].D_Name, del_name)) break;
    }
    if (i == 32) {
        cout << "未找到要删除的文件" << endl;
    } else {
        if (Dir_Current.subdir[i].type == 1)
            cout << "未找到要删除的文件" << endl;
        else {
            Dir_Current.subdir[i].type = 2;
            strcpy(Dir_Current.subdir[i].D_Name, "     ");
            Dir_Current.subdir[i].i_ino = Dir_Current.subdir[0].i_ino;

            flag = Dir_Current.subdir[i].i_ino - 1;
            Dir_Basic.basicdir[flag].i_ino = 0;
            Dir_Basic.basicdir[flag].type = 2;
            strcpy(Dir_Basic.basicdir[flag].D_Name, "     ");

            Dinode *it = (Dinode *) disk + Dir_Basic.basicdir[flag].i_ino;
            memmove(&Del_Dinode, it, sizeof(Dinode));
            int number = (Del_Dinode.di_number)--;
            if (number < 0 || number == 0) {
                Free_inode(disk, Dir_Basic.basicdir[flag].i_ino + disk);      //若关联文件数为0，则删除该i节点
            }
            Free_data_block(disk,
                            ((Dinode *) (Dir_Basic.basicdir[flag].i_ino + disk))->di_addr[0] + disk);   //释放文件所占的数据块
            unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
            Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[c_flag].i_ino);
            SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
            memmove(it2, &Dir_Current, sizeof(SubDir));      //将Dir_Current的信息覆盖
            cout << "文件释放成功" << endl;
        }
    }
}

void Copyfile(string name) {
    char name_copy[10];
    strcpy(name_copy, name.c_str());
    int i;
    for (i = 2; i < 32; i++) {
        if (strcmp(Dir_Current.subdir[i].D_Name, name_copy) == 0 && Dir_Current.subdir[i].type == 0) break;
    }
    if (i == 32) {
        cout << "当前目录下未找到要复制的文件" << endl;
    } else {
        Copy_Item.i_ino = Dir_Current.subdir[i].i_ino;
        Copy_Item.type = Dir_Current.subdir[i].type;
        strcpy(Copy_Item.D_Name, Dir_Current.subdir[i].D_Name);
        cout << "文件复制成功" << endl;
    }
}

void PasteFile() {
    int i, j;
    if (Copy_Item.type != 0) {
        cout << "当前未对文件进行复制" << endl;
    } else {
        for (i = 2; i < 32; i++) {
            if (Dir_Current.subdir[i].type == 2) break;
        }
        if (i == 32) {
            cout << "当前目录满，无法进行粘贴" << endl;
        } else {
            for (j = 2; j < 32; j++) {
                if (strcmp(Dir_Current.subdir[j].D_Name, Copy_Item.D_Name) == 0 &&
                    Dir_Current.subdir[j].type == 0)
                    break;
            }
            if (j < 32) {
                cout << "此目录下有重名文件，不能复制" << endl;
            } else {
                Dir_Current.subdir[i].i_ino = Copy_Item.i_ino;
                Dir_Current.subdir[i].type = 0;
                Dir_Current.subdir[i].id = (unsigned short) (i + 1);
                strcpy(Dir_Current.subdir[i].D_Name, Copy_Item.D_Name);
                unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
                Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[c_flag].i_ino);
                SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
                memmove(it2, &Dir_Current, sizeof(SubDir));      //将Dir_Current的信息覆盖
                cout << "文件粘贴成功" << endl;
            }
        }
    }
}

void ReNameFile(string name){
    char name_file[10];
    strcpy(name_file,name.c_str());
    string new_name;
    int i;
    for (i = 2; i < 32; i++)
    {
        if (strcmp(Dir_Current.subdir[i].D_Name, name_file) == 0 && Dir_Current.subdir[i].type == 0) break;
    }
    if (i == 32)
    {
        cout << "当前目录下未找到要重命名的文件" << endl;
    } else
    {
        cout<<"input the new name:";
        cin>>new_name;
        strcpy(Dir_Current.subdir[i].D_Name,new_name.c_str());
        unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
        Dinode * it1 = (Dinode *)(disk + Dir_Basic.basicdir[c_flag].i_ino);
        SubDir * it2 = (SubDir *)(disk + (it1->di_addr[0]));
        memmove(it2,&Dir_Current,sizeof(SubDir));      //将Dir_Current的信息覆盖
        cout << "文件重命名成功" << endl;
    }
}
void ReNameDir(string name){
    char name_file[10];
    strcpy(name_file,name.c_str());
    string new_name;
    int i;
    for (i = 2; i < 32; i++)
    {
        if (strcmp(Dir_Current.subdir[i].D_Name, name_file) == 0 && Dir_Current.subdir[i].type == 1) break;
    }
    if (i == 32)
    {
        cout << "当前目录下未找到要重命名的目录" << endl;
    } else
    {
        cout<<"input the new name:";
        cin>>new_name;

        strcpy(Dir_Current.subdir[i].D_Name,new_name.c_str());
        unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
        Dinode * it1 = (Dinode *)(disk + Dir_Basic.basicdir[c_flag].i_ino);
        SubDir * it2 = (SubDir *)(disk + (it1->di_addr[0]));
        memmove(it2,&Dir_Current,sizeof(SubDir));      //将Dir_Current的信息覆盖
        Cd_Dir(new_name);

        strcpy(Dir_Current.subdir[0].D_Name,new_name.c_str());
        unsigned short n_flag = Dir_Current.subdir[0].i_ino - 1;
        Dinode * it3 = (Dinode *)(disk + Dir_Basic.basicdir[n_flag].i_ino);
        SubDir * it4 = (SubDir *)(disk + (it3->di_addr[0]));
        memmove(it4,&Dir_Current,sizeof(SubDir));       //将更改目录名后的子目录的第零项名字修改。
        Cd_Dir("-");
        cout << "文件重命名成功" << endl;
    }
}


void Sign() {
    int i, k;
    char username[10];
    struct SubDir Dir_Add;
    struct Dinode Dinode_Add;
    for (i = 0; i < 8; i++) {       //检测基本目录表的用户区有无可用
        if (Dir_Basic.basicdir[i].type == 2) break;
    }
    if (i == 8) {
        cout << "用户数量满，无法注册新的用户" << endl;
    } else {
        cout << "请输入要用户名（不超过10位）";
        scanf("%s", username);
        for (k = 0; k < 8; k++) {
            if (!strcmp(username, Dir_Current.subdir[k].D_Name) && Dir_Current.subdir[k].type == 1) break;
        }
        if (k < 8) {
            cout << "注册用户有重名，创建失败" << endl;
        } else {
            byte *pi = Alloc_inode(disk);       //i节点空闲快分配
            byte *pd = Alloc_data_block(disk);       //对目录表空闲块进行分配
            Dir_Basic.basicdir[i].i_ino = (unsigned short) (pi - disk);   //修改基本目录表的信息，并保存
            Dir_Basic.basicdir[i].id = (unsigned short) (i + 1);
            Dir_Basic.basicdir[i].type = 1;
            strcpy(Dir_Basic.basicdir[i].D_Name, username);

            Dinode_Add.di_number = 1;       //创建新的目录表i节点信息
            Dinode_Add.di_mode = 3;
            Dinode_Add.di_uid = (unsigned short) (i + 1);
            Dinode_Add.di_gid = (unsigned short) (i + 1);
            Dinode_Add.di_size = 1;

            Dinode_Add.di_addr[0] = (pd - disk);       //所占的物理块号
            strcpy(Dir_Add.subdir[0].D_Name, username);          //0号索引代表的本级目录的名字和ino号
            Dir_Add.subdir[0].i_ino = Dir_Basic.basicdir[i].id;     //在此目录下的ino号对应的bfd中的id号
            Dir_Add.subdir[0].id = 1;
            Dir_Add.subdir[0].type = 1;
            strcpy(Dir_Add.subdir[1].D_Name, "BasicDir");       //1号索引代表上级目录的名字和ino号
            Dir_Add.subdir[1].i_ino = (i + 1);
            Dir_Add.subdir[1].id = 2;
            Dir_Add.subdir[1].type = 1;

            memmove(pd, &Dir_Add, sizeof(SubDir));       //将新的用户目录存到数据块里
            memmove(pi, &Dinode_Add, sizeof(Dinode));       //将新的i节点存到数据块里
            //分配数据块，写入数据块
        }
    }
}

void Display() {
    int i, j;
    //cout<<"当前目录："<<Dir_Current.subdir[0].D_Name<<endl;
    //cout << "current dir is " << Dir_Current.subdir[0].D_Name << endl;
    //cout << Full_Dir << ":" << endl;
    //cout<<"当前目录下的文件："<<endl;
    //cout << "current file is: ";

    for (i = 2; i < 32; i++) {
        if (Dir_Current.subdir[i].type == 0)
            cout << Dir_Current.subdir[i].D_Name << "   ";
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
    for (j = 2; j < 32; j++) {
        if (Dir_Current.subdir[j].type == 1)
            cout << Dir_Current.subdir[j].D_Name << "   ";
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
    cout << endl;
}