/***********************************************************************
 * File Name: global.h 
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
#ifndef ROSHI_variables
#define ROSHI_variables
#define maxLen 199999                                     //最大数组长度
#define tickets 100                                       //需要卖的票数
HANDLE hMutex, hThread1, hThread2, IOMutex;               //互斥对象、线程1、线程2、输入输出锁
int ip1 = 0, flag1 = 0, ir1 = 0, id1 = 1;                 //核心1：ip、flag、ir、id
int ip2 = 0, flag2 = 0, ir2 = 0, id2 = 2;                 //核心2：ip、flag、ir、id
int *ipp1 = &ip1, *flagp1 = &flag1, *irp1 = &ir1;         //核心1指针：对应ip、flag、ir、id
int *ipp2 = &ip2, *flagp2 = &flag2, *irp = &ir2;          //核心2指针：对应ip、flag、ir、id
short int data[maxLen] = {0}, ax1[9] = {0}, ax2[9] = {0}; //数据数组、寄存器数组
char tmp1[maxLen], tmp2[maxLen];                          //存指令的数组（核心1、核心2）
#endif