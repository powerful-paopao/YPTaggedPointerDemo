//
//  yp_taggedPointer.c
//  YPTaggedPointerDemo
//
//  Created by 张义沛 on 2024/12/12.
//

#include "yp_taggedPointer.h"
#include "yp-printf.h"

uintptr_t _yp_objc_obfuscatedTagToBasicTag(uintptr_t tag);
uintptr_t _yp_objc_decodeTaggedPointer_noPermute_withObfuscator(const void * _Nullable ptr,
                                                                uintptr_t obfuscator);
uintptr_t _yp_objc_decodeTaggedPointer_withObfuscator(const void * _Nullable ptr,
                                                      uintptr_t obfuscator);
void * _Nonnull _yp_objc_encodeTaggedPointer_withObfuscator(uintptr_t ptr, uintptr_t obfuscator);

uintptr_t yp__objc_basicTagToObfuscatedTag(uintptr_t tag);

/// 判断对象是否为Tagged Pointer
/// @param ptr 对象指针
bool yp_objc_isTaggedPointer(const void * _Nullable ptr) {
    /**
     * 将指针转化无符号长整型
     * _OBJC_TAG_MASK的值为(1UL<<63) = 0x1000 0000 0000 0000
     * 将指针与_OBJC_TAG_MASK进行按位与操作，如果结果等于_OBJC_TAG_MASK，则判定该对象为Tagged Pointer 对象
     * 这里运用了按位与的特性：任何数与0与操作结果为0，Tagged Pointer 对象在MSB模式下最高位为1，所以只有Tagged Pointer 对象的最高位值为1
     */
    /**
     * 举例1：
     * ptr = 0x1000 0000 0000 0012
     * ptr与_OBJC_TAG_MASK进行按位与操作，结果为0x1000 0000 0000 0000，除去最高位，其余位都与_OBJC_TAG_MASK的0位操作结果变成了0，结果是Tagged Pointer
     *
     * 举例2：
     * ptr = 0x0000 6000 0000 0010
     * ptr与_OBJC_TAG_MASK进行按位与操作，结果为0x0000 0000 0000 0000，结果为非Tagged Pointer
     *
     */
    return ((uintptr_t)ptr & _OBJC_TAG_MASK) == _OBJC_TAG_MASK;
}

void * _Nonnull yp_objc_makeTaggedPointer(enum objc_tag_index_t tag, uintptr_t value) {
    if (tag <= OBJC_TAG_Last60BitPayload) {
        /**
         * _OBJC_TAG_MASK                      = 1000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000，标志位在最高位，占1bit。
         * _OBJC_TAG_INDEX_SHIFT         = 0
         * _OBJC_TAG_PAYLOAD_RSHIFT = 4
         *
         *  (uintptr_t)tag << _OBJC_TAG_INDEX_SHIFT)，tag标识位置在低位0~2，占用3bits，值为0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 `0011`
         *
         *  value值包含数据 + 数据类型，数据类型占用4bits，在 [3,6] bits之间；数据占56bits，在[7,62] bits之间;
         *  ((value << _OBJC_TAG_PAYLOAD_RSHIFT) >> _OBJC_TAG_PAYLOAD_LSHIFT) ，目的是将数据+数据类型 向左移动3位，根据api提供的 标识位 | tag位 | 数据类型和数据 或运算；
         *  一个完整的标识指针：[标识位]（index = 63）+ [数据]（62~7） + 数据类型（6~3）+ tag标识位（0~2）
         */
        uintptr_t result =
            (_OBJC_TAG_MASK |
             ((uintptr_t)tag << _OBJC_TAG_INDEX_SHIFT) |
             ((value << _OBJC_TAG_PAYLOAD_RSHIFT) >> _OBJC_TAG_PAYLOAD_LSHIFT));
        printf("构建的原始指针为：0x%lx\n",result);
//        printf("构建的原始指针为：0b%s\n", decimalToBinary(result));
        // 构建好的标识指针进行加密混淆
        return _yp_objc_encodeTaggedPointer_withObfuscator(result, objc_debug_taggedpointer_obfuscator);
    } else {
        uintptr_t result =
            (_OBJC_TAG_EXT_MASK |
             ((uintptr_t)(tag - OBJC_TAG_First52BitPayload) << _OBJC_TAG_EXT_INDEX_SHIFT) |
             ((value << _OBJC_TAG_EXT_PAYLOAD_RSHIFT) >> _OBJC_TAG_EXT_PAYLOAD_LSHIFT));
        return _yp_objc_encodeTaggedPointer_withObfuscator(result, objc_debug_taggedpointer_obfuscator);
    }
}

uintptr_t yp_objc_decodeTaggedPointer(const void * _Nullable ptr) {
    return _yp_objc_decodeTaggedPointer_withObfuscator(ptr, objc_debug_taggedpointer_obfuscator);
}

