/***********************************************************************
 * File Name: main.c 
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
#include "getInstructions.c"
#include "analyseInstructions.c"
#include "outputInstructions.c"

#define maxLen 199999
int main(int argc, char const *argv[])
{
    int ip = 0, flag = 0, ir = 0;
    int *ipp = &ip, *flagp = &flag, *irp = &ir;
    short int data[maxLen] = {0}, ax[9] = {0}; //数据数组、寄存器数组
    char tmp[maxLen];                          //读指令数组
    //    FILE *fp = fopen("E:/cpp/BUPT-von-Neumann-CPUsimulation-master/Single-core/dict.dic", "r");
    FILE *fp = fopen("dict.dic", "r");
    if (fp == NULL)
    {
        printf("打开文件失败！\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        getInstructions(tmp, fp);
        analyseInstructions(tmp, ax, ipp, ir, flagp, data);
        outputInstructions(tmp, data);
    }
    fclose(fp);
    return 0;
}
