#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

char path[PATH_MAX] = "./";    //path to directory
int d, f, l, s = 0;            //options

typedef enum types {file, directory, link} types;

typedef struct item
{
    char* name;
    types type;
}item;

int sizeOfList = 0;

item makeItem(struct dirent* it)            //convert dir to item
{
    item one;
    one.name = it->d_name;
    if(it->d_type == DT_DIR) one.type = directory;
    if(it->d_type == DT_REG) one.type = file;
    if(it->d_type == DT_LNK) one.type = link;
    return one;
}

void PrintFiles(item* files)
{
    if(!files) return;
    for(int i = 0; i < sizeOfList; i++)
    {
        if(files[i].type == directory && d) puts(files[i].name);
        if(files[i].type == file && f) puts(files[i].name);
        if(files[i].type == link && l) puts(files[i].name);
    }
    free(files);
}


item* sort(item* files)
{
    if(!files) return NULL;
    if(!s) return files;
    for(int i = 0; i < sizeOfList - 1; i++)
        for(int j = 0; j < sizeOfList - i - 1; j++)
            if(strcmp(files[j].name, files[j+1].name) > 0)
            {
                item tmp = files[j];
                files[j] = files[j+1];
                files[j+1] = tmp;
            }
    return files;
}

item* readFiles(char* dirName)
{
    DIR* dir = opendir(dirName);
    if(!dir) {puts("Specified directory cannot be open or does not exist"); return NULL;}
    struct dirent* dirItem;
    item* files = NULL;
    while((dirItem = readdir(dir)) != NULL)
    {
        if(!strcmp(dirItem->d_name, ".") || !strcmp(dirItem->d_name, "..")) continue;   //skip current and parent directory

        files = (item*)realloc(files, (sizeOfList+1)*sizeof(item));                     //allocate memory for one more element
        files[sizeOfList++] = makeItem(dirItem);                                        //add element to list
    }
    closedir(dir);
    return files;
}

//get options from cmd
void readOptions(char**argv, int argc)
{
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
            for (int j = 1; argv[i][j]; j++)
            {
                if(argv[i][j] == 'l') l = 1;
                if(argv[i][j] == 'd') d = 1;
                if(argv[i][j] == 'f') f = 1;
                if(argv[i][j] == 's') s = 1;
            }
        else strcpy(path, argv[i]);
    }
    if(!l && !d && !f) d = f = l = 1;
}


int main(int argc, char*argv[])
{
    readOptions(argv, argc);
    PrintFiles(sort(readFiles(path)));
    return 0;
}
