#include<vector>
#include<string>
#include<map>
#include<iostream>
#include <sstream>
#include <fstream>

using namespace std;

class explain
{
    ofstream outfile_2;
    vector<string> code;//存储代码区
    map<string,int*> symbol;
    int variable[10];//存储静态变量运行中在存储区分配的地址，假设程序中最多只能存10个静态变量
    string rc;//存当前运行的指令
    int pc;//指向下一条指令
    int run[20];//运行栈区，这块是动态存储区
    vector<string> va;
public:
    explain(){
        variable[0] = '\0';
        run[0] = '\0';
        pc = 0;
        rc = '\0';
        outfile_2.open("/Users/tsq0313/Desktop/quar.txt");
    }
    //void ini_code();//人为添加几句四元式
    // void expn();//针对四元式的解释程序
    //void store_add();//将静态存储区的地址存入符号表中
    //void copy_code(vector<string> codd);//codd为四元式，存储四元式进代码区
    //void copy_map(map<string,char*>);
    void vector_out()
    {
        vector<string>::iterator it;
        for(it=code.begin();it!=code.end();it++)
        {
            outfile_2<<*it<<endl;
        }
    }
    int operator == (string x)
    {
        int i;
        if(rc.length() == x.length())
        {
            for(i=0;i<rc.length();i++){
                if(rc[i]!=x[i]){
                    return 0;
                }
            }
            return 1;
        }
        return 0;
    }
    void get_var(string rc)//取四元式中的变量
    {
        va.clear();
        string id;
        int j=1,count=0,i;
        while(count <3)
        {
            while(rc[j]!=',')
            {
                id.push_back(rc[j]);
                j++;
            }
            va.push_back(id);
            id.clear();
            if(rc[j] == ',')
            {
                count++;
            }
            j++;
            if(j>rc.size())
                break;
        }
        id.clear();
        while(rc[j]!='>')
        {
            id.push_back(rc[j]);
            j++;
            if(j>rc.size())
                break;
        }
        va.push_back(id);
    };
    
    void copy_map(map<string,char*> sym)
    {
        int *add;
        map<string,char*>::iterator iter;
        map<string,int*>::iterator it;
        for(iter = sym.begin();iter!=sym.end();iter++)
        {
            add = new int(1);
            symbol.insert(pair<string,int*>(iter->first,add));
        }
    }
    
    //将一个容器复制到另一个容器
    void copy_code(vector<string> codd)
    {
        int i = 0;
        outfile_2<<"this is codd!"<<endl;
        vector<string>::iterator it;
        for(it=codd.begin();it!=codd.end();it++)
        {
            code.push_back(*it);
        }
        vector_out();
    }
    //往符号表里赋地址，赋运行中该变量对应的地址，地址存的是运行中变量的值
    void store_add()
    {
        int *p;
        p = variable;//任意赋初始值
        variable[0] = 1;
        variable[1] = 2;
        variable[2] = 5;
        variable[3] = 4;
        variable[4] = 6;
        map<string,int*>::iterator iter;
        for(iter = symbol.begin();iter!=symbol.end();iter++)
        {
            iter->second = p;
            p++;
        }
        for(iter = symbol.begin();iter!= symbol.end();iter++)
        {
            outfile_2<<"the symbol is:"<<iter->first<<endl;
            outfile_2<<"the symbol value is："<<*(iter->second)<<endl;
        }
    }
    //自己定义的四元式，和自己定义的符号表的执行
    void ini_code()
    {
        code.push_back("(=,2, ,a)");
        code.push_back("(=,4, ,b)");
        code.push_back("(+,a,b,t1)");
        code.push_back("(=,t1, , c)");
    }
    
