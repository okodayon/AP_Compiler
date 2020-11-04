#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#define N 256

typedef struct Token Token;
struct Token{
    char *type;
    char *nextpt;
    int val;//数字の値を保持するvalを追加
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
            //ここでvalに10進数の数字を。
            cur->val = strtol(p,NULL,10);
            while(isdigit(*p) != 0){
                p++;
            }
            cur->type = "NUMBER";
            cur->nextpt = p;
            return cur;
        }

        //isspaceより下においてしまうと動かなくなる
        if ('\n' == *p){
            cur->type = "EOF";
            cur->nextpt = NULL;
            return cur;
        }


        if(isspace(*p)){
            p++;
        }

        if('+' == *p){
            cur->type = "PLUS";
            cur->nextpt = ++p;
            return cur;
        }

        if('-' == *p){
            cur->type = "MINUS";
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

//typeをprintしていた関数を削除


static char *fileopen(char *filename,char *readline){
    FILE *fp;

    if((fp = fopen(filename,"r")) == NULL){
        fprintf(stderr,"fopenできませんでした\n");
        exit(EXIT_FAILURE);
    }

    fgets(readline,N,fp);
    fclose(fp);
}



static int tkcheck(Token **cur){
    Token **p;
    p = cur;

    //正しければtypeにNUMBERを持っている構造体がココに来ているはず。
    if((*p)->type == "NUMBER"){
        return (*p)->val;
    }
    fprintf(stderr,"入力された文字列が [数字 (+ or -) 数字] の形になっていません");
    exit(1);
}


int main(){
    char *filename = "text.txt";
    char readline[N] = {'\n'};
    Token **cur;

    fileopen(filename,readline);
    //toknizeしたダブルポインタの先頭アドレスをcurに
    cur = tokenize(readline);

    //アセンブリのおまじない
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    //一番はじめの数字ここで数字でなければerror
    printf("    mov rax, %i\n",tkcheck(cur));
    //curの一番最後にはtypeがEOF\nでnextptがNULLのToken型の構造体が入っている。
    //それまでwhileで回す。
    while((*cur)->nextpt != NULL){
        //演算子の次には数字が来ているはずなので次のポインタが数字ならprint
        if((*cur)->type == "PLUS"){
            printf("    add rax, %i\n",tkcheck(++cur));
        }
        if((*cur)->type == "MINUS"){
            printf("    sub rax, %i\n",tkcheck(++cur));
        }
        cur++;
    }

    printf("    ret\n");
    return 0;

}


