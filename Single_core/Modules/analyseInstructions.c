#ifndef _ROSHI_H
#define _ROSHI_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#endif

void analyseInstructions(const char tmp[], short int ax[], int *ipp, int ir, int *flagp, short int data[]);                   //分析指令
int transBase(const char tmp[], int start, int end, int tag);                                                                 //二进制转换十进制
void decideWork(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *ipp, int *flagp); //任务分工
void taskConvey(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[]);                       //数据传送指令
void taskCalculate(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[]);                    //算数运算指令
void taskBool(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[]);                         //逻辑运算指令
void taskCompare(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *flagp);          //比较运算指令
void taskFlash(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *ipp, int *flagp);  //跳转指令
void taskIO(int operaNum, int frontNum, short int ax[], short int data[]);                                                    //输入输出指令
void putUpdate(short int ax[], int *ipp, const int *flagp, int ir);                                                           //输出各个状态
int judgeStart(int num, int *ipp, int tag);                                                                                   //判断开始翻译的位置
int transBase(const char tmp[], int start, int end, int tag);
void analyseInstructions(const char tmp[], short int ax[], int *ipp, int ir, int *flagp, short int data[])
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
        decideWork(operaNum, frontNum, backNum, imeNum, ax, data, ipp, flagp);
        putUpdate(ax, ipp, flagp, ir);
    }
}

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

void decideWork(int operaNum, int frontNum, int backNum, int imeNum, short int ax[], short int data[], int *ipp, int *flagp)
{
    switch (operaNum)
    {
    case 1:
        taskConvey(operaNum, frontNum, backNum, imeNum, ax, data);
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        taskCalculate(operaNum, frontNum, backNum, imeNum, ax, data);
        break;
    case 6:
    case 7:
    case 8:
        taskBool(operaNum, frontNum, backNum, imeNum, ax, data);
        break;
    case 9:
        taskCompare(operaNum, frontNum, backNum, imeNum, ax, data, flagp);
        break;
    case 10:
        taskFlash(operaNum, frontNum, backNum, imeNum, ax, data, ipp, flagp);
        break;
    case 11:
    case 12:
        taskIO(operaNum, frontNum, ax, data);
        break;
    default:
        break;
    }
}

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

void taskIO(int operaNum, int frontNum, short int ax[], short int data[])
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
            printf("out: %hd\n", ax[frontNum]);
        }
        else if (frontNum >= 5)
        {
            printf("out: %hd\n", data[ax[frontNum]]);
        }
        break;
    default:
        break;
    }
}

void putUpdate(short int ax[], int *ipp, const int *flagp, int ir)
{
    int i;
    printf("ip = %d\n", *ipp);
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
}

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