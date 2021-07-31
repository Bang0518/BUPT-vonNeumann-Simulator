/***********************************************************************
 * File Name: Multi_final.c 
 * Code Version: 1.47 
 * Author: Jianjun.WU 
 * Date: 2021-05-20 
 * Gcc Version: 8.1.0 (x86_64-posix-seh-rev0, Built by MinGW-W64 project)
 * # 系统版本: Windows 10 专业版 20H2 
 * # 编辑器: Visual Studio Code 
 * # 编程语言: C
 * ========== ======= ========= =======================================
 * Copyright © WuJianjun 2021 All rights reserved
***********************************************************************/
//头文件
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <process.h>
#include <windows.h>
#include <time.h>
#include <pthread.h>
//全局定义部分
#define maxLen 199999                                     //最大数组长度
#define tickets 100                                       //需要卖的票数
HANDLE hMutex, hThread1, hThread2, IOMutex;               //互斥对象、线程1、线程2、输入输出锁
int ip1 = 0, flag1 = 0, ir1 = 0, id1 = 1;                 //核心1：ip、flag、ir、id
int ip2 = 0, flag2 = 0, ir2 = 0, id2 = 2;                 //核心2：ip、flag、ir、id
int *ipp1 = &ip1, *flagp1 = &flag1, *irp1 = &ir1;         //核心1指针：对应ip、flag、ir、id
int *ipp2 = &ip2, *flagp2 = &flag2, *irp = &ir2;          //核心2指针：对应ip、flag、ir、id
short int data[maxLen] = {0}, ax1[9] = {0}, ax2[9] = {0}; //数据数组、寄存器数组1、寄存器数组2
char tmp1[maxLen], tmp2[maxLen];                          //存指令的数组（核心1、核心2）
//函数声明部分
void getInstructions(char tmp[], FILE *fp);                                                                                                          //读取指令
void analyseInstructions(const char tmp[], short int ax[], int *ipp, int ir, int *flagp, short int data[], int id, HANDLE hMutex);                   //分析指令
void outputInstructions(const char tmp1[], const char tmp2[], short int data[]);                                                                     //输出代码段、数据段
int transBase(const char tmp[], int start, int end, int tag);                                                                                        //二进制转换十进制
void decideWork(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *ipp, int *flagp, int id, HANDLE hMutex); //任务分工
void taskConvey(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[]);                                              //数据传送指令
void taskCalculate(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[]);                                           //算数运算指令
void taskBool(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[]);                                                //逻辑运算指令
void taskCompare(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *flagp);                                 //比较运算指令
void taskFlash(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *ipp, int *flagp);                         //跳转指令
void taskIO(int operaNum, int frontNum, short int ax[], short int data[], int id);                                                                   //输入输出指令
void putUpdate(short int ax[], int *ipp, const int *flagp, int ir, int id);                                                                          //输出各个寄存器状态
unsigned __stdcall Fun1Proc(void *pArguments);                                                                                                       //线程1调用的函数
unsigned __stdcall Fun2Proc(void *pArguments);                                                                                                       //线程2调用的函数
int judgeStart(int num, int *ipp, int tag);                                                                                                          //判断开始翻译的位置
//**************************************  main函数部分  **************************************//
int main(int argc, char const *argv[])
{
    FILE *fp1 = fopen("E:/cpp/BUPT-von-Neumann-CPUsimulation-master/Multi-core/dict1.dic", "r");
    FILE *fp2 = fopen("E:/cpp/BUPT-von-Neumann-CPUsimulation-master/Multi-core/dict2.dic", "r");
    // FILE *fp1 = fopen("dict1.dic", "r"); //打开dict1.dic
    // FILE *fp2 = fopen("dict2.dic", "r"); //打开dict2.dic
    if ((fp1 == NULL) || (fp2 == NULL))
    {
        printf("打开文件失败！\n");
        exit(EXIT_FAILURE);
    }
    getInstructions(tmp1, fp1);                                          //读取指令到核心1的tmp1[]
    getInstructions(tmp2, fp2);                                          //读取指令到核心2的tmp2[]
    data[16384] = tickets;                                               //根据要求初始化
    hMutex = CreateMutex(NULL, FALSE, NULL);                             //进程锁
    IOMutex = CreateMutex(NULL, FALSE, NULL);                            //卡输入输出的锁
    hThread1 = (HANDLE)_beginthreadex(NULL, 0, Fun1Proc, NULL, 0, NULL); //核心1调用的函数
    hThread2 = (HANDLE)_beginthreadex(NULL, 0, Fun2Proc, NULL, 0, NULL); //核心2调用的函数
    WaitForSingleObject(hThread1, INFINITE);                             //等待线程1结束
    CloseHandle(hThread1);                                               //关闭线程1
    WaitForSingleObject(hThread2, INFINITE);                             //等待线程2结束
    CloseHandle(hThread2);                                               //关闭线程2
    data[16384] = 0;                                                     //根据要求置零
    outputInstructions(tmp1, tmp2, data);                                //输出代码段、数据段
    fclose(fp1);                                                         //关闭文件
    fclose(fp2);                                                         //关闭文件
    _endthreadex(0);                                                     //结束进程
    return 0;                                                            //返回0
}

