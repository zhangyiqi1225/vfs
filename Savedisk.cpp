#include "filesys.h"
using namespace std;

void Save_disk(byte *disk)
{
    FILE *f = fopen(NAME_DISK,"wb+");
    int j = 0;
    fwrite(disk,1,SIZE_OF_DISK,f);//写入文件
    fclose(f);
}
