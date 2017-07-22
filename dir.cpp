#include <type_traits>//��ʼ��copyitem

#include <iostream>
#include <cstdio>
#include <cstring>
#include <rpc.h>
#include "filesys.h"

extern unsigned short Current_Uid;     //��ǰ�����ļ�ϵͳ���û�
extern unsigned short Current_Gid;       //��ǰ����ϵͳ���û�level
extern char Current_Dir_Name[10];   //��ǰ����Ŀ¼��
extern struct BasicDir Dir_Basic;  //����Ŀ¼��
extern struct SubDir Dir_Current;  //��ǰĿ¼��
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
    for (i = 8; i < 128; i++) {       //������Ŀ¼�������޿����п���
        if (Dir_Basic.basicdir[i].type == 2) break;
    }
    if (i == 128) {
        cout << "����Ŀ¼�������޷�������Ŀ¼" << endl;
    }
    for (j = 0; j < 32; j++) {       //
        if (Dir_Current.subdir[j].type == 2) break;
    }
    if (j == 32) {
        cout << "��Ŀ¼���������޷�������Ŀ¼" << endl;
    } else {
        for (k = 2; k < 32; k++) {
            if (!strcmp(name_add, Dir_Current.subdir[k].D_Name) && Dir_Current.subdir[k].type == 1) break;
        }
        if (k < 32) {
            cout << "��Ŀ¼���������ļ�������ʧ��" << endl;
        } else {
            byte *pi = Alloc_inode(disk);       //i�ڵ���п����
            byte *pd = Alloc_data_block(disk);       //��Ŀ¼����п���з���
            Dir_Basic.basicdir[i].i_ino = pi - disk;   //�޸Ļ���Ŀ¼�����Ϣ��������
            Dir_Basic.basicdir[i].id = (unsigned short) (i + 1);
            Dir_Basic.basicdir[i].type = 1;
            strcpy(Dir_Basic.basicdir[i].D_Name, name_add);

            Dir_Current.subdir[j].i_ino = Dir_Basic.basicdir[i].id;  //�޸ĵ�ǰĿ¼�����Ϣ��������
            Dir_Current.subdir[j].id = (unsigned short) (j + 1);
            Dir_Current.subdir[j].type = 1;
            strcpy(Dir_Current.subdir[j].D_Name, name_add);
            Dinode_Add.di_number = 1;       //�����µ�Ŀ¼��i�ڵ���Ϣ
            Dinode_Add.di_mode = 3;
            Dinode_Add.di_uid = Current_Uid;
            Dinode_Add.di_gid = Current_Gid;
            Dinode_Add.di_size = 1;

            Dinode_Add.di_addr[0] = pd - disk;       //��ռ��������
            for (int l = 1; l < 10; ++l) {
                Dinode_Add.di_addr[l] = 0;
            }
            strcpy(Dir_Add.subdir[0].D_Name, name_add);          //0����������ı���Ŀ¼�����ֺ�ino��
            Dir_Add.subdir[0].i_ino = Dir_Basic.basicdir[i].id;     //�ڴ�Ŀ¼�µ�ino�Ŷ�Ӧ��bfd�е�id��
            Dir_Add.subdir[0].id = 1;
            Dir_Add.subdir[0].type = 1;
            strcpy(Dir_Add.subdir[1].D_Name, Dir_Current.subdir[0].D_Name);       //1�����������ϼ�Ŀ¼�����ֺ�ino��
            Dir_Add.subdir[1].i_ino = Dir_Current.subdir[0].i_ino;
            Dir_Add.subdir[1].id = 2;
            Dir_Add.subdir[1].type = 1;
            // ��ʼ��

            for (int l = 2; l < 32; ++l) {
                strcpy(Dir_Add.subdir[l].D_Name, "     ");
                Dir_Add.subdir[l].i_ino = 0;
                Dir_Add.subdir[l].id = l + 1;
                Dir_Add.subdir[l].type = 2;
            }
            unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
            Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[c_flag].i_ino);
            SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
            memmove(it2, &Dir_Current, sizeof(SubDir));      //��Dir_Current����Ϣ����

            memmove(pd, &Dir_Add, sizeof(SubDir));       //���µ�Ŀ¼��浽���ݿ���
            memmove(pi, &Dinode_Add, sizeof(Dinode));       //���µ�i�ڵ�浽���ݿ���
            //�������ݿ飬д�����ݿ�
        }
    }
}

