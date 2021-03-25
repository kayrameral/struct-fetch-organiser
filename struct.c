#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parseJSON(char *filename);
void parseLine(char* token,char* item,char* desc[1000],char* output,
               char* lastOutput[1000],int numOfTab,int* now);

int main(int argc, char *argv[]){
 
    if(argc==2)
        parseJSON(argv[1]);
    else if(argc==1)
        printf("The path of file is missing!"); 
    else
        printf("Command line argument is not correct!");
    
    return 0;
}

void parseJSON(char *filename){

    char *lastOutput[1000];
    char *output = "";

    char *token, *token2;
    char *item = "$";
    char *temp;
    char *desc[1000];
    char line[1000];

    int now = 0;

    FILE *fp = fopen(filename, "r");
    
    if (fp == NULL){ 
        printf("Could not open file %s", filename); 
        return;
    } 
    
    while(fgets(line, sizeof(line), fp) != NULL){
        
        if(strstr(line,":") == NULL) continue;
        
        int index = 0;
        int numOfTab = 0;

        while(line[index]==' '){
            numOfTab+=1;
            index+=1;
        }

        numOfTab/=4; 

        if(numOfTab == 0){
            if(line[0]=='{'){
                temp = (char *) malloc(1 + strlen(item)+ 1);
                strcpy(temp, item);
                strcat(temp, ".");
                item = (char *) malloc(1 + strlen(temp));
                strcpy(item,temp);
            }
            else if(line[0]=='['){
                temp = (char *) malloc(1 + strlen(item)+ 2);
                strcpy(temp, item);
                strcat(temp, "[]");
                item = (char *) malloc(1 + strlen(temp));
                strcpy(item,temp);
            }
        }

        token = strtok(line, "\"");
        token = strtok(NULL, "\"");

        temp = (char *) malloc(1 + strlen(item)+ strlen(token));
        strcpy(temp, item);
        strcat(temp, token);
        item = (char *) malloc(1 + strlen(temp));
        strcpy(item,temp);

        token = strtok(NULL, ":");
        parseLine(token,item,desc,output,lastOutput,numOfTab,&now);
  
        while(strstr(token,":")){
            item = "";
            token2 = strtok(token, "\"");
            token2 = strtok(NULL, "\"");
            
            temp = (char *) malloc(1 + strlen(item)+ strlen(token2));
            strcpy(temp, item);
            strcat(temp, token2);
            item = (char *) malloc(1 + strlen(temp));
            strcpy(item,temp);

            token2 = strtok(token,":");
            parseLine(token2,item,desc,output,lastOutput,numOfTab+1,&now);
        }

        item = "";
        token = "";
        token2 = "";
    }
    int index = 0;
    while(index<=now){
        printf("%s\n",lastOutput[index]);
        index++;
    }
}

void parseLine(char* token,char* item,char* desc[1000],char* output,
               char* lastOutput[1000],int numOfTab,int* now){

    int index = 0;
    char* temp;

    while(token[index] == ' '){
        index++;
    }

    if(token[index] == '['){
        temp = (char *) malloc(1 + strlen(item)+ 2);
        strcpy(temp, item);
        strcat(temp, "[]");
        item = (char *) malloc(1 + strlen(temp));
        strcpy(item,temp);
    }
    else if(token[index] == '{'){
        temp = (char *) malloc(1 + strlen(item)+ 1);
        strcpy(temp, item);
        strcat(temp, ".");
        item = (char *) malloc(1 + strlen(temp));
        strcpy(item,temp);
    }
    
    temp = (char *) malloc(1 + strlen(item));
    strcpy(temp, item);
    desc[numOfTab] = (char *) malloc(1 + strlen(temp)); 
    strcpy(desc[numOfTab],temp);

    if(numOfTab != 0){
        index = 0;
        while(index<=numOfTab){
            temp = (char *) malloc(1 + strlen(output)+ strlen(desc[index]));
            strcpy(temp, output);
            strcat(temp, desc[index]);
            output = (char *) malloc(1 + strlen(temp));
            strcpy(output,temp);
            index++;
        }

        if(*now==0){
            lastOutput[0] = output;     
            *now = *now + 1;      
        }
        else{
            index = 0;
            while(index<*now){
                if(strcmp(lastOutput[index],output) == 0) return;
                index++;
            }
            lastOutput[*now] = output;
            *now = *now + 1;
        }
    }
}