#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include "explain.cpp"
using namespace std;

#define R2 20
#define R3 30
#define R4 8//最长标志符长度定为8
vector<string> code;//保存四元式的容器

ofstream outfile_1;
int sym;   //当前所读单词的类型
char *symval=new char [R2];  //当前所读单词的值
char c;  //当前所读取的字符
ifstream infile_1;
string line;//line表示每行
int l=0,counter=0,label1=-1,label2=-1,label3=-1,label4=-1;//label表示回填的位置
int flag=0;//判断是否需要弹出标识符的四元式
map<string, char*>chtable;//符号表,string：变量名；char *：变量地址

//状态转换函数
void P();//程序段
void D();//声明语句
void T();
void C();//标志符
void R();
char* E();//算术表达式
char* E1();
char* A();
char* A1();
char* B();
void J();//逻辑
char* M();
void S();//语句
void N();
void I();//数字（可以以0开头）
void I1();
void O();
void F();
void F1();

int MatchType(const char* trans);//转换类型
void ReadTypeValue();//读取一个字符getsym()

void insert(string name);//插入&检查是否重复定义
char* lookup(string name);//查找变量是否定义了
char *newtemp();//回送一个新的临时变量名，临时变量名产生的顺序为t1，t2，…
void emit(char *op,char *ag1,char *ag2,char *result);//生成一个三地址语句送到四元式表中

struct quad{
    char op[12];
    char ag1[12];
    char ag2[12];
    char result[12];
};
struct quad quad[R2];//表示中间代码输出
void StructToVector(struct quad quad[]);//将保存四元式的struct转化成vector

int main(int argc, const char * argv[]) {
    
    
    infile_1.open("/Users/tsq0313/Desktop/lexical_analysis.txt");
    if(!infile_1) cerr<<"open failed!"<<endl;
    
    outfile_1.open("/Users/tsq0313/Desktop/grammer_analysis.txt");
    outfile_1<<"Begin the grammer analysis:"<<endl;
       ReadTypeValue();
    
    P();
    
    if(strcmp(symval,"#")==0) outfile_1<<"success!"<<endl;
    else cerr<<"error in main!"<<endl;
    
    StructToVector(quad);
    
    outfile_1<<endl;
    outfile_1<<"四元式："<<endl;
    vector<string>::iterator it;
    for(it=code.begin();it!=code.end();it++)
    {
        outfile_1<<*it<<endl;
    }
    explain exp;
    exp.copy_map(chtable);
    exp.copy_code(code);
    exp.store_add();
    exp.expn();
    return 0;
}

void StructToVector(struct quad quad[]){
    char *str=(char*)malloc(4*R4);
    string sstr;
    for(int i=0;i<counter;i++){
    strcpy(str,"<");
    strcat(str,quad[i].op);strcat(str,",");
    strcat(str,quad[i].ag1);strcat(str,",");
    strcat(str,quad[i].ag2);strcat(str,",");
    strcat(str,quad[i].result);
    strcat(str,">");
    sstr=str;
    code.push_back(sstr);
        memset(str,0,strlen(str));
    }
}


void insert(string name){
    if(chtable.count(name)==0){//比较是否同名，无则
        char *address=(char*)malloc(1);
        chtable.insert(pair<string,char *>(name,address));
       // layer.insert(pair<string,int>(name,0));//0层表示非函数调用，即主函数调用的变量
    }
    else {
        cerr<<"符号表中已有此类型同名变量！"<<endl;exit(0);
    }
}

char* lookup(string name){
    map<string,char*>::iterator iter;
    iter = chtable.find(name);//第一个符号类型符合的数据的位置
    if (iter!=chtable.end())//说明符号表中存在对应
        return iter->second;
    else return NULL;
}

void emit(char *op,char *ag1,char *ag2,char *result)
{
    strcpy(quad[counter].op,op);
    strcpy(quad[counter].ag1,ag1);
    strcpy(quad[counter].ag2,ag2);
    strcpy(quad[counter].result,result);
    //strcpy(quad[counter].layer,"0");
    counter++;
    return;
}

