#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void parseQuery(char *filename, char query[100]);
void append(char token[100][50],int childNo,char c);
void getChild(char *filename,char token[100][50],int* arrayOrChild,int arrayIndex[100][2],
              int* thereIsTwo,int* getEveryChild,int childNo);

int main(int argc, char *argv[]){
    
    char query[100]; 

    if(argc==2){
        printf("Enter a query to process: ");
        fgets(query,100,stdin);
        query[strcspn(query, "\n")] = 0;
        parseQuery(argv[1],query);
    }
    else if(argc==1)
        printf("The path of file is missing!"); 
    else
        printf("Command line argument is not correct!");

    return 0;
}

void parseQuery(char *filename,char query[100]){

    if(query[0] != '$') return;
    
    int childNo = -1;

    int arrayOrChild[100];
    int thereIsTwo[100];
    int getEveryChild[100];
    int arrayIndex[100][2];

    char token[100][50];

    for(int i=0; i<100; i++){
        for(int j=0; j<50; j++){
            token[i][j] = '\0';
        }
    }

    for(int i=0; i<100; i++){
        arrayOrChild[i] = 0;
        thereIsTwo[i] = 0;
        getEveryChild[i] = 0;
        arrayIndex[i][0] = 0;
        arrayIndex[i][1] = 0;
    }

    for(int i=1; i<100; i++){
        if(query[i]=='\0') break;
        else if(query[i]=='.') childNo++;
        else if(query[i]=='[') arrayOrChild[childNo] = 1;
        else if(query[i]=='*') getEveryChild[childNo-1] = 1;
        else if(query[i]==',') thereIsTwo[childNo] = 1;
        else if(isalnum(query[i]) && arrayOrChild[childNo] == 0) 
            append(token,childNo,query[i]);
        
        else if(isdigit(query[i]) && arrayOrChild[childNo] == 1){
            if(thereIsTwo[childNo] == 1) arrayIndex[childNo][1] = query[i]-'0';
            else arrayIndex[childNo][0] = query[i]-'0';
        }
    }

    //$.shop.book[1]
    getChild(filename,token,arrayOrChild,arrayIndex,thereIsTwo,getEveryChild,childNo);
}

void getChild(char *filename,char token[100][50],int* arrayOrChild,int arrayIndex[100][2],
              int* thereIsTwo,int* getEveryChild,int childNo){
    
    printf("\nresult: [\n");

    FILE *fp;
    fp = fopen(filename,"r");

    if (fp == NULL){ 
        printf("Could not open file %s", filename); 
        return;
    } 

    char line[1000];
    char line2[1000];

    int index = 0, lineIndex = 0;
    int lastTab = -1;

    int findOnlyChild = 1;
    int findArrayChild = 0;
    int findAllChild = 0;
    int findEveryItem = 0;
    int numOfArrayChild = -1;
    int indexOfReadChild = -1;
    int indexOfChild[2] = {0,0};
    
    while(fgets(line, sizeof(line), fp) != NULL){

        if(index>childNo) break;

        int numOfTab = 0;
        while(line[lineIndex]==' '){
            numOfTab+=1;
            lineIndex+=1;
        }
        numOfTab/=4; 

        if(findEveryItem==1 && strstr(line,token[index])){
            printf("%s",line);
        }

        if(findAllChild==1){
            if(numOfTab > lastTab) printf("%s", line);
            if(numOfTab <= lastTab){
                findAllChild == 0;
                index++;
            }
        }

        if(findArrayChild==1 && numOfTab >= lastTab*2){

            if(numOfArrayChild!=-1 && indexOfReadChild==numOfArrayChild){
                findArrayChild = 0;
                numOfArrayChild = -1;
                indexOfReadChild = -1;
                findOnlyChild = 1;
                index++;
                continue;
            }else if((indexOfChild[0]!=0 && indexOfChild[1]!=0) && 
                     (indexOfReadChild > indexOfChild[1])){
                findArrayChild = 0;
                indexOfChild[0] = 0;
                indexOfChild[1] = 0;
                indexOfReadChild = -1;
                findOnlyChild = 1;
                index++;
                continue;
            }

            if((indexOfChild[0]==0 && indexOfChild[1]==0) && 
               (indexOfReadChild+1 == numOfArrayChild))
                printf("%s", line);
            else if((indexOfChild[0]!=0 || indexOfChild[1]!=0) && 
                (indexOfReadChild <= indexOfChild[1]-1) && (indexOfReadChild>= indexOfChild[0]-1))
                printf("%s", line);
            
            if((numOfTab == lastTab*2) && strstr(line,"}")!=NULL) indexOfReadChild++;
        }

        if(findOnlyChild==1 && strlen(token[index])==0){
            index++;
            findEveryItem = 1;
            findOnlyChild = 0;
        }

        if(findOnlyChild==1 && strstr(line,token[index])!= NULL){

            if(arrayOrChild[index] == 0 && getEveryChild[index]!=1){
                findOnlyChild = 1;
                findArrayChild = 0;
                findAllChild = 0;
                index++;
            }
            else if(arrayOrChild[index] == 0 && getEveryChild[index]==1){
                findOnlyChild = 0;
                findArrayChild = 0;
                findAllChild = 1;
            }
            else if(arrayOrChild[index] == 1){
                findOnlyChild = 0;
                findArrayChild = 1;
                findAllChild = 0;
                
                if(thereIsTwo[index] == 0) numOfArrayChild = arrayIndex[index][0];
                else{
                    indexOfChild[0] = arrayIndex[index][0];
                    indexOfChild[1] = arrayIndex[index][1];
                }
            }
            lastTab = numOfTab;
        } 
        lineIndex = 0;
    }

    printf("]");
    
    /*
    printf("\nTokens: ");
    for(int i=0; i<=childNo; i++){
        for(int j=0; j<50; j++){
            printf("%c", token[i][j]);
        }
        printf(" ");
    }

    printf("\nArray or child: ");
    for(int i=0; i<=childNo; i++){
        printf("%d ", arrayOrChild[i]);
    }

    printf("\nArray index: ");
    for(int i=0; i<=childNo; i++){
        printf("%d %d | ", arrayIndex[i][0], arrayIndex[i][1]);
    }

    printf("\nThere is two: ");
    for(int i=0; i<=childNo; i++){
        printf("%d ", thereIsTwo[i]);
    }

    printf("\nEvery child: ");
    for(int i=0; i<=childNo; i++){
        printf("%d ",getEveryChild[i]);
    } */
}

void append(char token[100][50],int childNo,char c) {
    int len = strlen(token[childNo]);
    token[childNo][len] = c;
    token[childNo][len+1] = '\0';
}