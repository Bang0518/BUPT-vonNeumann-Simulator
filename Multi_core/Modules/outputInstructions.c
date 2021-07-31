/***********************************************************************
 * File Name: outputInstructions.c 
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

void outputInstructions(const char tmp1[], const char tmp2[], short int data[]); //输出代码段、数据段

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