//函数名称：线程1调用的函数
//作用：线程1分析指令
//返回值：0
unsigned __stdcall Fun1Proc(void *pArguments)
{
    analyseInstructions(tmp1, ax1, ipp1, ir1, flagp1, data, id1, hMutex); //分析指令
    return 0;
}

//函数名称：线程2调用的函数
//作用：线程2分析指令
//返回值：0
unsigned __stdcall Fun2Proc(void *pArguments)
{
    analyseInstructions(tmp2, ax2, ipp2, ir2, flagp2, data, id2, hMutex); //分析指令
    return 0;
}

//函数名称：取指令函数
//作用：从dict.dic里读取指令，存储到tmp数组中
//返回值：无
void getInstructions(char tmp[], FILE *fp)
{
    int i;
    for (i = 0; !feof(fp); i++)
    {
        tmp[i] = fgetc(fp);
    }
    tmp[i] = '\0'; //防止产生野指针
}

//函数名称：二进制转十进制函数
//作用：将二进制数转换为十进制，涉及原码和补码的转化
//返回值：所得十进制数
int transBase(const char tmp[], int start, int end, int tag)
{
    int i = 0, a = 1, sum = 0;
    //原码转化
    for (i = end - 1; i >= start - 1; i--)
    {
        sum = sum + (tmp[i] - '0') * a;
        a = a <<= 1;
    }
    //判断补码，进行转化
    if (tag == 1)
    {
        if (sum < pow(2, 15))
        {
            sum = sum;
        }
        else
        {
            sum -= pow(2, 16);
        }
    }
    return sum; //返回结果
}

//函数名称：分析指令函数
//作用：分析指令，决定对应的操作，输出更新状态
//返回值：无
void analyseInstructions(const char tmp[], short int ax[], int *ipp, int ir, int *flagp, short int data[], int id, HANDLE hMutex)
{
    int operaNum = 0, frontNum = 0, backNum = 0, imeNum = 0, start_operaNum = 0, start_frontNum = 0, start_backNum = 0, start_imeNum = 0, f = 1;
    while (f)
    {
        start_operaNum = judgeStart(start_operaNum, ipp, 1); //操作码
        operaNum = transBase(tmp, start_operaNum + 1, start_operaNum + 8, 0);
        start_frontNum = judgeStart(start_frontNum, ipp, 2); //前对象
        frontNum = transBase(tmp, start_frontNum + 1, start_frontNum + 4, 0);
        start_backNum = judgeStart(start_backNum, ipp, 3); //后对象
        backNum = transBase(tmp, start_backNum + 1, start_backNum + 4, 0);
        start_imeNum = judgeStart(start_imeNum, ipp, 4); //立即数
        imeNum = transBase(tmp, start_imeNum + 1, start_imeNum + 16, 1);
        if (operaNum == 0)
        {
            f = 0;
        }
        (*ipp) += 4;
        ir = transBase(tmp, start_operaNum + 1, start_operaNum + 16, 0);
        decideWork(operaNum, frontNum, backNum, imeNum, ax, data, ipp, flagp, id, hMutex); //分工函数
        putUpdate(ax, ipp, flagp, ir, id);                                                 //输出寄存器状态
    }
}

