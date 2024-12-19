//
//  ViewController.m
//  YPTaggedPointerDemo
//
//  Created by zyp on 2024/12/12.
//

#import "ViewController.h"
#import <malloc/malloc.h>
#import "yp_taggedPointer.h"
#import "yp-printf.h"
#import <CoreFoundation/CoreFoundation.h>

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    NSNumber *number1 = [NSNumber numberWithInt:1];
    NSLog(@"number1指针为：%p",number1);

    void *p = (void *)[self buildingNumberBinariesWithValue:[number1 longValue] type:@"int"];
    printf("构建的原始指针为：0x%lx\n",(uintptr_t)p);
}

- (uintptr_t)buildingStringBinariesWithString:(NSString *)str {
    
    NSInteger len = str.length;
    
    uintptr_t res = 0LU;
    
    // 字符串转ASCLL码
    for (int i = 0 ; i < len; i++) {
        int asciicode = [str characterAtIndex:i];
        
        // 根据Unicode编码
        res |= asciicode << (i * 8);
    }
    
    uintptr_t result = ((len << 3) | (res << 7)) >> 3;
    void *p = yp_objc_makeTaggedPointer(OBJC_TAG_NSString,result);
    
    return (uintptr_t)p;

}

- (uintptr_t)buildingNumberBinariesWithValue:(long)value type:(NSString *)type {
    
    NSDictionary *dic = @{
        @"char":@(0),
        @"short":@(1),
        @"int":@(2),
        @"long":@(3),
        @"float":@(4),
        @"double":@(5)
    };
    NSNumber *typeNumber = [dic objectForKey:type];
    uintptr_t typeValue = [typeNumber unsignedLongValue];
    uintptr_t result = ((typeValue << 3) | (value << 7)) >> 3;
    void *p = yp_objc_makeTaggedPointer(OBJC_TAG_NSNumber,result);
    
    return (uintptr_t)p;
}

@end
