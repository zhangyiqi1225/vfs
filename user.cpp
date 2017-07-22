//
// Created by xujingguo on 2017/7/6.
//

#include "filesys.h"
#include <fstream>
#include <conio.h>
#include <vector>
#include <algorithm>
#include <cstring>

extern unsigned short Current_Uid;     //当前操作文件系统的用户
extern unsigned short Current_Gid;       //当前操作系统的用户level
extern char Current_Dir_Name[8];   //当前所在目录表
extern struct BasicDir Dir_Basic;  //基本目录表
extern struct SubDir Dir_Current;  //当前目录表
extern Direct Copy_Item;
extern byte *disk;

string Full_Dir;
using namespace std;


User::User()
{
    FILE *fp;
    int i = 0;
    if (fp = fopen("password.txt", "r"))
    {
        int byte_temp = 0;
        while (fscanf(fp, "%s %s %d\n", &pwd[i].username, &pwd[i].password, &byte_temp) != EOF)
        {
            pwd[i].u_uid = (byte) (byte_temp);
            //cout <<pwd[i].username << pwd[i].password << pwd[i].u_uid << endl;
            i++;
            nPwdNum++;
        }
    } else
    {
        cout << "error";
    }
    fclose(fp);
}

/**
 * [User::inputPassword 输入密码 以*显示]
 * @return [返回输入的字符串]
 */
string User::inputPassword()
{
    char c;  //输入字符
    int nCount = 0;   //输入字符计数
    string strPassword;  //要返回的即结果
    while ((c = getch()) != '\r')
    {

        if (c == 8)
        { // 退格
            if (nCount == 0)
            {
                continue;
            }
            putchar('\b'); // 回退一格
            putchar(' '); // 输出一个空格将原来的*隐藏
            putchar('\b'); // 再回退一格等待输入
            nCount--;
        }
        if (nCount == PWDSIZ - 1)
        { // 最大长度为size-1
            break;
        }
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        {  // 密码只可包含数字和字母
            putchar('*');  // 接收到一个字符后, 打印一个*
            strPassword += c;
            nCount++;
        }
    }
    return strPassword;
}

/**
 * [User::login 登录函数]
 * @param  strUserName [用户输入的用户名]
 * @param  strPassword [用户输入的密码]
 * @return             [正确返回TRUE]
 */
bool User::login()
{
    string strUserName, strPassword;
    cout << "welcome get in our virtual fileSystem, please signin first" << endl;
    cout << "(login)please input your username:";
    cin >> strUserName;
    cout << "(login)please input your password:";
    strPassword = User().inputPassword();
    // strUserName = "root";
    // strPassword = "root";
    cout << endl;
    //fileRead();             //从文件中读取
    for (int i = 0; i < PWDNUM && i < nPwdNum; i++)
    {
        string tmp_username = pwd[i].username;
        string tmp_password = pwd[i].password;
        //cout << tmp_username << tmp_password << endl;
        if (strUserName == tmp_username && strPassword == tmp_password)
        {
            unsigned short u_uid = pwd[i].u_uid;
            unsigned short flag = (unsigned short) (u_uid - 1);
            Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[flag].i_ino);
            SubDir *it2 = (SubDir *) (disk + it1->di_addr[0]);
            //SubDir * it2 = (SubDir *)(disk + (it1->di_addr[0]));
            memmove(&Dir_Current, it2, sizeof(SubDir));       //将此目录的信息复制给Dir_Current
            strcpy(Current_Dir_Name, Dir_Current.subdir[0].D_Name);
            //cout << "now your id is " << Dir_Current.subdir[0].D_Name << endl;

            Full_Dir += Dir_Current.subdir[0].D_Name;
            return true;
        }
    }
    //cout << "error";
    return false;
}

/**
 * [User::signInCheck 用户名注册检查]
 * @param  strUserName [用户输入要注册的用户名]
 * @return             [description]
 */
int User::signInCheck(string strUserName)
{
    if (nPwdNum >= PWDNUM)
    {   //密码数量大于32
        return 0;
    }
    vector<string> vUsername; //保存所有用户名
    for (int i = 0; i < PWDNUM && i < nPwdNum; ++i)
    {
        string tmp_username = pwd[i].username;
        //cout << tmp_username << " ";
        vUsername.push_back(tmp_username);
    }
    cout << endl;
    sort(vUsername.begin(), vUsername.end());  //去重，计算用户数量
    vUsername.erase(unique(vUsername.begin(), vUsername.end()), vUsername.end());
    for (int j = 0; j < vUsername.size(); ++j)
    {
        //cout << vUsername[j];
        if (strUserName == vUsername[j])
        {  //说明已经存在该用户
            return -2;
        }
    }
    if (vUsername.size() >= USERNUM)
    {  //用户数量大于8
        return -1;
    }
    return 1;
}

