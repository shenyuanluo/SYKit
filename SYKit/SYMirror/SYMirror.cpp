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
            return nv12Mirror(inYuv, width, height, outYuv, direction);
        }
            break;
            
        case SYYuv_nv21:
        {
            return nv21Mirror(inYuv, width, height, outYuv, direction);
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
    const int len               = width * height; // 像素点数
    unsigned int uStart         = len;            // U 数据起始位置
    unsigned int vStart         = len + (len>>2); // V 数据起始位置
    unsigned int uvWidth        = (width>>1);     // U、V 数据宽度
    unsigned int uvHeight       = (height>>1);    // U、V 数据高度
    unsigned int yWidthMaxIdx   = width - 1;      // Y 数据宽度最大下标
    unsigned int yHeightMaxIdx  = height - 1;     // Y 数据高度最大下标
    unsigned int uvWidthMaxIdx  = uvWidth - 1;    // U、V 数据宽度最大下标
    unsigned int uvHeightMaxIdx = uvHeight - 1;   // U、V 数据高度最大下标
    unsigned int srcYPos        = 0;              // 数据源（Y 数据）位置
    unsigned int srcUPos        = len;            // 数据源（U 数据）位置
    unsigned int srcVPos        = vStart;         // 数据源（V 数据）位置
    unsigned int dstYPos        = 0;              // 目标（Y 数据）位置
    unsigned int dstUVPos       = 0;              // 目标（U、V 数据）位置
    unsigned int yPos           = 0;              // 镜像后的 Y 数据 行 下标
    unsigned int uvPos          = 0;              // 镜像后的 U 数据 行 下标
    
    switch (direction)  // 镜像方向
    {
        case SYMirror_horizontal:    // 水平镜像
        {
            for(int row = 0; row < height; row++)   // 遍历所有行
            {
                yPos = row * width;
                if (0 == (row&1))
                {
                    uvPos = (row>>1) * uvWidth;
                }
                for(int col = 0; col < width; col++)  // 遍历所有列
                {
                    dstYPos         = yPos + (yWidthMaxIdx - col);
                    outYuv[dstYPos] = inYuv[srcYPos++]; // 镜像 Y 数据
                    
                    if ((0 == (row&1)) && (0 == (col&1)))
                    {
                        dstUVPos                  = uvPos + (uvWidthMaxIdx - (col>>1));
                        outYuv[uStart + dstUVPos] = inYuv[srcUPos++];   // 镜像 U 数据
                        outYuv[vStart + dstUVPos] = inYuv[srcVPos++];   // 镜像 V 数据
                    }
                }
            }
        }
            break;
            
        case SYMirror_vertical:    // 垂直镜像
        {
            for(int row = 0; row < height; row++)   // 遍历所有行
            {
                yPos = (yHeightMaxIdx - row) * width;
                if (0 == (row&1))
                {
                    uvPos = (uvHeightMaxIdx - (row>>1)) * uvWidth;
                }
                for(int col = 0; col < width; col++)  // 遍历所有列
                {
                    dstYPos         = yPos + col;
                    outYuv[dstYPos] = inYuv[srcYPos++]; // 镜像 Y 数据
                    
                    if ((0 == (row&1)) && (0 == (col&1)))
                    {
                        dstUVPos                  =  uvPos + (col>>1);
                        outYuv[uStart + dstUVPos] = inYuv[srcUPos++];   // 镜像 U 数据
                        outYuv[vStart + dstUVPos] = inYuv[srcVPos++];   // 镜像 V 数据
                    }
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
    const int len               = width * height;   // 像素点数
    unsigned int uvWidth        = (width>>1);       // UV 数据宽度
    unsigned int uvHeight       = (height>>1);      // UV 数据高度
    unsigned int yWidthMaxIdx   = width - 1;        // Y 数据宽度最大下标
    unsigned int yHeightMaxIdx  = height - 1;       // Y 数据高度最大下标
    unsigned int uvWidthMaxIdx  = uvWidth - 1;      // U、V 数据宽度最大下标
    unsigned int uvHeightMaxIdx = uvHeight - 1;     // U、V 数据高度最大下标
    unsigned int srcYPos        = 0;                // 数据源（Y 数据）位置
    unsigned int srcUVPos       = len;              // 数据源（UV 数据）位置
    unsigned int dstYPos        = 0;                // 目标（Y 数据）位置
    unsigned int dstUVPos       = 0;                // 目标（UV 数据）位置
    unsigned int yPos           = 0;                // 镜像后的 Y 数据 行下标
    unsigned int uvPos          = 0;                // 镜像后的 UV 数据 行 下标
    
    switch (direction)  // 镜像方向
    {
        case SYMirror_horizontal:    // 水平镜像
        {
            for(int row = 0; row < height; row++)   // 遍历所有行
            {
                yPos = row * width;
                if (0 == (row&1))
                {
                    uvPos = row * uvWidth ;
                }
                for(int col = 0; col < width; col++)  // 遍历所有列
                {
                    // 镜像 Y 数据
                    dstYPos         = yPos + (yWidthMaxIdx - col);
                    outYuv[dstYPos] = inYuv[srcYPos++];
                    
                    if ((0 == (row&1)) && (0 == (col&1)))
                    {
                        // 镜像 UV 数据
                        dstUVPos             = len + uvPos + ((uvWidthMaxIdx - (col>>1))<<1);
                        outYuv[dstUVPos]     = inYuv[srcUVPos++];
                        outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                    }
                }
            }
        }
            break;
            
        case SYMirror_vertical:    // 垂直镜像
        {
            for(int row = 0; row < height; row++)   // 遍历所有行
            {
                yPos = (yHeightMaxIdx - row) * width;
                if (0 == (row&1))
                {
                    uvPos = ((uvHeightMaxIdx - (row>>1))<<1) * uvWidth;
                }
                for(int col = 0; col < width; col++)  // 遍历所有列
                {
                    // 镜像 Y 数据
                    dstYPos         = yPos + col;
                    outYuv[dstYPos] = inYuv[srcYPos++];
                    
                    if ((0 == (row&1)) && (0 == (col&1)))
                    {
                        // 镜像 UV 数据
                        dstUVPos             = len + uvPos + col;
                        outYuv[dstUVPos]     = inYuv[srcUVPos++];   
                        outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                    }
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