void DeleteSubDir(string name) {
    int i, flag, number;
    char name_del[10];
    strcmp(name_del,name.c_str());
    strcpy(name_del, name.c_str());
    struct SubDir Dir_Del;
    //cout << "������Ҫɾ����Ŀ¼��" << endl;
    //scanf("%s", name_del);
    for (i = 2; i < 32; i++) {
        if (!strcmp(Dir_Current.subdir[i].D_Name, name_del)) break;
    }
    if (i == 32) {
        cout << "δ�ҵ�Ҫɾ����Ŀ¼" << endl;
    } else {
        if (Dir_Current.subdir[i].type == 0) cout << "δ�ҵ�Ҫɾ����Ŀ¼" << endl;
        else {       //�����ҵ���Ŀ¼���ж��Ƿ����ļ���δɾ����
            flag = Dir_Current.subdir[i].i_ino - 1;    //flagΪ��Ŀ¼���ڻ���Ŀ¼���е�����
            Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[flag].i_ino);       //�׵�ַ��ƫ����ָ��i�ڵ�����
            SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
            memmove(&Dir_Del, it2, sizeof(SubDir));       //����Ŀ¼����Ϣ���Ƹ�Dir_Current
            for (number = 2; number < 32; number++) {
                if (Dir_Del.subdir[number].type != 2) break;
            }
            if (number < 32) {
                cout << "��Ŀ¼�»����ļ����޷�ɾ��" << endl;
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
                memmove(it2, &Dir_Current, sizeof(SubDir));      //��Dir_Current����Ϣ����
                cout << "��Ŀ¼�ͷųɹ�" << endl;
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
            cout << "��Ŀ¼���޷�����" << endl;
        } else {//������һ��
            string temp = Dir_Current.subdir[0].D_Name;
            Full_Dir = Full_Dir.substr(0, Full_Dir.size() - (temp.size() + 1));

            flag = Dir_Current.subdir[1].i_ino - 1;
            // (disk + Dir_Basic.basicdir[flag].i_ino);//���̿�ͷ�����ַ + Dir_Basic.basicdir[i].i_ino,�õ�Ҫ�ҵ�i�ڵ��ʵ�������ַ�����Զ�λ�������i�ڵ������ڴ�����
            Dinode *it1 = (Dinode *) (disk +
                                      Dir_Basic.basicdir[flag].i_ino);//���̿�ͷ�����ַ + Dir_Basic.basicdir[i].i_ino,�õ�Ҫ�ҵ�i�ڵ��ʵ�������ַ�����Զ�λ�������i�ڵ������ڴ�������ǿ������ת������ʾ�õ�ַ��Dinode����
            SubDir *it2 = (SubDir *) (disk +
                                      ((it1->di_addr)[0]));//it1ָ����ڴ������ַ�������У�addr�����0�ŵ�Ԫ��ȡ�����ݣ��ټ��ϴ��̿�ͷ�����ַ�� �ɵõ���ǰĿ¼��ʵ���ڴ������ַ��
            // SubDir it2;
            // memmove(&it2,disk + (it1->di_addr[0]), sizeof(SubDir));
            memmove(&Dir_Current, it2, sizeof(SubDir));//��SubDir��ʵ�������ַ��it2��ָ����ڴ�����ݿ�����Dir_Current�ĵ�ַ��ȥ����СΪһ��SubDir�Ĵ�С��
            strcpy(Current_Dir_Name, Dir_Current.subdir[0].D_Name);
        }


    } else {        //������һ��
        for (i = 2; i < 32; i++) {
            if (!strcmp(Dir_Current.subdir[i].D_Name, route)) break;
        }
        if (i == 32) {
            cout << "��Ŀ¼��δ�ҵ���Ҫ�������Ŀ¼��" << endl;
        } else {
            if (Dir_Current.subdir[i].type == 0) {
                cout << "��Ŀ¼��δ�ҵ���Ҫ�������Ŀ¼��" << endl;
            } else {
                flag = Dir_Current.subdir[i].i_ino - 1;             //flag�Ǵ�Ŀ¼�ڻ���Ŀ¼���еı�ţ�����-1
                Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[flag].i_ino);
                SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
                memmove(&Dir_Current, it2, sizeof(SubDir));       //����Ŀ¼����Ϣ���Ƹ�Dir_Current
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
        cout << "����Ŀ¼�������޷������ļ�";
    } else {
        for (j = 2; j < 32; j++) {
            if (Dir_Current.subdir[j].type == 2) break;
        }
        if (j == 32) {
            cout << "��ǰĿ¼�������޷������ļ�" << endl;
        } else {
            for (k = 0; k < 32; k++) {
                if (!strcmp(name_add, Dir_Current.subdir[k].D_Name) && Dir_Current.subdir[k].type == 0) break;
            }
            if (k < 32) {
                cout << "�뵱ǰĿ¼���Ѵ����ļ�����������ʧ��" << endl;
            } else {
                byte *pi = Alloc_inode(disk);       //i�ڵ���п����
                byte *pf = Alloc_data_block(disk);       //�Դ������ļ�����������ݿ�

                Dir_Basic.basicdir[i].id = (i + 1);   //�Ի���Ŀ¼��Ŀ¼����ļ���Ϣ����
                Dir_Basic.basicdir[i].type = 0;
                strcpy(Dir_Basic.basicdir[i].D_Name, name_add);
                Dir_Basic.basicdir[i].i_ino = pi - disk;

                Dir_Current.subdir[j].i_ino = Dir_Basic.basicdir[i].id; //�Ե�ǰĿ¼��Ŀ¼����ļ���Ϣ����
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
                memmove(it2, &Dir_Current, sizeof(SubDir));      //��Dir_Current����Ϣ����
                memmove(pf, s, sizeof(s));       //���µ��ļ��浽���ݿ���
                memmove(pi, &Dinode_Add, sizeof(Dinode));       //���µ�i�ڵ�浽���ݿ���
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
        cout << "δ�ҵ�Ҫɾ�����ļ�" << endl;
    } else {
        if (Dir_Current.subdir[i].type == 1)
            cout << "δ�ҵ�Ҫɾ�����ļ�" << endl;
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
                Free_inode(disk, Dir_Basic.basicdir[flag].i_ino + disk);      //�������ļ���Ϊ0����ɾ����i�ڵ�
            }
            Free_data_block(disk,
                            ((Dinode *) (Dir_Basic.basicdir[flag].i_ino + disk))->di_addr[0] + disk);   //�ͷ��ļ���ռ�����ݿ�
            unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
            Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[c_flag].i_ino);
            SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
            memmove(it2, &Dir_Current, sizeof(SubDir));      //��Dir_Current����Ϣ����
            cout << "�ļ��ͷųɹ�" << endl;
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
        cout << "��ǰĿ¼��δ�ҵ�Ҫ���Ƶ��ļ�" << endl;
    } else {
        Copy_Item.i_ino = Dir_Current.subdir[i].i_ino;
        Copy_Item.type = Dir_Current.subdir[i].type;
        strcpy(Copy_Item.D_Name, Dir_Current.subdir[i].D_Name);
        cout << "�ļ����Ƴɹ�" << endl;
    }
}

void PasteFile() {
    int i, j;
    if (Copy_Item.type != 0) {
        cout << "��ǰδ���ļ����и���" << endl;
    } else {
        for (i = 2; i < 32; i++) {
            if (Dir_Current.subdir[i].type == 2) break;
        }
        if (i == 32) {
            cout << "��ǰĿ¼�����޷�����ճ��" << endl;
        } else {
            for (j = 2; j < 32; j++) {
                if (strcmp(Dir_Current.subdir[j].D_Name, Copy_Item.D_Name) == 0 &&
                    Dir_Current.subdir[j].type == 0)
                    break;
            }
            if (j < 32) {
                cout << "��Ŀ¼���������ļ������ܸ���" << endl;
            } else {
                Dir_Current.subdir[i].i_ino = Copy_Item.i_ino;
                Dir_Current.subdir[i].type = 0;
                Dir_Current.subdir[i].id = (unsigned short) (i + 1);
                strcpy(Dir_Current.subdir[i].D_Name, Copy_Item.D_Name);
                unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
                Dinode *it1 = (Dinode *) (disk + Dir_Basic.basicdir[c_flag].i_ino);
                SubDir *it2 = (SubDir *) (disk + (it1->di_addr[0]));
                memmove(it2, &Dir_Current, sizeof(SubDir));      //��Dir_Current����Ϣ����
                cout << "�ļ�ճ���ɹ�" << endl;
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
        cout << "��ǰĿ¼��δ�ҵ�Ҫ���������ļ�" << endl;
    } else
    {
        cout<<"input the new name:";
        cin>>new_name;
        strcpy(Dir_Current.subdir[i].D_Name,new_name.c_str());
        unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
        Dinode * it1 = (Dinode *)(disk + Dir_Basic.basicdir[c_flag].i_ino);
        SubDir * it2 = (SubDir *)(disk + (it1->di_addr[0]));
        memmove(it2,&Dir_Current,sizeof(SubDir));      //��Dir_Current����Ϣ����
        cout << "�ļ��������ɹ�" << endl;
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
        cout << "��ǰĿ¼��δ�ҵ�Ҫ��������Ŀ¼" << endl;
    } else
    {
        cout<<"input the new name:";
        cin>>new_name;

        strcpy(Dir_Current.subdir[i].D_Name,new_name.c_str());
        unsigned short c_flag = Dir_Current.subdir[0].i_ino - 1;
        Dinode * it1 = (Dinode *)(disk + Dir_Basic.basicdir[c_flag].i_ino);
        SubDir * it2 = (SubDir *)(disk + (it1->di_addr[0]));
        memmove(it2,&Dir_Current,sizeof(SubDir));      //��Dir_Current����Ϣ����
        Cd_Dir(new_name);

        strcpy(Dir_Current.subdir[0].D_Name,new_name.c_str());
        unsigned short n_flag = Dir_Current.subdir[0].i_ino - 1;
        Dinode * it3 = (Dinode *)(disk + Dir_Basic.basicdir[n_flag].i_ino);
        SubDir * it4 = (SubDir *)(disk + (it3->di_addr[0]));
        memmove(it4,&Dir_Current,sizeof(SubDir));       //������Ŀ¼�������Ŀ¼�ĵ����������޸ġ�
        Cd_Dir("-");
        cout << "�ļ��������ɹ�" << endl;
    }
}


void Sign() {
    int i, k;
    char username[10];
    struct SubDir Dir_Add;
    struct Dinode Dinode_Add;
    for (i = 0; i < 8; i++) {       //������Ŀ¼����û������޿���
        if (Dir_Basic.basicdir[i].type == 2) break;
    }
    if (i == 8) {
        cout << "�û����������޷�ע���µ��û�" << endl;
    } else {
        cout << "������Ҫ�û�����������10λ��";
        scanf("%s", username);
        for (k = 0; k < 8; k++) {
            if (!strcmp(username, Dir_Current.subdir[k].D_Name) && Dir_Current.subdir[k].type == 1) break;
        }
        if (k < 8) {
            cout << "ע���û�������������ʧ��" << endl;
        } else {
            byte *pi = Alloc_inode(disk);       //i�ڵ���п����
            byte *pd = Alloc_data_block(disk);       //��Ŀ¼����п���з���
            Dir_Basic.basicdir[i].i_ino = (unsigned short) (pi - disk);   //�޸Ļ���Ŀ¼�����Ϣ��������
            Dir_Basic.basicdir[i].id = (unsigned short) (i + 1);
            Dir_Basic.basicdir[i].type = 1;
            strcpy(Dir_Basic.basicdir[i].D_Name, username);

            Dinode_Add.di_number = 1;       //�����µ�Ŀ¼��i�ڵ���Ϣ
            Dinode_Add.di_mode = 3;
            Dinode_Add.di_uid = (unsigned short) (i + 1);
            Dinode_Add.di_gid = (unsigned short) (i + 1);
            Dinode_Add.di_size = 1;

            Dinode_Add.di_addr[0] = (pd - disk);       //��ռ��������
            strcpy(Dir_Add.subdir[0].D_Name, username);          //0����������ı���Ŀ¼�����ֺ�ino��
            Dir_Add.subdir[0].i_ino = Dir_Basic.basicdir[i].id;     //�ڴ�Ŀ¼�µ�ino�Ŷ�Ӧ��bfd�е�id��
            Dir_Add.subdir[0].id = 1;
            Dir_Add.subdir[0].type = 1;
            strcpy(Dir_Add.subdir[1].D_Name, "BasicDir");       //1�����������ϼ�Ŀ¼�����ֺ�ino��
            Dir_Add.subdir[1].i_ino = (i + 1);
            Dir_Add.subdir[1].id = 2;
            Dir_Add.subdir[1].type = 1;

            memmove(pd, &Dir_Add, sizeof(SubDir));       //���µ��û�Ŀ¼�浽���ݿ���
            memmove(pi, &Dinode_Add, sizeof(Dinode));       //���µ�i�ڵ�浽���ݿ���
            //�������ݿ飬д�����ݿ�
        }
    }
}

void Display() {
    int i, j;
    //cout<<"��ǰĿ¼��"<<Dir_Current.subdir[0].D_Name<<endl;
    //cout << "current dir is " << Dir_Current.subdir[0].D_Name << endl;
    //cout << Full_Dir << ":" << endl;
    //cout<<"��ǰĿ¼�µ��ļ���"<<endl;
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