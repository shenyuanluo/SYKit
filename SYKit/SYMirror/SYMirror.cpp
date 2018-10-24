//
//  SYMirror.cpp
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by shenyuanluo on 2018/10/24.
//  Copyright © 2018 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "SYMirror.h"


#pragma mark - Public
#pragma mark -- 构造s函数
SYMirror::SYMirror()
{
    initParam();
}

#pragma mark -- 析构函数
SYMirror::~SYMirror()
{
    
}

#pragma mark -- 设置 YUV 数据（格式）类型
SYKIT_API void SYMirror::SY_SetYuvType(SYYuvType yuvType)
{
    m_yuvType = yuvType;
}

#pragma mark -- YUV 镜像
SYKIT_API int SYMirror::SY_MirrorYuv(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYMirrorDirection direction)
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    switch (m_yuvType)
    {
        case SYYuv_i420:
        {
            return i420Mirror(inYuv, width, height, outYuv, direction);
        }
            break;
            
        case SYYuv_nv12:
        {
            return nv21Mirror(inYuv, width, height, outYuv, direction);
        }
            break;
            
        case SYYuv_nv21:
        {
            return nv12Mirror(inYuv, width, height, outYuv, direction);
        }
            break;
            
        default:
            break;
    }
    return SYErr_success;
}


#pragma mark - Pirvate
#pragma mark -- 初始化参数
void SYMirror::initParam()
{
    m_yuvType = SYYuv_i420;
}

#pragma mark -- I420 镜像
int SYMirror::i420Mirror(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYMirrorDirection direction) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const int len           = width * height;   // 像素点数
    unsigned int uWidth     = (width>>1);       // U 数据宽度
    unsigned int vWidth     = (width>>1);       // V 数据宽度
    unsigned int uHeight    = (height>>1);      // U 数据高度
    unsigned int vHeight    = (height>>1);      // V 数据高度
    unsigned int srcYPos    = 0;                // 数据源（Y 数据）位置
    unsigned int srcUPos   = len;               // 数据源（U 数据）位置
    unsigned int srcVPos   = len + (len>>2);    // 数据源（V 数据）位置
    unsigned int dstYPos   = 0;                 // 目标（Y 数据）位置
    unsigned int dstUPos   = 0;                 // 目标（U 数据）位置
    unsigned int dstVPos   = 0;                 // 目标（V 数据）位置
    unsigned int newYRow   = 0;                 // 镜像后的 Y 数据 行 下标
    unsigned int newYCol   = 0;                 // 镜像后的 Y 数据 列 下标
    unsigned int newURow   = 0;                 // 镜像后的 U 数据 行 下标
    unsigned int newVRow   = 0;                 // 镜像后的 Y 数据 行 下标
    unsigned int newUCol   = 0;                 // 镜像后的 U 数据 列 下标
    unsigned int newVCol   = 0;                 // 镜像后的 V 数据 列 下标
    unsigned int newYWidth = width;             // 镜像后 Y 数据宽度
    unsigned int newUWidth = uWidth;            // 镜像后 U 数据宽度
    unsigned int newVWidth = vWidth;            // 镜像后 V 数据宽度
    
    switch (direction)  // 镜像方向
    {
        case SYMirror_horizontal:    // 水平镜像
        {
            // 镜像 Y 数据
            for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
            {
                newYRow = oldRow;
                for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                {
                    newYCol         = width - 1 - oldCol;
                    dstYPos         = newYRow * newYWidth + newYCol;
                    outYuv[dstYPos] = inYuv[srcYPos++];
                }
            }
            // 镜像 U 数据
            for (int oldRow = 0; oldRow < uHeight; oldRow++)
            {
                newURow = oldRow;
                for (int oldCol = 0; oldCol < uWidth; oldCol++)
                {
                    newUCol             = uWidth - 1 - oldCol;
                    dstUPos             = len + newURow * newUWidth + newUCol;
                    outYuv[dstUPos]     = inYuv[srcUPos++];
                }
            }
            // 镜像 V 数据
            for (int oldRow = 0; oldRow < vHeight; oldRow++)
            {
                newVRow = oldRow;
                for (int oldCol = 0; oldCol < vWidth; oldCol++)
                {
                    newVCol             = vWidth - 1 - oldCol;
                    dstVPos             = len + (len>>2) + newVRow * newVWidth + newVCol;
                    outYuv[dstVPos]     = inYuv[srcVPos++];
                }
            }
        }
            break;
            
        case SYMirror_vertical:    // 垂直镜像
        {
            // 镜像 Y 数据
            for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
            {
                newYRow = height - 1 - oldRow;
                for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                {
                    newYCol         = oldCol;
                    dstYPos         = newYRow * newYWidth + newYCol;
                    outYuv[dstYPos] = inYuv[srcYPos++];
                }
            }
            // 镜像 U 数据
            for (int oldRow = 0; oldRow < uHeight; oldRow++)
            {
                newURow = uHeight - 1 - oldRow;
                for (int oldCol = 0; oldCol < uWidth; oldCol++)
                {
                    newUCol             = oldCol;
                    dstUPos             = len + newURow * newUWidth + newUCol;
                    outYuv[dstUPos]     = inYuv[srcUPos++];
                }
            }
            // 镜像 V 数据
            for (int oldRow = 0; oldRow < vHeight; oldRow++)
            {
                newVRow = vHeight - 1 - oldRow;
                for (int oldCol = 0; oldCol < vWidth; oldCol++)
                {
                    newVCol             = oldCol;
                    dstVPos             = len + (len>>2) + newVRow * newVWidth + newVCol;
                    outYuv[dstVPos]     = inYuv[srcVPos++];
                }
            }
        }
            break;
            
        default:
            break;
    }
    return SYErr_success;
}

