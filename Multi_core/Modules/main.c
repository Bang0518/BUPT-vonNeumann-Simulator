/***********************************************************************
 * File Name: main.c 
 * Code Version: 1.47 
 * Author: Jianjun.WU 
 * Date: 2021-05-20 
 * Gcc Version: 8.1.0 (x86_64-posix-seh-rev0, Built by MinGW-W64 project)
 * # 系统版本: Windows 10 专业版 20H2 
 * # 编辑器: Visual Studio Code 
 * # 编程语言: C
 * ========== ======= ========= =======================================
 * @Copyright (C)  2021  all right reserved
***********************************************************************/
#ifndef _ROSHI_H
#define _ROSHI_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <process.h>
#include <windows.h>
#include <time.h>
#endif
#include "global.h"
#include "getInstructions.c"
#include "analyseInstructions.c"
#include "outputInstructions.c"

unsigned __stdcall Fun1Proc(void *pArguments);                //线程1调用的函数
unsigned __stdcall Fun2Proc(void *pArguments);                //线程2调用的函数
int transBase(const char tmp[], int start, int end, int tag); //二进制转换十进制函数

int main(int argc, char const *argv[])
{
    FILE *fp1 = fopen("dict1.dic", "r");
    FILE *fp2 = fopen("dict2.dic", "r");
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

unsigned __stdcall Fun1Proc(void *pArguments)
{
    analyseInstructions(tmp1, ax1, ipp1, ir1, flagp1, data, id1, hMutex); //分析指令
    return 0;
}
unsigned __stdcall Fun2Proc(void *pArguments)
{
    analyseInstructions(tmp2, ax2, ipp2, ir2, flagp2, data, id2, hMutex); //分析指令
    return 0;
}
