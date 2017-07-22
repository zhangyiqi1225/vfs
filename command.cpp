//
// Created by xujingguo on 2017/7/12.
//
#include <iostream>
#include "filesys.h"
extern byte * disk;

using namespace std;
int command_recognize(string command){
    string op = "", src = "";
    for (int i = 0; i < command.length(); ++i) {
        if (command[i] == ' '){
            for (int j = i+1; j < command.length(); ++j) {
                src += command[j];
            }
            int index = 0;
                if( !src.empty())
                {
                    while( (index = src.find(' ',index)) != string::npos)
                    {
                        src.erase(index,1);
                    }
                }
            break;
        }
        op += command[i];
    }
    if(op == "mkdir" && src != ""){
        CreatSubDir(src);
    } else if(op == "rm" && src != ""){
        Deletefile(src);
    } else if(op == "rmsub" && src != ""){  //É¾³ýÄ¿Â¼
        DeleteSubDir(src);
    } else if(op == "cd" && src != ""){
        Cd_Dir(src);
    } else if(op == "ls"){
        Display();
    } else if(op == "touch" && src != ""){
        CreatFile(src);
    } else if(op == "cp" && src != ""){
        Copyfile(src);
    } else if(op == "pas"){
        PasteFile();
    } else if(op == "open" && src != ""){
        Openfile(src);
    } else if(op == "save" && src != ""){
        Savefile(src);
    }else if(op == "renamef"){
        ReNameFile(src);
    }
    else if(op == "renamed"){
        ReNameDir(src);
    }
    else if(op == "out"){
        User userLogout;
        userLogout.logout();
    } else if(op == "reg"){
        User reg;
        reg.signup();
    } else if(op == "show"){
        Show_disk_info();
    }else if(op == ""){

    }else if(op == "help"){
        cout << "mkdir  [src]  //create new dir"<< endl;
        cout << "cd  [src]|-  //change current dir,src means child dir,or  '-' means super dir "<< endl;
        cout << "ls  //show current dir message"<< endl;
        cout << "touch  [src]  //create new file"<< endl;
        cout << "cp  [src]  //copy file"<< endl;
        cout << "pas        //paste current file you have copyed" << endl;
        cout << "open [src] //open file" << endl;
        cout << "save [src] //save file" << endl;
        cout << "reg        //signup" << endl;
        cout << "out        // logout" << endl;
        cout << "rm  [src]  //delete file"<< endl;
        cout << "rmsub [src] //delete dir" << endl;
        cout << "show        //show system message" <<endl;
        cout << "exit        //exut system and save" << endl;
        cout << "help      //show help" << endl;
    } else {
        cout << "can't recognize your command, please check your command" << endl;
    }
}