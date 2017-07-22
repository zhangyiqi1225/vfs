#include <iostream>
#include <conio.h>
#include <cstring>
#include "filesys.h"

using namespace std;
unsigned short Current_Uid;     //当前操作文件系统的用户
unsigned short Current_Gid;       //当前操作系统的用户level
char Current_Dir_Name[10];   //当前所在目录表
struct BasicDir Dir_Basic;  //基本目录表
struct SubDir Dir_Current;  //当前目录表
Direct Copy_Item;
extern string Full_Dir;
byte *disk;
extern byte *disk_it;//内存虚拟磁盘起始物理地址
int main() {
    disk = Load_disk();
    // cout << sizeof(BasicDir) << endl;
    BasicDir *it = (BasicDir *) (disk + START_OF_DIR);
    memmove(&Dir_Basic, it, sizeof(BasicDir));
    cout << "if you want to signup? yse(y) or no(n)" << endl;
    char c;
    cin >> c;
    if( c == 'y'){
        User userSignup;
        userSignup.signup();
    }
    // disk = Load_disk();
    User userLogin;
    t:bool loginResult = userLogin.login();
    //cout << loginResult << endl;
    if (loginResult == true) {
        cout << "login success,welcome " << endl;
        // choose if format

        cin.sync(); // 清cin缓冲区
        //please  input your command
        cout << Full_Dir << ":";
        string command;
        getline(cin, command);
        while (command != "exit") {
            command_recognize(command);
            cout << Full_Dir << ":";
            cin.sync();
            getline(cin, command);
        }
        memmove(it,&Dir_Basic,sizeof(BasicDir));
        Save_disk(disk);
    } else {
        cout << "login error,please check your username and password" << endl;
        goto t;
    }
        return 0;
}