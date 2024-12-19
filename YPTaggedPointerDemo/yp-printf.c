//
//  yp-printf.c
//  YPTaggedPointerDemo
//
//  Created by 张义沛 on 2024/12/13.
//

#include "yp-printf.h"
#include <stdlib.h>
#include <string.h>


/// 获取当前系统的位数
/// @param x 整数
int count_bits(unsigned long x) {
    int count = 0;
    while (x) {
        if (x & 1U) {
            count++;
        }
        x >>= 1;
    }
    return count;
}

int int_bits(void) {
    return count_bits(~0lU);
}


char* decimalToBinary(long n) {
    
    int count = count_bits(~0lU);
    
    char* binary = (char*)malloc((count + 1) * sizeof(char));
    binary[64] = '\0';
    
    char arr[64];
    for (int i = count - 1; i >= 0; i--) {
        char val = n & 1lU ? '1' : '0';
        binary[i] = val;
        arr[i] = val;
        n >>= 1;
    }
    // 格式化
    unsigned int spaceCount = count / 4 - 1 ;
    
    char* format = (char*)malloc((count + spaceCount + 1) * sizeof(char));
    format[count + spaceCount] = '\0';
    
    int index = 1;
    for (int i = 0; i < count + spaceCount; i++) {
        if (index % 5 == 0) {
            format[i] = ' ';
            index = 0;
        } else {
            format[i] = binary[i - i / 5];
        }
        index ++;
    }
    
    free(binary);
    binary = NULL;
    
    return format;;
}