//函数名称：输出指令函数
//作用：程序的最后输出代码段和数据段
//返回值：无
void outputInstructions(const char tmp1[], const char tmp2[], short int data[])
{
    int ip = 0, countRow1 = 0, codeNum1 = 1, countRow2 = 0, codeNum2 = 1, start = 0, i = 0;
    //代码段
    printf("\n");
    printf("codeSegment :\n");
    while (codeNum1)
    {
        start = ip / 4 * 33;
        codeNum1 = transBase(tmp1, start + 1, start + 32, 0);
        if ((countRow1 + 1) % 8 == 0)
        {
            printf("%d", codeNum1);
            printf("\n");
        }
        else
        {
            printf("%d ", codeNum1);
        }
        ip += 4;
        countRow1++;
    }
    for (i = 8 * 8 - countRow1; i > 0; i--)
    {
        if ((i - 1) % 8 == 0)
        {
            printf("0");
            printf("\n");
        }
        else
        {
            printf("0 ");
        }
        countRow1++;
    }
    ip = 0;
    while (codeNum2)
    {
        start = ip / 4 * 33;
        codeNum2 = transBase(tmp2, start + 1, start + 32, 0);
        if ((countRow2 + 1) % 8 == 0)
        {
            printf("%d", codeNum2);
            printf("\n");
        }
        else
        {
            printf("%d ", codeNum2);
        }
        ip += 4;
        countRow2++;
    }
    for (i = 8 * 8 - countRow2; i > 0; i--)
    {
        if ((i - 1) % 8 == 0)
        {
            printf("0");
            printf("\n");
        }
        else
        {
            printf("0 ");
        }
        countRow2++;
    }
    //数据段
    printf("\n");
    printf("dataSegment :\n");
    for (i = 0; i < pow(2, 9); i += 2)
    {
        if (i == pow(2, 9) - 2)
        {
            printf("%hd", data[i + 16384]);
        }
        else if ((i + 2) % 32 == 0 && i != 0)
        {
            printf("%hd\n", data[i + 16384]);
        }
        else
        {
            printf("%hd ", data[i + 16384]);
        }
    }
}

//函数名称：分工函数
//作用：根据传入的操作码决定操作
//返回值：无
void decideWork(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *ipp, int *flagp, int id, HANDLE hMutex)
{
    switch (operaNum)
    {
    case 1:
        taskConvey(operaNum, frontNum, backNum, imeNum, ax, data); //数据传送指令
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        taskCalculate(operaNum, frontNum, backNum, imeNum, ax, data); //算数运算指令
        break;
    case 6:
    case 7:
    case 8:
        taskBool(operaNum, frontNum, backNum, imeNum, ax, data); //逻辑运算指令
        break;
    case 9:
        taskCompare(operaNum, frontNum, backNum, imeNum, ax, data, flagp); //比较指令
        break;
    case 10:
        taskFlash(operaNum, frontNum, backNum, imeNum, ax, data, ipp, flagp); //跳转指令
        break;
    case 11:
    case 12:
        taskIO(operaNum, frontNum, ax, data, id); //输入输出指令
        break;
    case 13:
        WaitForSingleObject(hMutex, INFINITE); //锁内存指令
        break;
    case 14:
        ReleaseMutex(hMutex); //释放内存指令
        break;
    case 15:
        Sleep(imeNum); //休眠指令
        break;

    default:
        break;
    }
}

//函数名称：数据传送函数
//作用：进行数据传送
//返回值：无
void taskConvey(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[])
{
    int record;
    if (backNum == 0)
    {
        ax[frontNum] = imeNum;
    }
    else
    {
        if (frontNum <= 4 && backNum >= 5)
        {
            record = ax[backNum];
            ax[frontNum] = data[record];
        }
        if (frontNum >= 5 && backNum <= 4)
        {
            record = ax[frontNum];
            data[record] = ax[backNum];
        }
        if (frontNum <= 4 && backNum <= 4)
        {
            ax[frontNum] = ax[backNum];
        }
        if (backNum >= 5 && frontNum >= 5)
        {
            data[ax[frontNum]] = data[ax[backNum]];
        }
    }
}

