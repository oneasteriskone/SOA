#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usableSpaces.h"


//StreetSpace spaceList[1230];

void readUsableFile(StreetSpace spaceList[1230]){
    FILE *fp;
    char* line = NULL;
    ssize_t read;
    size_t len = 0;
    int i,j;   
    fp = fopen("ThreadvilleUsableSpaces.txt","r");
    if(fp == NULL){
        printf("The usable spaces file is not well named or missing\n");
        exit(0);
    }else{
        read = getline(&line, &len, fp);
        if(read != -1){
            int position = 0;
            int counter1 = 0;
            int counter0 = 0;
            for(position; position < len;position++){
                if(line[position] == '1'){
                    spaceList[position].usable = 1;                 
                    spaceList[position].busy = 0;
                }
                else if(line[position] == '0'){
                    spaceList[position].usable = 0;
                }
            }            
        }else{
            printf("The usable spaces file is empty or invalid\n");
            exit(0);
        }
    }

}
