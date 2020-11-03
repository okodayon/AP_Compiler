#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#define N 256

typedef struct Token Token;
struct Token{
    char *type;
    char *nextpt;
};


//ここでTokenを作成
//Tokenのtypeに文字の種類
//nextptに次のreadlineの位置を格納して返す
//つまりここで二桁以上の数字の場合はポインタを増やす必要がある
static Token *crtoken(char *readline){
    Token *cur;
    char *p;
    p = readline;
    cur = (Token*)calloc(1,sizeof(Token));
    if(cur == NULL)exit(1);

    for(;;){
        if(isdigit(*p)){
            while(isdigit(*p) != 0){
                p++;
            }
            cur->type = "NUMBER ";
            cur->nextpt = p;
            return cur;
        }

        //isspaceより下においてしまうと動かなくなる
        if ('\n' == *p){
            cur->type = "EOF\n";
            cur->nextpt = NULL;
            return cur;
        }


        if(isspace(*p)){
            p++;
        }

        if('+' == *p){
            cur->type = "PLUS ";
            cur->nextpt = ++p;
            return cur;
        }

        if('-' == *p){
            cur->type = "MINUS ";
            cur->nextpt = ++p;
            return cur;
        }
    }
}


static Token **tokenize(char *readline) {
    //Tokenのダブルポインタを作成
    //originalはfree用
    //とりあえすココでは10個確保
    Token **tklist;
    Token **original;
    Token *cur;
    tklist = (Token **)calloc(10,sizeof(Token*));
    if(tklist == NULL) exit(1);
    original = tklist;

    while(*readline != '\n'){
       cur = crtoken(readline);
       readline = cur->nextpt;
       *tklist = cur;
       tklist++;
    }

    //最後にEOFを入れる。本当は別に書きたくなかった(思いつかない
    cur = crtoken(readline);
    *tklist = cur;

    return original;
}

static void printlist(Token **original){
    Token **p;
    p = original;

    for(;;){
        printf("%s",(*p)->type);
        if((*p)->nextpt == NULL){
            free(*p);
            break;
        }
        free(*p);
        p++;
    }
    //いい書き方が思いつかなかった
    /*
    while((*p)->nextpt != NULL){
        printf("%s",(*p)->type);
        free(*p);
        p++;
    }
    printf("%s",(*p)->type);
    free(*p);
    */

    free(original);
}



static char *fileopen(char *filename,char *readline){
    FILE *fp;

    if((fp = fopen(filename,"r")) == NULL){
        fprintf(stderr,"fopenできませんでした\n");
        exit(EXIT_FAILURE);
    }

    fgets(readline,N,fp);
    fclose(fp);
}





int main(){
    char *filename = "text.txt";
    char readline[N] = {'\n'};

    fileopen(filename,readline);
    printlist(tokenize(readline));

    return 0;

}