//函数名称：算数计算函数
//作用：进行算数计算
//返回值：无
void taskCalculate(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[])
{
    if (backNum == 0)
    {
        switch (operaNum)
        {
        case 2:
            if (frontNum <= 4)
            {
                ax[frontNum] += imeNum;
            }
            else if (frontNum >= 5)
            {
                data[ax[frontNum]] += imeNum;
            }
            break;
        case 3:
            if (frontNum <= 4)
            {
                ax[frontNum] -= imeNum;
            }
            else if (frontNum >= 5)
            {
                data[ax[frontNum]] -= imeNum;
            }
            break;
        case 4:
            if (frontNum <= 4)
            {
                ax[frontNum] *= imeNum;
            }
            else if (frontNum >= 5)
            {
                data[ax[frontNum]] *= imeNum;
            }
            break;
        case 5:
            if (frontNum <= 4)
            {
                ax[frontNum] /= imeNum;
            }
            else if (frontNum >= 5)
            {
                data[ax[frontNum]] /= imeNum;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (operaNum)
        {
        case 2:
            if (frontNum <= 4 && backNum <= 4)
            {
                ax[frontNum] += ax[backNum];
            }
            else if (frontNum <= 4 && backNum >= 5)
            {
                ax[frontNum] += data[ax[backNum]];
            }
            else if (frontNum >= 5 && backNum <= 4)
            {
                data[ax[frontNum]] += ax[backNum];
            }
            else if (frontNum >= 5 && backNum >= 5)
            {
                data[ax[frontNum]] += data[ax[backNum]];
            }
            break;
        case 3:
            if (frontNum <= 4 && backNum <= 4)
            {
                ax[frontNum] -= ax[backNum];
            }
            else if (frontNum <= 4 && backNum >= 5)
            {
                ax[frontNum] -= data[ax[backNum]];
            }
            else if (frontNum >= 5 && backNum <= 4)
            {
                data[ax[frontNum]] -= ax[backNum];
            }
            else if (frontNum >= 5 && backNum >= 5)
            {
                data[ax[frontNum]] -= data[ax[backNum]];
            }
            break;
        case 4:
            if (frontNum <= 4 && backNum <= 4)
            {
                ax[frontNum] *= ax[backNum];
            }
            else if (frontNum <= 4 && backNum >= 5)
            {
                ax[frontNum] *= data[ax[backNum]];
            }
            else if (frontNum >= 5 && backNum <= 4)
            {
                data[ax[frontNum]] *= ax[backNum];
            }
            else if (frontNum >= 5 && backNum >= 5)
            {
                data[ax[frontNum]] *= data[ax[backNum]];
            }
            break;
        case 5:
            if (frontNum <= 4 && backNum <= 4)
            {
                ax[frontNum] /= ax[backNum];
            }
            else if (frontNum <= 4 && backNum >= 5)
            {
                ax[frontNum] /= data[ax[backNum]];
            }
            else if (frontNum >= 5 && backNum <= 4)
            {
                data[ax[frontNum]] /= ax[backNum];
            }
            else if (frontNum >= 5 && backNum >= 5)
            {
                data[ax[frontNum]] /= data[ax[backNum]];
            }
            break;

        default:
            break;
        }
    }
}

//函数名称：逻辑运算函数
//作用：进行逻辑运算
//返回值：无
void taskBool(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[])
{
    if (frontNum != 0 && backNum == 0)
    {
        switch (operaNum)
        {
        case 6:
            if (frontNum <= 4)
            {
                if (ax[frontNum] && imeNum)
                {
                    ax[frontNum] = 1;
                }
                else
                {
                    ax[frontNum] = 0;
                }
            }
            else if (frontNum >= 5)
            {
                if (data[ax[frontNum]] && imeNum)
                {
                    data[ax[frontNum]] = 1;
                }
                else
                {
                    data[ax[frontNum]] = 0;
                }
            }
            break;
        case 7:
            if (frontNum <= 4)
            {
                if (ax[frontNum] || imeNum)
                {
                    ax[frontNum] = 1;
                }
                else
                {
                    ax[frontNum] = 0;
                }
            }
            else if (frontNum >= 5)
            {
                if (data[ax[frontNum]] || imeNum)
                {
                    data[ax[frontNum]] = 1;
                }
                else
                {
                    data[ax[frontNum]] = 0;
                }
            }
            break;
        case 8:
            if (frontNum <= 4)
            {
                ax[frontNum] = !ax[frontNum];
            }
            else if (frontNum >= 5)
            {
                data[ax[frontNum]] = !data[ax[frontNum]];
            }
            break;

        default:
            break;
        }
    }
    else if (frontNum == 0 && backNum != 0)
    {
        if (backNum <= 4)
        {
            ax[backNum] = !ax[backNum];
        }
        else if (backNum >= 5)
        {
            data[ax[backNum]] = !data[ax[backNum]];
        }
    }
    else if (frontNum != 0 && backNum != 0)
    {
        switch (operaNum)
        {
        case 6:
            if (frontNum <= 4 && backNum <= 4)
            {
                if (ax[frontNum] && ax[backNum])
                {
                    ax[frontNum] = 1;
                }
                else
                {
                    ax[frontNum] = 0;
                }
            }
            else if (frontNum <= 4 && backNum >= 5)
            {
                if (ax[frontNum] && data[ax[backNum]])
                {
                    ax[frontNum] = 1;
                }
                else
                {
                    ax[frontNum] = 0;
                }
            }
            else if (frontNum >= 5 && backNum <= 4)
            {
                if (data[ax[frontNum]] && ax[backNum])
                {
                    data[ax[frontNum]] = 1;
                }
                else
                {
                    data[ax[frontNum]] = 0;
                }
            }
            else if (frontNum >= 5 && backNum >= 5)
            {
                if (data[ax[frontNum]] && data[ax[backNum]])
                {
                    data[ax[frontNum]] = 1;
                }
                else
                {
                    data[ax[frontNum]] = 0;
                }
            }
            break;
        case 7:
            if (frontNum <= 4 && backNum <= 4)
            {
                if (ax[frontNum] || ax[backNum])
                {
                    ax[frontNum] = 1;
                }
                else
                {
                    ax[frontNum] = 0;
                }
            }
            else if (frontNum <= 4 && backNum >= 5)
            {
                if (ax[frontNum] || data[ax[backNum]])
                {
                    ax[frontNum] = 1;
                }
                else
                {
                    ax[frontNum] = 0;
                }
            }
            else if (frontNum >= 5 && backNum <= 4)
            {
                if (data[ax[frontNum]] || ax[backNum])
                {
                    data[ax[frontNum]] = 1;
                }
                else
                {
                    data[ax[frontNum]] = 0;
                }
            }
            else if (frontNum >= 5 && backNum >= 5)
            {
                if (data[ax[frontNum]] || data[ax[backNum]])
                {
                    data[ax[frontNum]] = 1;
                }
                else
                {
                    data[ax[frontNum]] = 0;
                }
            }
            break;
        default:
            break;
        }
    }
}

//函数名称：比较函数
//作用：进行比较指令
//返回值：无
void taskCompare(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *flagp)
{
    if (backNum == 0)
    {
        if (frontNum <= 4)
        {
            if (ax[frontNum] == imeNum)
            {
                *flagp = 0;
            }
            else if (ax[frontNum] > imeNum)
            {
                *flagp = 1;
            }
            else if (ax[frontNum] < imeNum)
            {
                *flagp = -1;
            }
        }
        else if (frontNum >= 5)
        {
            if (data[ax[frontNum]] == imeNum)
            {
                *flagp = 0;
            }
            else if (data[ax[frontNum]] > imeNum)
            {
                *flagp = 1;
            }
            else if (data[ax[frontNum]] < imeNum)
            {
                *flagp = -1;
            }
        }
    }
    else
    {
        if (frontNum <= 4 && backNum <= 4)
        {
            if (ax[frontNum] == ax[backNum])
            {
                *flagp = 0;
            }
            else if (ax[frontNum] > ax[backNum])
            {
                *flagp = 1;
            }
            else if (ax[frontNum] < ax[backNum])
            {
                *flagp = -1;
            }
        }
        else if (frontNum <= 4 && backNum >= 5)
        {
            if (ax[frontNum] == data[ax[backNum]])
            {
                *flagp = 0;
            }
            else if (ax[frontNum] > data[ax[backNum]])
            {
                *flagp = 1;
            }
            else if (ax[frontNum] < data[ax[backNum]])
            {
                *flagp = -1;
            }
        }
        else if (frontNum >= 5 && backNum <= 4)
        {
            if (data[ax[frontNum]] == ax[backNum])
            {
                *flagp = 0;
            }
            else if (data[ax[frontNum]] > ax[backNum])
            {
                *flagp = 1;
            }
            else if (data[ax[frontNum]] < ax[backNum])
            {
                *flagp = -1;
            }
        }
        else if (frontNum >= 5 && backNum >= 5)
        {
            if (data[ax[frontNum]] == data[ax[backNum]])
            {
                *flagp = 0;
            }
            else if (data[ax[frontNum]] > data[ax[backNum]])
            {
                *flagp = 1;
            }
            else if (data[ax[frontNum]] < data[ax[backNum]])
            {
                *flagp = -1;
            }
        }
    }
}

//函数名称：跳转函数
//作用：进行跳转指令
//返回值：无
void taskFlash(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *ipp, int *flagp)
{
    if (backNum == 0)
    {
        (*ipp) += imeNum - 4;
    }
    if (backNum == 1)
    {
        if ((*flagp) == 0)
        {
            (*ipp) += imeNum - 4;
        }
    }
    if (backNum == 2)
    {
        if ((*flagp) == 1)
        {
            (*ipp) += imeNum - 4;
        }
    }
    if (backNum == 3)
    {
        if ((*flagp) == -1)
        {
            (*ipp) += imeNum - 4;
        }
    }
}

//函数名称：输入输出函数
//作用：执行输入输出指令
//返回值：无
void taskIO(int operaNum, int frontNum, short int ax[], short int data[], int id)
{
    switch (operaNum)
    {
    case 11:
        printf("in:\n");
        if (frontNum <= 4)
        {
            scanf("%hd", &ax[frontNum]);
        }
        else if (frontNum >= 5)
        {
            scanf("%hd", &data[ax[frontNum]]);
        }
        break;
    case 12:
        if (frontNum <= 4)
        {
            printf("id = %d    out: %hd\n", id, ax[frontNum]);
        }
        else if (frontNum >= 5)
        {
            printf("id = %d    out: %hd\n", id, data[ax[frontNum]]);
        }
        break;
    default:
        break;
    }
}

//函数名称：输出更新状态函数
//作用：输出更新状态
//返回值：无
void putUpdate(short int ax[], int *ipp, const int *flagp, int ir, int id)
{
    int i;
    WaitForSingleObject(IOMutex, INFINITE);
    printf("\nid = %d\n", id);
    if (id == 1)
    {
        printf("ip = %d\n", *ipp);
    }
    else if (id == 2)
    {
        printf("ip = %d\n", *ipp + 256);
    }
    printf("flag = %d\n", *flagp);
    printf("ir = %d\n", ir);
    for (i = 1; i <= 4; i++)
    {
        if (i > 1)
        {
            printf(" ");
        }
        printf("ax%d = %hd", i, ax[i]);
    }
    printf("\n");
    for (i = 5; i <= 8; i++)
    {
        if (i > 5)
        {
            printf(" ");
        }
        printf("ax%d = %hd", i, ax[i]);
    }
    printf("\n");
    ReleaseMutex(IOMutex);
}

//函数名称：判断起始位置
//作用：根据传入的tag决定开始的位置
//返回值：对应的起始位置
int judgeStart(int num, int *ipp, int tag)
{
    if (tag == 1)
    {
        num = (*ipp) / 4 * 33; //操作码
    }
    if (tag == 2)
    {
        num = (*ipp) / 4 * 33 + 8; //前对象
    }
    if (tag == 3)
    {
        num = (*ipp) / 4 * 33 + 8 + 4; //后对象
    }
    if (tag == 4)
    {
        num = (*ipp) / 4 * 33 + 8 + 4 + 4; //立即数
    }
    return num;
}