char *newtemp()
{
    //l初值为0
    char *p;
    char m[R4];
    string str;
    p=(char *)malloc(R4);
    l++;//假设第一次调用该函数，此时l=1;
    sprintf(m,"%d",l);//将l值转化成字符串保存到m中，m=1;
    strcpy(p+1,m);//把m的值复制到p指针指向的地址后面一个字节开始的内存中,p[1]=1
    p[0]='t';
    str=p;
    insert(str);
    return(p);
}
int MatchType(const char* trans){
    
    int type=0;
    
    if (strcmp(trans,"int")==0) type=1;
    else if(strcmp(trans,"double")==0) type=2;
    else if(strcmp(trans,"float")==0) type=3;
    else if(strcmp(trans,"for")==0) type=4;
    else if(strcmp(trans,"printf")==0) type=5;
    else if(strcmp(trans,"scanf")==0) type=6;
    else if(strcmp(trans,"if")==0) type=7;
    else if(strcmp(trans,"enum")==0) type=8;
    else if(strcmp(trans,"else")==0) type=9;
    else if(strcmp(trans,"while")==0) type=10;
    else if(strcmp(trans,"return")==0) type=11;
    else if(strcmp(trans,"ID")==0) type=12;
    else if(strcmp(trans,"NUM")==0) type=13;
    else if(strcmp(trans,"RELOP")==0) type=19;
    else if(strcmp(trans,"0")==0) type=0;
    else{
        type=trans[0]-'0';
        type=type*10+trans[1]-'0';
    }
    return type;
}

void ReadTypeValue(){
    const char* trans;//为了将string tmp转换成char* symval,用于暂时存储
    string tmp;//tmp表示每个单词
    
    getline(infile_1,line);
    istringstream istrm(line);//创建字符串流
    istrm.get(c);//读取“<”
    
    istrm>>tmp;//读单词类型
    trans=tmp.data();
    sym=MatchType(trans);//获取该单词的类型
    
    istrm>>tmp;//读取“,”
    
    istrm>>tmp;//读单词值
    strcpy(symval,tmp.c_str());
    istrm.get(c);//读取“>”
    istrm.get(c);//读取"\n"
}

