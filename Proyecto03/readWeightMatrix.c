#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "readWeightMatrix.h"

void readWeightMatrix(int returnVal[][98]){
    FILE *fp;
    char* line = NULL;
    ssize_t read;
    size_t len = 0;
    int i,j;   
    fp = fopen("threadville.csv","r");
    if(fp == NULL){
        printf("The weights file is not well named or missing\n");
        exit(0);
    }else{
    // Create a return value and initialize it in 0
        printf("Here\n");
        for(i = 0;i<98;i++)
             for(j=0;j<98;j++)returnVal[i][j] = 0;
        printf("Here2\n");
        i = 0;
        j = 0;
        //Read the file line by line to get the numbers
        char* split;       
        while((read = getline(&line, &len, fp)) != -1){
           split = strtok(line,",");
           while(split != NULL){
                int val = atoi(split);    
                returnVal[i][j] = val;    
                j++;
                split = strtok(NULL,",");
           }
           j = 0;           
           i++;
        }
        fclose(fp);
        return;
        
    }
    fclose(fp);  
}
