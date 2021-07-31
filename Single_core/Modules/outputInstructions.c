/***********************************************************************
 * File Name: outputInstructions.c 
 * Code Version: 2.56 
 * Author: Jianjun.WU 
 * Date: 2021-05-16 
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
#endif

void outputInstructions(const char tmp[], short int data[]); //输出指令

void outputInstructions(const char tmp[], short int data[])
{
    int ip = 0, countRow = 0, codeNum = 1, start = 0, i = 0;
    //代码段
    printf("\n");
    printf("codeSegment :\n");
    while (codeNum)
    {
        start = ip / 4 * 33;
        codeNum = transBase(tmp, start + 1, start + 32, 0);
        if ((countRow + 1) % 8 == 0)
        {
            printf("%d", codeNum);
            printf("\n");
        }
        else
        {
            printf("%d ", codeNum);
        }
        ip += 4;
        countRow++;
    }
    for (i = 16 * 8 - countRow; i > 0; i--)
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
        countRow++;
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