    void expn()//只能进行单数字，单字符
    {
        
        int number;
        int j=0,i=0;
        string l_i;//算数表达式左边的标识符
        string id;//用来获取四元式第一个位置的符号
        string l_r;
        string st = "a";//用来将字符化为字符串
        int position;//存储变量地址
        map<string,int*>::iterator iter;
        int flag=0;
        while(pc<code.size()){
            flag=0;
            outfile_2<<"this is pc@@@@@@:"<<pc<<endl;
            rc = code[pc];
            get_var(rc);
            outfile_2<<"############################################"<<endl;
            outfile_2<<"this is rc:"<<rc<<endl;
            if(rc[0] == '<')
            {
                j=1;
                id.clear();
                id = va[0];
                outfile_2<<"this is id:"<<id<<endl;
                if(id == "="){
                    i=0;
                    l_i.clear();
                    l_i = va[3];
                    outfile_2<<"this is the left identifier:"<<l_i<<endl;
                    //            l_i.clear();
                    //            l_i = va[3];
                    iter = symbol.find(l_i);
                    if(iter!=symbol.end())
                    {
                        position = iter->second - variable;
                        outfile_2<<"this is the variable position:"<<position<<endl;
                        run[0] = position;//地址入栈
                        //                i++;
                        l_r = va[1];
                        outfile_2<<"this is l_r:"<<l_r<<endl;
                        if(l_r[0]>='0' && l_r[0]<='9')
                        {
                            stringstream ss;
                            ss<<l_r;
                            ss>>number;
                            //将转化后的数字压入栈顶地址
                        }else if(l_r[0] >= 'a' && l_r[0]<='z')//如果为标识符
                        {
                            iter = symbol.find(l_r);
                            if(iter != symbol.end())
                            {
                                number = *(iter->second);
                            }
                        }
                        variable[run[0]] = number;
                        outfile_2<<"变量"<<l_i<<"的值为："<<number<<endl;
                    }
                    //            symbol.find
                    //查符号表，取得a的地址，压入栈顶，符号左边的标识符
                    //如果是数字，直接赋值
                    //如果是标识符，则取其地址中的值压入左边的标识符中
                }else if(id == "+" || id =="-" || id == "*" ||id == "/"){
                    //将符号左边压入栈顶
                    //。。。类似
                    int num1,num2;
                    int value;
                    int count=0;//计算逗号的数目，判断左变量的标识符
                    l_i = va[3];
                    outfile_2<<"this is the left identifier haha###:"<<l_i<<endl;
                    iter = symbol.find(l_i);
                    if(iter!=symbol.end())
                    {
                        position = iter->second - variable;
                        outfile_2<<"this is the left variable position:"<<position<<endl;
                        run[0] = position;//地址入栈
                        string l_r;
                        //第一种情况：标识符+标识符//左边第一个标识符
                        j=0;
                        l_r = va[1];
                        for(j=0;j<2;j++){
                            if((l_r[0]>='a' && l_r[0]<='z') || (l_r[0]>='0' && l_r[0]<='9'))
                            {
                                outfile_2<<"this is l_r:"<<l_r<<endl;
                                if(l_r[0]>='a' && l_r[0]<='z')
                                {
                                    iter = symbol.find(l_r);
                                    if(iter!=symbol.end())
                                    {
                                        position = iter->second - variable;
                                        outfile_2<<"the right identifier value is:"<<variable[position]<<endl;
                                        if(j==0)
                                        {
                                            num1 = variable[position];
                                        }else if(j==1)
                                        {
                                            num2 = variable[position];
                                        }
                                    }
                                }else if(l_r[0]>='0' && l_r[0]<='9')
                                {
                                    stringstream ss;
                                    ss<<l_r;
                                    ss>>number;
                                    if(j==0)
                                    {
                                        num1 = number;
                                    }else if(j==1)
                                    {
                                        num2 = number;
                                    }
                                }
                                
                            }
                            l_r.clear();
                            l_r = va[2];
                        }
                        if(id=="+")
                            value = num1 + num2;
                        else if(id=="-")
                            value = num1 - num2;
                        else if(id=="*")
                            value = num1 * num2;
                        else if(id=="/")
                            value = num1 / num2;
                        outfile_2<<"this is value:"<<value<<endl;
                        outfile_2<<"this is the ###"<<id<<" ###value:"<<value<<endl;
                        variable[run[0]] = value;
                    }
                }else if(id == "j>" || id == "j<"){//跳转语句
                    string l_r1;
                    string l_r2;
                    int num1,num2;
                    l_r1 = va[1];
                    l_r2 = va[2];
                    if(l_r1[0]>='0' && l_r1[0]<='9')
                    {
                        stringstream ss;
                        ss<<l_r1;
                        ss>>num1;
                    }else if(l_r1[0]>='a' && l_r1[0]<='z'){
                        iter = symbol.find(l_r1);
                        if(iter!=symbol.end())
                        {
                            position = iter->second - variable;
                            outfile_2<<"the right identifier value is:"<<variable[position]<<endl;
                            num1 = variable[position];
                        }
                    }
                    if(l_r2[0]>='0' && l_r2[0]<='9')
                    {
                        stringstream ss;
                        ss<<l_r2;
                        ss>>num2;
                    }else if(l_r2[0]>='a' && l_r2[0]<='z'){
                        iter = symbol.find(l_r2);
                        if(iter!=symbol.end())
                        {
                            position = iter->second - variable;
                            outfile_2<<"the right identifier value is:"<<variable[position]<<endl;
                            num2 = variable[position];
                        }
                    }
                    outfile_2<<"this is num1,num2:"<<num1<<","<<num2<<endl;
                    if(id=="j>")
                    {
                        if(num1>num2)
                        {
                            stringstream ss;
                            ss<<va[3];
                            ss>>pc;
                            flag=1;
                        }
                    }else if(id=="j<")
                    {
                        if(num1<num2)
                        {
                            stringstream ss;
                            ss<<va[3];
                            ss>>pc;
                            flag=1;
                        }
                    }
                }else if(id == "jp"){
                    stringstream ss;
                    ss<<va[3];
                    ss>>pc;
                    flag=1;
                }else if(id == "out"){
                    string ss = va[3];
                    iter = symbol.find(ss);
                    if(iter!=symbol.end())
                    {
                        position = iter->second-variable;
                        outfile_2<<"this is the output: variable "<<ss<<" value is: "<<variable[position]<<endl;
                    }else{
                        outfile_2<<"符号表中无此变量！"<<endl;
                        exit(0);
                    }
                }
                if(flag==0)//无跳转语句直接加加
                    pc++;
            }
            
        }
    }
};