void P(){
    if(strcmp(symval,"{")==0){
        outfile_1<<"P->{DS}  :{"<<endl;
        
        ReadTypeValue();//读下一个值
        
        D();
        S();
        
        if(strcmp(symval,"}")==0){
            outfile_1<<"P->{DS}  :}"<<endl;
            ReadTypeValue();//读下一个值
        }
        else {cerr<<"P 缺'}'"<<endl;exit(0);}
        
    }
    else {cerr<<"P 缺左变量{"<<endl; exit(0);}
}
void D(){
    if(strcmp(symval,"int")==0){

        outfile_1<<"D->int CT;D   :int"<<endl;
        ReadTypeValue();//读下一个值
        string str=symval;
        C();
        T();
        insert(str);
        if(strcmp(symval,";")==0){
            outfile_1<<"D->int CT;   :;"<<endl;
            ReadTypeValue();//读下一个值
            D();
        }
        else {cerr<<"D 缺;"<<endl;exit(0);}
    }
    else if(strcmp(symval,"if")==0||strcmp(symval,"{")==0||strcmp(symval,"for")==0||
            strcmp(symval,"while")==0||strcmp(symval,"printf")==0||sym==12){
        outfile_1<<"D->null"<<endl;
    }
    else{cerr<<"D 缺左变量int"<<endl;exit(0);}
}
void S(){
    if(strcmp(symval,"if")==0){
        outfile_1<<"S->if(J)SNS   :if"<<endl;
        
        ReadTypeValue();//读下一个值
        
        if(strcmp(symval,"(")==0){
            outfile_1<<"S->if(J)SNS   :("<<endl;
            ReadTypeValue();//读下一个值
            
            J();
            label1=counter;
            emit("jp","_","_","");
            
            if(strcmp(symval,")")==0){
                outfile_1<<"S->if(J)SNS   :)"<<endl;
                
                ReadTypeValue();//读下一个值
                S();
                sprintf(quad[label1].result,"%d",counter+1);
                label2=counter;
                emit("jp","_","_","");
                
                N();
                sprintf(quad[label2].result,"%d",counter);
                S();
            }
            else {cerr<<"S 缺)"<<endl;exit(0);}
        }
        else{cerr<<"S 缺("<<endl;exit(0);}
    }
    else if(strcmp(symval,"{")==0){
        outfile_1<<"S->{S}S   :{"<<endl;
        
        ReadTypeValue();//读下一个值
        
        S();
        if(strcmp(symval,"}")==0){
            outfile_1<<"S->{S}S    :}"<<endl;
            ReadTypeValue();//读下一个值
            S();
        }
        else {cerr<<"S 缺}"<<endl;exit(0);}
    }
    else if(strcmp(symval,"}")==0||strcmp(symval,"else")==0||strcmp(symval,";")==0)
        outfile_1<<"S->null"<<endl;
    
    else if(sym==12){
        char *p=(char*)malloc(R4);
        char *q=(char*)malloc(R4);
        strcpy(p,symval);
        if(!lookup(symval))//查找是否定义过了
        {cerr<<symval<<"变量未定义"<<endl; exit(0);}
    
        outfile_1<<"S->C=E;S   :识别"<<*symval<<endl;
        C();
        if(strcmp(symval,"=")==0){
            outfile_1<<"S->C=E;S   :="<<endl;
            ReadTypeValue();//读下一个值
            q=E();
            emit("=",q,"_",p);
            if(strcmp(symval,";")==0){
                outfile_1<<"S->C=E;S   :;"<<endl;
                ReadTypeValue();//读下一个值
                S();
            }
            else{cerr<<"S 缺;"<<endl;exit(0);}
        }
        else{cerr<<"S 缺="<<endl;exit(0);}
    }
    
    else if (strcmp(symval,"for")==0){
        outfile_1<<"S->for(C=E;J;C++){S}  :  for "<<endl;
        ReadTypeValue();
        if(strcmp(symval, "(")==0){
            outfile_1<<"S->for(C=E;J;C++){S}  :  ( "<<endl;
            ReadTypeValue();
            char *p=(char*)malloc(R4);
            char *q=(char*)malloc(R4);
            strcpy(p,symval);
            C();
            if(strcmp(symval,"=")==0){
                
                outfile_1<<"S->for(C=E;J;C++){S}  :  ="<<endl;
                ReadTypeValue();//读下一个值
                q=E();
                
                emit("=",q,"_",p);
                
                emit("jp","_","_","");
                sprintf(quad[counter-1].result,"%d",counter);
                
                if(strcmp(symval, ";")==0){
                    outfile_1<<"S->for(C=E;J;C++){S}  :  ; "<<endl;
                    ReadTypeValue();
                    label1=counter;
                    J();
                    
                    label3=counter;
                    emit("jp","_","_","");
                    
                    if(strcmp(symval, ";")==0){
                        outfile_1<<"S->for(C=E;J;C++){S}  :  ; "<<endl;
                        ReadTypeValue();
                        label4=counter;
                        char *str=(char*)malloc(R4);
                        strcpy(str,symval);
                        C();
                        emit("+",str,"1",str);
                       
                        emit("jp","_","_","");
                        sprintf(quad[label3-1].result,"%d",counter);
                        sprintf(quad[counter-1].result,"%d",label1);
                        if(strcmp(symval, "++")==0){
                            outfile_1<<"S->for(C=E;J;C++){S}  :  ++ "<<endl;
                            ReadTypeValue();
                            if(strcmp(symval, ")")==0){
                                outfile_1<<"S->for(C=E;J;C++){S}  :  ) "<<endl;
                                ReadTypeValue();
                                if(strcmp(symval, "{")==0){
                                    outfile_1<<"S->for(C=E;J;C++){S}  :  { "<<endl;
                                    ReadTypeValue();
                                    S();
                                    emit("jp","_","_","");
                                    sprintf(quad[counter-1].result,"%d",label4);
                                    sprintf(quad[label3].result,"%d",counter);
                                    
                                    if(strcmp(symval, "}")==0){
                                        outfile_1<<"S->for(C=E;J;C++){S}  :  } "<<endl;
                                        ReadTypeValue();
                                        S();
                                    }
                                    else {cerr<<"S 缺}"<<endl;exit(0);}
                                }
                                else {cerr<<"S 缺{"<<endl;exit(0);}
                                }
                            else {cerr<<"S 缺)"<<endl;exit(0);}
                        }
                        else {cerr<<"S 缺++"<<endl;exit(0);}
                    }
                    else {cerr<<"S 缺;"<<endl;exit(0);}
                }
                else {cerr<<"S 缺;"<<endl;exit(0);}
                }
            else {cerr<<"S 缺="<<endl;exit(0);}
        }
        else {cerr<<"S 缺("<<endl;exit(0);}
    }
    else if(strcmp(symval, "while")==0){
        outfile_1<<"S->while(J){S}  :  while "<<endl;
        ReadTypeValue();
        if(strcmp(symval, "(")==0){
            outfile_1<<"S->while(J){S}  :  ( "<<endl;
            ReadTypeValue();
            label1=counter;
            J();
            
            label2=counter;
            emit("jp","_","_","");
            
            if(strcmp(symval, ")")==0){
                outfile_1<<"S->while(J){S}  :  ) "<<endl;
                ReadTypeValue();
                if(strcmp(symval, "{")==0){
                    outfile_1<<"S->while(J){S}  :  { "<<endl;
                    ReadTypeValue();
                    S();
                    emit("jp","_","_","");
                    sprintf(quad[counter-1].result,"%d",label1);
                    sprintf(quad[label2].result,"%d",counter);
                    if(strcmp(symval, "}")==0){
                        outfile_1<<"S->while(J){S}  :  } "<<endl;
                        ReadTypeValue();
                        S();
                    }
                    else {cerr<<"S 缺}"<<endl                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ;exit(0);}
                }
                else {cerr<<"S 缺{"<<endl;exit(0);}
            }
            else {cerr<<"S 缺)"<<endl;exit(0);}
        }
        else {cerr<<"S 缺("<<endl;exit(0);}
    }
    else if (strcmp(symval, "printf")==0){
        outfile_1<<"S->O   :识别"<<*symval<<endl;
        O();
        S();
    }
    else {cerr<<"S 缺左变量"<<endl;exit(0);}
}
void T(){
    if(strcmp(symval,",")==0){
        outfile_1<<"T->,CT   :,"<<endl;
        ReadTypeValue();
        
        string str=symval;
        C();
        T();
        if(flag==0) insert(str);
    }
    else if(strcmp(symval,"}")==0||strcmp(symval,"if")==0||strcmp(symval,"{")==0||
            ('a'<=*symval&&*symval<='z')||strcmp(symval,";")==0||strcmp(symval,")")==0)
    {outfile_1<<"T->null"<<endl;}
    else {cerr<<"T 缺,"<<endl;exit(0);}
}
void C(){
    if(sym==12){
        if(flag==1){emit("out","_","_",symval);}
        outfile_1<<"C->(a|b|...|z)R   :识别"<<*symval<<endl;
        if(strlen(symval)>1) symval++;
        else ReadTypeValue();
        R();
    }
    else {cerr<<"C 缺a|b|...|z"<<endl;exit(0);}
}
void R(){
    if('a'<=*symval&&*symval<='z'){
        outfile_1<<"R->(a|b|...|z)R   :识别"<<*symval<<endl;
        if(strlen(symval)>1) symval++;
        else ReadTypeValue();
        R();
    }
    else if(strcmp(symval,",")==0||strcmp(symval,"}")==0||strcmp(symval,"if")==0||
            strcmp(symval,"{")==0||('a'<=*symval&&*symval<='z')||strcmp(symval,"*")==0||
            strcmp(symval,";")==0||strcmp(symval,"=")==0||strcmp(symval,"!=")==0||
            strcmp(symval,">=")==0||strcmp(symval,"<=")==0||strcmp(symval,">")==0||
            strcmp(symval,"<")==0||strcmp(symval,"==")==0||strcmp(symval,"/")==0||
            strcmp(symval,"+")==0||strcmp(symval,"-")==0||strcmp(symval,"++")==0||
            strcmp(symval,")")==0){
        outfile_1<<"R->null"<<endl;
    }
    else {cerr<<"R 缺左变量"<<endl;exit(0);}
}
char* E(){
    char* p=(char*)malloc(R4);
    char* q=(char*)malloc(R4);
    char* t=(char*)malloc(R4);
    char op;
    char *opp=(char*)malloc(R4);
    outfile_1<<"E->AE'"<<endl;
    p=A();
    op=*symval;
    *opp=op;*(opp+1)='\0';
    q=E1();
    if(q==NULL) return p;
    else{
        t=newtemp();
        emit(opp,p,q,t);
        return t;
    }
}
char* E1(){
    char* p=(char*)malloc(R4);
    char* q=(char*)malloc(R4);
    char* t=(char*)malloc(R4);
    char op;
    if(strcmp(symval,"+")==0){
        outfile_1<<"E'->+AE'   :+"<<endl;
        ReadTypeValue();
        p=A();
        op=*symval;
        q=E1();
        if(q==NULL) return p;
        else {
            t=newtemp();
            if(op=='+') emit("+",p,q,t);
            else if(op=='-') emit("-",p,q,t);
            return t;
        }
    }
    else if(strcmp(symval,"-")==0){
        outfile_1<<"E'->-AE'   :-"<<endl;
        ReadTypeValue();
        p=A();
        q=E1();
        if(q==NULL) return p;
        else {
            t=newtemp();
            if(op=='+') emit("+",p,q,t);
            else if(op=='-') emit("-",p,q,t);
            return t;
        }
    }
    else if(strcmp(symval,";")==0){
        outfile_1<<"E'->null"<<endl;
        return NULL;
    }
    else {cerr<<"E' 缺左变量"<<endl;exit(0);}
}
char* A(){
    char* p=(char*)malloc(R4);
    char* q=(char*)malloc(R4);
    char* t=(char*)malloc(R4);
    char op;
    char* opp=(char*)malloc(R4);
    if((*symval>='a'&&*symval<='z')||(*symval>='0'&&*symval<='9')){
        outfile_1<<"A->BA'   :识别"<<*symval<<endl;
        p=B();
        op=*symval;
        *opp=op;*(opp+1)='\0';
        q=A1();
        if(q==NULL) return p;
        else{
            t=newtemp();
            emit(opp,p,q,t);
            return t;
        }
    }
    
    else {cerr<<"A 缺左变量"<<endl;exit(0);}
}
char* A1(){
    char* p=(char*)malloc(R4);
    char* q=(char*)malloc(R4);
    char* t=(char*)malloc(R4);
    char op;
    if(strcmp(symval,"*")==0){
        outfile_1<<"A'->*B   :*"<<endl;
        ReadTypeValue();
        p=B();
        op=*symval;
        q=A1();
        if(q==NULL) return p;
        else {
            t=newtemp();
            if(op=='*') emit("*",p,q,t);
            else if(op=='/') emit("/",p,q,t);
            return t;
        }
    }
    else if(strcmp(symval,"/")==0){
        outfile_1<<"A'->/B   :/"<<endl;
        ReadTypeValue();
        p=B();
        q=A1();
        if(q==NULL) return p;
        else{
            t=newtemp();
            if(op=='*') emit("*",p,q,t);
            else if(op=='/') emit("/",p,q,t);
            return t;
        }
    }
    else if(strcmp(symval,"+")==0||strcmp(symval,"-")==0||strcmp(symval,";")==0){
        outfile_1<<"A'->null"<<endl;
        return NULL;
    }
    else {cerr<<"A' 缺左变量"<<endl;exit(0);}
}
char* B(){
    char *p=(char*)malloc(R4);
    if(sym==12){//说明是标志符
        strcpy(p,symval);
        if(!lookup(symval)){cerr<<symval<<"变量未定义"<<endl;exit(0);}
        outfile_1<<"B->C   :识别"<<*symval<<endl;
        C();
        return p;
    }
    else if(sym==13){//说明是数字
        strcpy(p,symval);
        outfile_1<<"B->I   :识别"<<*symval<<endl;
        I();
        return p;
    }
    else {cerr<<"B 缺左变量"<<endl;exit(0);}
}
void J(){
    char* p=(char*)malloc(R4);
    char* q=(char*)malloc(R4);
    char* judge=(char*)malloc(R4);
    strcpy(judge,"j");
    char* jump=(char*)malloc(R4);
    if((*symval>='a'&&*symval<='z')||(*symval>='0'&&*symval<='9')){
        outfile_1<<"J->BM   :识别"<<*symval<<endl;
        p=B();
        strcat(judge,symval);
        q=M();
        sprintf(jump,"%d",counter+2);
        emit(judge,p,q,jump);
    }
    else {cerr<<"J 缺左变量"<<endl;exit(0);}
}
void I(){
    if(*symval>='0'&&*symval<='9'){
        outfile_1<<"I->(0|1|...|9)I   :识别"<<*symval<<endl;
        if(strlen(symval)>1) symval++;
        else ReadTypeValue();
        I1();
    }
    else {cerr<<"I 缺左变量"<<endl;exit(0);}
}
void I1(){
    if(*symval>='0'&&*symval<='9'){
        outfile_1<<"I'->(0|1|...|9)I'   :识别"<<*symval<<endl;
        if(strlen(symval)>1) symval++;
        else ReadTypeValue();
        I1();
    }
    else if(strcmp(symval,"+")==0||strcmp(symval,"-")==0||(*symval>='a'&&*symval<='z')||
            (*symval>='0'&&*symval<='9')||strcmp(symval,"!=")==0||strcmp(symval,">=")==0||
            strcmp(symval,"<=")==0||strcmp(symval,">")==0||strcmp(symval,"<")==0||
            strcmp(symval,"==")==0||strcmp(symval,";")==0||strcmp(symval,")")==0){
        outfile_1<<"I'->null"<<endl;
    }
    else {cerr<<"I' 缺左变量"<<endl;exit(0);}
}
char* M(){
    char* p=(char*)malloc(R4);
    if(strcmp(symval,"!=")==0||strcmp(symval,">=")==0||strcmp(symval,"<=")==0
       ||strcmp(symval,">")==0||strcmp(symval,"<")==0||strcmp(symval,"==")==0){
        outfile_1<<"M->"<<symval<<"B   :"<<symval<<endl;
        ReadTypeValue();
        p=B();
        return p;
    }
    else {cerr<<"M 缺左变量"<<endl;exit(0);}
}
void N(){
    if(strcmp(symval,"else")==0){
        outfile_1<<"N->else {S}   :else"<<endl;
        ReadTypeValue();
        if(strcmp(symval,"{")==0){
            outfile_1<<"N->else {S}   :{"<<endl;
            ReadTypeValue();
            S();
            if(strcmp(symval,"}")==0)
            { outfile_1<<"N->else {S}   :}"<<endl;ReadTypeValue();}
            else{cerr<<"N 缺}"<<endl;exit(0);}
        }
        else {cerr<<"N 缺{"<<endl;exit(0);}
    }
   
    else if(strcmp(symval,"if")==0||strcmp(symval,"{")==0||(*symval>='a'&&*symval<='z')||
            strcmp(symval,"}")==0||strcmp(symval,"else")==0||strcmp(symval,";")==0){
        outfile_1<<"N->null"<<endl;
    }
    else {cerr<<"N 缺左变量"<<endl;exit(0);}
}
void O(){
    outfile_1<<"O->printf(\"F\",CT);   :printf"<<endl;
    ReadTypeValue();
    if(strcmp(symval,"(")==0){
        outfile_1<<"O->printf(\"F\",CT);   :("<<endl;
        ReadTypeValue();
        if(strcmp(symval,"\"")==0){
            outfile_1<<"O->printf(\"F\",CT);   :\""<<endl;
            ReadTypeValue();
            F();
            if(strcmp(symval, "\"")==0){
                outfile_1<<"O->printf(\"F\",CT);   :\""<<endl;
                ReadTypeValue();
                if(strcmp(symval, ",")==0){
                    outfile_1<<"O->printf(\"F\",CT);   :,"<<endl;
                    ReadTypeValue();
                    flag=1;
                    C();
                    T();
                    if(strcmp(symval, ")")==0){
                        outfile_1<<"O->printf(\"F\",CT);   :)"<<endl;
                        ReadTypeValue();
                        if(strcmp(symval, ";")==0){
                            outfile_1<<"O->printf(\"F\",CT);   :;"<<endl;
                            ReadTypeValue();
                        }
                        else {cerr<<"O 缺;"<<endl;exit(0);}
                    }
                    else {cerr<<"O 缺)"<<endl;exit(0);}
                }
                else {cerr<<"O 缺,"<<endl;exit(0);}
            }
            else {cerr<<"O 缺\""<<endl;exit(0);}
        }
        else {cerr<<"O 缺\""<<endl;exit(0);}
    }
    else {cerr<<"O 缺("<<endl;exit(0);}
    flag=0;
}
void F(){
    if(strcmp(symval, "%d")==0){
        outfile_1<<"F->%dF'   :%d"<<endl;
        ReadTypeValue();
        F1();
    }
    else {cerr<<"F 缺%d"<<endl;exit(0);}
}
void F1(){
    if(strcmp(symval, ",")==0){
        outfile_1<<"F'->,%dF'  :,"<<endl;
        ReadTypeValue();
        if(strcmp(symval, "%d")==0){
            outfile_1<<"F'->,%dF'   :%d"<<endl;
            ReadTypeValue();
            F1();
        }
        else {cerr<<"F' 缺%d"<<endl;exit(0);}
    }
    else if(strcmp(symval, "\"")==0){
        outfile_1<<"F'->null"<<endl;
    }
    else {cerr<<"F' 缺,"<<endl;exit(0);}
}