/**
 * [User::addUser 增加一个用户]
 * @param  strUserName [用户名]
 * @param  strPassword [密码]
 * @return             [返回是否成功]
 */
bool User::addUser(string strUserName, string strPassword)
{
    FILE *fp;
    const char *bufferUsername, *bufferPassword;
    //cout << "the username's length is "<< strUserName.length() << endl;
    bufferUsername = strUserName.c_str();
    bufferPassword = strPassword.c_str();
    cout << "the buffer is " << bufferPassword << endl;
    if (fp = fopen("password.txt", "a+"))
    {
        fprintf(fp, "%s %s \n", bufferUsername, bufferPassword);
        // 此处是增加用户目录

        return true;
    } else
        return false;
    fclose(fp);
}

void User::signup()
{
    int i, j, k, p;
    PWD add_user;
    //char username[10];
    struct SubDir Dir_Add;
    struct Dinode Dinode_Add;
    for (i = 0; i < 8; i++)
    {       //检测基本目录表的用户区有无可用
        if (Dir_Basic.basicdir[i].type == 2) break;
    }
    if (i == 8)
    {
        cout << "the number of user is full " << endl;
        //cout<<"用户数量满，无法注册新的用户"<<endl;
    } else
    {
        cout << "before you signin, you should signup first" << endl;
        cout << "(signup)please input your username you want to signup(not longer than 10):";
        cin >> add_user.username;
        cout << "(signup)please input your password:";
        //cin >> add_user.password;
        strcpy(add_user.password , User().inputPassword().c_str());
        cout << endl;
        //strcpy(add_user.username, "root");
        //strcpy(add_user.password, "root");
        //scanf("%s",&username);
        for (k = 0; k < 8; k++)
        {
            if (!strcmp(add_user.username, Dir_Basic.basicdir[k].D_Name) && Dir_Basic.basicdir[k].type == 1) break;
        }
        if (k < 8)
        {
            cout << "the username already exited,error" << endl;
            //cout<<"注册用户有重名，创建失败"<<endl;
        } else
        {
            cout << "successful" << endl;
            byte *pi = Alloc_inode(disk);       //i节点空闲快分配
            byte *pd = Alloc_data_block(disk);       //对目录表空闲块进行分配
            Dir_Basic.basicdir[i].i_ino = (int) (pi - disk);   //修改基本目录表的信息，并保存
            Dir_Basic.basicdir[i].id = (unsigned short) (i + 1);
            Dir_Basic.basicdir[i].type = 1;
            strcpy(Dir_Basic.basicdir[i].D_Name, add_user.username);
            Dinode_Add.di_number = 1;       //创建新的目录表i节点信息
            Dinode_Add.di_mode = 3;
            Dinode_Add.di_uid = (unsigned short) (i + 1);
            Dinode_Add.di_gid = (unsigned short) (i + 1);
            Dinode_Add.di_size = 1;

            Dinode_Add.di_addr[0] = pd - disk;       //所占的物理块号
            strcpy(Dir_Add.subdir[0].D_Name, add_user.username);          //0号索引代表的本级目录的名字和ino号
            Dir_Add.subdir[0].i_ino = Dir_Basic.basicdir[i].id;     //在此目录下的ino号对应的bfd中的id号
            Dir_Add.subdir[0].id = 1;
            Dir_Add.subdir[0].type = 1;
            strcpy(Dir_Add.subdir[1].D_Name, "Basic");       //1号索引代表上级目录的名字和ino号
            Dir_Add.subdir[1].i_ino = 0;
            Dir_Add.subdir[1].id = 2;
            Dir_Add.subdir[1].type = 1;
            for (int l = 2; l < 32; ++l)
            {
                strcpy(Dir_Add.subdir[l].D_Name, "     ");
                Dir_Add.subdir[l].i_ino = 0;
                Dir_Add.subdir[l].id = l + 1;
                Dir_Add.subdir[l].type = 2;
            }
            add_user.u_uid = Dir_Basic.basicdir[i].id;
            memmove(pd, &Dir_Add, sizeof(SubDir));       //将新的目录表存到数据块里
            memmove(pi, &Dinode_Add, sizeof(Dinode));       //将新的i节点存到数据块里

            //分配数据块，写入数据块

            // 写入文件
            FILE *fp;
            if (fp = fopen("password.txt", "a+"))
            {
                int int_temp = (int) (add_user.u_uid);
                fprintf(fp, "%s %s %d\n", add_user.username, add_user.password, int_temp);
                fclose(fp);
            }
            //Save_disk(disk);
        }
    }
}

void User::logout(){
    Full_Dir = "";
    User().login();
}