#include "filesys.h"
using namespace std;

byte * Load_disk()
{
    byte * disk = (byte *) malloc(SIZE_OF_DISK);//new byte[SIZE_OF_DISK];
    FILE *f = fopen(NAME_DISK,"rb+");
    if(f == NULL)
    {
        cout <<"The virtual disk file "<< NAME_DISK <<" is not exist. "<<endl;
        cout <<"Create disk and formating..."<<endl;
        Format(disk);
        cout << "Format successful."<<endl;
        fclose(f);
        Save_disk(disk);
    }
    else
    {
        cout <<"The virtual disk file "<< NAME_DISK <<"is opened. "<<endl;
        char a = ' ';
        while (a != 'y' && a != 'n')
        {
            cout << "Format ? (y)es/(n)o : ";
            cin >> a ;
            if(a != 'y' && a != 'n')
            {
                cout << "Invalid input !"<<endl;
            }
        }
        switch (a)
        {
            case 'y' : {
                Format(disk);
                fclose(f);
                Save_disk(disk);
                // Çå¿ÕÎÄ¼þ
                FILE *fp = fopen("password.txt","w");
                fclose(fp);
            }break;
            case 'n' : {
                int j = 0;
                fread(disk,1, SIZE_OF_DISK,f);
                fclose(f);
            }break;
            default:break;
        }
    }
    return disk;
}
