//
//  SYMirror.h
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by shenyuanluo on 2018/10/24.
//  Copyright © 2018 http://blog.shenyuanluo.com/ All rights reserved.
//


/*
 YUV 镜像
 */

#ifndef SYMirror_h
#define SYMirror_h

#include <iostream>
#include "SYHeader.h"

class SYMirror
{
private:
    SYYuvType m_yuvType;          // 镜像 YUV 数据（格式）类型
    
    /**
     初始化默认参数
     */
    void initParam();
    
    /**
     I420 镜像
     
     @param inYuv I420 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv 镜像后 I420 数据（输出）
     @param direction 镜像方向，参见‘SYMirrorDirection’
     @return 镜像是否成功，参见‘SYErrType’
     */
    int i420Mirror(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYMirrorDirection direction) const;
    
    /**
     NV12 镜像
     
     @param inYuv NV12 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv 镜像后 NV12 数据（输出）
     @param direction 镜像方向，参见‘SYMirrorDirection’
     @return 镜像是否成功，参见‘SYErrType’
     */
    int nv12Mirror(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYMirrorDirection direction) const;
    
    /**
     NV21 镜像
     
     @param inYuv NV21 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv 镜像后 NV21 数据（输出）
     @param direction 镜像方向，参见‘SYMirrorDirection’
     @return 镜像是否成功，参见‘SYErrType’
     */
    int nv21Mirror(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYMirrorDirection direction) const;
    
public:
    SYMirror();
    
    ~SYMirror();
    
    /**
     设置镜像 YUV 数据格式类型（默认：I420）
     
     @param yuvType yuv 数据格式类型
     */
    SYKIT_API void SY_SetYuvType(SYYuvType yuvType);
    
    /**
     YUV 镜像
     
     @param inYuv YUV 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv 镜像后 YUV 数据（输出）
     @param direction 镜像方向，参见‘SYMirrorDirection’
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_MirrorYuv(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYMirrorDirection direction);
};

#endif /* SYMirror_h */
