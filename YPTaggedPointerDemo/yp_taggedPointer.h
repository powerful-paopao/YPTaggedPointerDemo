//
//  yp_taggedPointer.h
//  YPTaggedPointerDemo
//
//  Created by 张义沛 on 2024/12/12.
//

#ifndef yp_taggedPointer_h
#define yp_taggedPointer_h

#include <stdio.h>
#include <stdbool.h>
#include "yp-taggedpoint-header.h"

/// 是否支持Tagged Pointer
bool yp_objc_taggedPointersEnabled(void);

/// 判断对象是否为Tagged Pointer
/// @param ptr 对象指针
bool yp_objc_isTaggedPointer(const void * _Nullable ptr);

/// 获取系统随机值
uintptr_t yp_objc_debug_taggedpointer_obfuscator(void);

/// Tagged Pointer对象指针进行混淆操作
/// @param tag tag标签位
/// @param value 当前Tagged Pointer对象的指针（未混淆指针）
void * _Nonnull yp_objc_makeTaggedPointer(enum objc_tag_index_t tag, uintptr_t value);

/// Tagged Pointer对象指针进行解混淆操作
/// @param ptr 当前Tagged Pointer对象的指针（混淆指针）
uintptr_t yp_objc_decodeTaggedPointer(const void * _Nullable ptr);

/// 获取Tagged Pointer对象的 Tag值（对象类型）
/// @param ptr 对象指针
uintptr_t yp_objc_getTaggedPointerTag(const void * _Nullable ptr);

/// 获取Tagged Pointer对象的 Value值（对象类型）
/// @param ptr ptr 对象指针
uintptr_t yp_objc_getTaggedPointerValueValue(const void * _Nullable ptr);

#endif /* yp_taggedPointer_h */