uintptr_t yp_objc_getTaggedPointerTag(const void * _Nullable ptr) {
    /**
     * 解混淆操作
     * 与 _OBJC_TAG_INDEX_MASK = 0111 进行 按位与 操作，得到tag位（[0~2]bits）数据
     */
    uintptr_t value = _yp_objc_decodeTaggedPointer_withObfuscator(ptr, objc_debug_taggedpointer_obfuscator);
    uintptr_t basicTag = (value >> _OBJC_TAG_INDEX_SHIFT) & _OBJC_TAG_INDEX_MASK;
    // TODO：extTag的布局需要更一步了解
    uintptr_t extTag =   (value >> _OBJC_TAG_EXT_INDEX_SHIFT) & _OBJC_TAG_EXT_INDEX_MASK;
    if (basicTag == _OBJC_TAG_INDEX_MASK) {
        return (objc_tag_index_t)(extTag + OBJC_TAG_First52BitPayload);
    } else {
        return (objc_tag_index_t)basicTag;
    }
}

uintptr_t yp_objc_getTaggedPointerValueValue(const void * _Nullable ptr) {
    uintptr_t value = yp_objc_decodeTaggedPointer(ptr);
    uintptr_t basicTag = (value >> _OBJC_TAG_INDEX_SHIFT) & _OBJC_TAG_INDEX_MASK;
    if (basicTag == _OBJC_TAG_INDEX_MASK) {
        return (value << _OBJC_TAG_EXT_PAYLOAD_LSHIFT) >> _OBJC_TAG_EXT_PAYLOAD_RSHIFT;
    } else {
        return (value << _OBJC_TAG_PAYLOAD_LSHIFT) >> _OBJC_TAG_PAYLOAD_RSHIFT;
    }
}

#pragma mark --private

void * _Nonnull _yp_objc_encodeTaggedPointer_withObfuscator(uintptr_t ptr, uintptr_t obfuscator) {
    
    // 混淆具体细节可以不用关系
    uintptr_t value = (obfuscator ^ ptr);
#if OBJC_SPLIT_TAGGED_POINTERS
    if ((value & _OBJC_TAG_NO_OBFUSCATION_MASK) == _OBJC_TAG_NO_OBFUSCATION_MASK)
        return (void *)ptr;
    uintptr_t basicTag = (value >> _OBJC_TAG_INDEX_SHIFT) & _OBJC_TAG_INDEX_MASK;
    uintptr_t permutedTag = yp__objc_basicTagToObfuscatedTag(basicTag);
    value &= ~(_OBJC_TAG_INDEX_MASK << _OBJC_TAG_INDEX_SHIFT);
    value |= permutedTag << _OBJC_TAG_INDEX_SHIFT;
#endif
    return (void *)value;
}

/// 将Tagged Pointer对象指针和随机值进行接触混淆操作
/// @param ptr 当前Tagged Pointer对象指针
/// @param obfuscator 随机值
uintptr_t _yp_objc_decodeTaggedPointer_withObfuscator(const void * _Nullable ptr,
                                         uintptr_t obfuscator)
{
    uintptr_t value = _yp_objc_decodeTaggedPointer_noPermute_withObfuscator(ptr, obfuscator);
#if OBJC_SPLIT_TAGGED_POINTERS
    uintptr_t basicTag = (value >> _OBJC_TAG_INDEX_SHIFT) & _OBJC_TAG_INDEX_MASK;

    value &= ~(_OBJC_TAG_INDEX_MASK << _OBJC_TAG_INDEX_SHIFT);
    value |= _yp_objc_obfuscatedTagToBasicTag(basicTag) << _OBJC_TAG_INDEX_SHIFT;
    printf("basicTag = %ld , permutedTag = %ld \n", basicTag, _yp_objc_obfuscatedTagToBasicTag(basicTag));
    
#endif
    return value;
}

uintptr_t _yp_objc_decodeTaggedPointer_noPermute_withObfuscator(const void * _Nullable ptr,
                                                   uintptr_t obfuscator) {
    uintptr_t value = (uintptr_t)ptr;
#if OBJC_SPLIT_TAGGED_POINTERS
    /**
     * 举例：value = 0x80000017 ，该Tagged Pointer对象tag值为7，带有扩展位
     * _OBJC_TAG_NO_OBFUSCATION_MASK 值为 0xc0000007
     * value 和 _OBJC_TAG_NO_OBFUSCATION_MASK 进行按位与操作，结果为
     */

    if ((value & _OBJC_TAG_NO_OBFUSCATION_MASK) == _OBJC_TAG_NO_OBFUSCATION_MASK)
        return value;
#endif
    return value ^ obfuscator;
}

uintptr_t _yp_objc_obfuscatedTagToBasicTag(uintptr_t tag) {
    for (unsigned i = 0; i < 7; i++)
        if (objc_debug_tag60_permutations[i] == tag)
            return i;
    return 7;
}

uintptr_t yp__objc_basicTagToObfuscatedTag(uintptr_t tag) {
    return objc_debug_tag60_permutations[tag];
}
