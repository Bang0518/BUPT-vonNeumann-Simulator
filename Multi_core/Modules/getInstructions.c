/***********************************************************************
 * File Name: getInstructions.c 
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
void getInstructions(char tmp[], FILE *fp); //读取指令
void getInstructions(char tmp[], FILE *fp)
{
    int i;
    for (i = 0; !feof(fp); i++)
    {
        tmp[i] = fgetc(fp);
    }
    tmp[i] = NULL; //防止产生野指针
}