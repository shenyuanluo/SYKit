//
//  SYYuvConverter.h
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by shenyuanluo on 2018/10/12.
//  Copyright © 2018 http://blog.shenyuanluo.com/ All rights reserved.
//


/*
 YUV 格式转换
 */

#ifndef SYYuvConverter_h
#define SYYuvConverter_h

#include <iostream>
#include "SYHeader.h"


class SYYuvConverter
{
private:
    
public:
    SYYuvConverter();
    
    ~SYYuvConverter();
    
    /**
     I420 转 NV12
     
     @param inYuv I420数据（输入）
     @param width 帧-宽度
     @param height 帧-高度度
     @param outYuv NV12数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_I420ToNv12(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     I420 转 NV21
     
     @param inYuv I420数据（输入）
     @param width 帧-宽度
     @param height 帧-高度度
     @param outYuv NV21数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_I420ToNv21(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     NV12 转 I420

     @param inYuv NV12数据（输入）
     @param width 帧-宽度
     @param height 帧-高度度
     @param outYuv I420数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Nv12ToI420(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     NV12 转 NV21
     
     @param inYuv NV12数据（输入）
     @param width 帧-宽度
     @param height 帧-高度度
     @param outYuv NV21数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Nv12ToNv21(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     NV21 转 I420
     
     @param inYuv NV21数据（输入）
     @param width 帧-宽度
     @param height 帧-高度度
     @param outYuv I420数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Nv21ToI420(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     NV21 转 NV12
     
     @param inYuv NV21数据（输入）
     @param width 帧-宽度
     @param height 帧-高度度
     @param outYuv NV12数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Nv21ToNv12(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
};

#endif /* SYYuvConverter_h */