#pragma mark -- NV12 镜像
int SYMirror::nv12Mirror(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYMirrorDirection direction) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const int len           = width * height;   // 像素点数
    unsigned int uvWidth    = (width>>1);       // UV 数据宽度
    unsigned int uvHeight   = (height>>1);      // UV 数据高度
    unsigned int srcYPos    = 0;                // 数据源（Y 数据）位置
    unsigned int srcUVPos   = len;              // 数据源（UV 数据）位置
    unsigned int dstYPos    = 0;                // 目标（Y 数据）位置
    unsigned int dstUVPos   = 0;                // 目标（UV 数据）位置
    unsigned int newYRow    = 0;                // 镜像后的 Y 数据 行下标
    unsigned int newYCol    = 0;                // 镜像后的 Y 数据 列 下标
    unsigned int newUVRow   = 0;                // 镜像后的 UV 数据 行 下标
    unsigned int newUVCol   = 0;                // 镜像后的 UV 数据 列 下标
    unsigned int newYWidth  = width;           // 镜像后 UV 数据宽度
    unsigned int newUVWidth = uvWidth;         // 镜像后 UV 数据高度
    
    switch (direction)  // 镜像方向
    {
        case SYMirror_horizontal:    // 水平镜像
        {
            // 镜像 Y 数据
            for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
            {
                newYRow = oldRow;
                for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                {
                    newYCol         = width - 1 - oldCol;
                    dstYPos         = newYRow * newYWidth + newYCol;
                    outYuv[dstYPos] = inYuv[srcYPos++];
                }
            }
            // 镜像 UV 数据
            for (int oldRow = 0; oldRow < uvHeight; oldRow++)
            {
                newUVRow = ((oldRow)<<1);
                for (int oldCol = 0; oldCol < uvWidth; oldCol++)
                {
                    newUVCol             = ((uvWidth - 1 - oldCol)<<1);
                    dstUVPos             = len + newUVRow * newUVWidth + newUVCol;
                    outYuv[dstUVPos]     = inYuv[srcUVPos++];
                    outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                }
            }
        }
            break;
            
        case SYMirror_vertical:    // 垂直镜像
        {
            // 镜像 Y 数据
            for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
            {
                newYRow = height - 1 - oldRow;
                for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                {
                    newYCol         = oldCol;
                    dstYPos         = newYRow * newYWidth + newYCol;
                    outYuv[dstYPos] = inYuv[srcYPos++];
                }
            }
            // 镜像 UV 数据
            for (int oldRow = 0; oldRow < uvHeight; oldRow++)
            {
                newUVRow = ((uvHeight - 1 - oldRow)<<1);
                for (int oldCol = 0; oldCol < uvWidth; oldCol++)
                {
                    newUVCol             = ((oldCol)<<1);
                    dstUVPos             = len + newUVRow * newUVWidth + newUVCol;
                    outYuv[dstUVPos]     = inYuv[srcUVPos++];
                    outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                }
            }
        }
            break;
            
        default:
            break;
    }
    return SYErr_success;
}

#pragma mark -- NV21 镜像
int SYMirror::nv21Mirror(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYMirrorDirection direction) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    
    return nv12Mirror(inYuv, width, height, outYuv, direction);
}
