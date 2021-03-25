#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void readDoc(char* filename,char* sortBy[10],int itemNo);
void findErrors(char* items[20],char* sortBy[10],int itemNo,int itemIndex);
int checkIfThisUnique(char* itemNames[20],char* item,int itemNum);
int checkIfThisAlpNum(char *token);

void organizeItems(char* items[20],int itemNo,int itemIndex,char* sortBy[10]);
char* findValue(char* sortBy,char* item);
void writeErrors(char* erroredItems[20],int errorNum);
void writeIntoResult(char* items[20],int itemIndex);

int main(int argc, char *argv[]){
    
    char* sortBy[10];
 
    int index = 2, itemNo = 0;

    if(argc>=4){
        while(index<argc){
            sortBy[itemNo] = (char*) malloc(strlen(argv[index])*sizeof(char));
            strcpy(sortBy[itemNo],argv[index]);
            itemNo++;
            index+=2;
        }
        readDoc(argv[1],sortBy,itemNo);
    }
    else if(argc==2){
        readDoc(argv[1],sortBy,itemNo);
    }
    else
        printf("Command line argument is not correct!");
    return 0;
}

void readDoc(char* filename,char* sortBy[10],int itemNo){

    FILE *fp;
    fp = fopen(filename,"r");

    if (fp == NULL){ 
        printf("Could not open file %s", filename); 
        return;
    } 

    char line[1000];
    char* items[20];
    char *temp;

    for(int i=0; i<20; i++){
        items[i] = "";
    }

    int isNew = 0;
    int itemIndex = 0;

    while(fgets(line, sizeof(line), fp) != NULL){
        if(strstr(line,"@") && isNew==1){
            itemIndex++;
            isNew=0;
        }
        if(strstr(line,"@") && isNew==0){
            isNew=1;
        }
        temp = (char*) malloc(strlen(items[itemIndex])+strlen(line)+1);
        strcpy(temp,items[itemIndex]);
        strcat(temp,line);
        items[itemIndex] = (char *) malloc(1 + strlen(temp));
        strcpy(items[itemIndex],temp);
    }
    fclose(fp);

    findErrors(items,sortBy,itemNo,itemIndex);
    if(itemNo!=0) organizeItems(items,itemNo,itemIndex,sortBy);
}

void findErrors(char* items[20],char* sortBy[10],int itemNo,int itemIndex){

    char *itemNames[20];
    char *erroredItems[20];
    char *token;
    int errorNum = 0;

    for(int i=0; i<=itemIndex; i++){

        token = (char*) malloc(strlen(items[i])+1);
        strcpy(token,items[i]);
        itemNames[i] = strtok(token,"{");
        itemNames[i] = strtok(NULL,",");

        if(checkIfThisAlpNum(itemNames[i])==0 || checkIfThisUnique(itemNames,itemNames[i],i)==0
        || strstr(items[i],"=") == NULL){
            erroredItems[errorNum] = (char*) malloc(strlen(items[i])+1);
            strcpy(erroredItems[errorNum],items[i]);
            items[i] = '\0';
            errorNum++;
            continue;
        }

        int index = 0;
        while(index<itemNo){
            if(strstr(items[i],sortBy[index]) == NULL){
                erroredItems[errorNum] = (char*) malloc(strlen(items[i])+1);
                strcpy(erroredItems[errorNum],items[i]);
                items[i] = '\0';
                errorNum++;
                break;
            }
            index++;
        }
    }
    if(errorNum>0) writeErrors(erroredItems,errorNum);
}

int checkIfThisUnique(char* itemNames[20],char* item,int itemNum){
    for(int i=0; i<itemNum; i++){
        if(strcmp(itemNames[i],item)==0) return 0;
    }
    return 1;
}

int checkIfThisAlpNum(char *token){
    int index = 0;
    int isAlphFound = 0;

    while(token[index]!='\0' && token[index]!='\n'){
        if(isdigit(token[index]) && isAlphFound==0) return 0;
        if(isalpha(token[index])) isAlphFound=1;
        index++;
    }
    return 1;
}

void organizeItems(char* items[20],int itemNo,int itemIndex,char* sortBy[10]){

    char *temp = "", *temp2 = "";
    int index = 0;

    for(int i=0; i<=itemIndex-1; i++){
        if(items[i]==NULL) continue;

        for(int j=i+1; j<=itemIndex; j++){
            if(items[j]==NULL) continue;

            while(index<itemNo){
                char *value1 = findValue(sortBy[index],items[i]);
                char *value2 = findValue(sortBy[index],items[j]);

                if(strcmp(value1,value2)>0){
                    temp = (char*) malloc(strlen(items[i])+1);
                    strcpy(temp,items[i]);
                    temp2 = (char*) malloc(strlen(items[j]+1));
                    strcpy(temp2,items[j]);
                    items[i] = (char*) malloc(strlen(temp2));
                    items[j] = (char*) malloc(strlen(temp));
                    strcpy(items[i],temp2);
                    strcpy(items[j],temp);
                    break;
                }
                else if(strcmp(value1,value2)<0) break;
                index++;
            }
            index = 0;
        }
    }
    writeIntoResult(items,itemIndex);
}

char* findValue(char* sortBy,char* item){
    
    int index=0, index2=0, start=0, end=0;
    char *value, *found;

    found = strstr(item,sortBy);
    while(found[index]!='{') index++;
    start = index+1;
    while(found[index]!='}') index++;
    end = index;

    value = (char*) malloc((end-start+2)*sizeof(char));

    while(start<end){
        value[index2] = found[start];
        index2++;
        start++;
    }
    value[end] = '\0';
    return value;
}

void writeErrors(char* erroredItems[20],int errorNum){

    FILE *fp;
    fp = fopen("error.txt", "w");

    if (fp == NULL){ 
        printf("Could not write into error.txt file.\n"); 
        return;
    } 

    for(int i=0; i<errorNum; i++){
       fprintf(fp,"%s",erroredItems[i]);
    }
    printf("\nError.txt is created!");
    fclose(fp);
}

void writeIntoResult(char* items[20],int itemIndex){

    FILE *fp;
    fp = fopen("result.txt", "w");

    if (fp == NULL){ 
        printf("Could not write into result.txt file.\n"); 
        return;
    } 

    for(int i=0; i<=itemIndex; i++){
       fprintf(fp,"%s",items[i]);
    }
    printf("\nText is organized successfuly!");
    fclose(fp);
}


