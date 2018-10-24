//
//  SYRotate.cpp
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/10/23.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "SYRotate.h"


#pragma mark - Public
#pragma mark -- 构造函数
SYRotate::SYRotate()
{
    initParam();
}

#pragma mark -- 析构函数
SYRotate::~SYRotate()
{
    
}

#pragma mark -- 设置 YUV 数据（格式）类型
SYKIT_API void SYRotate::SY_SetYuvType(SYYuvType yuvType)
{
    m_yuvType = yuvType;
}

#pragma mark -- 旋转 YUV
SYKIT_API int SYRotate::SY_RotateYuv(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYRotateDirection direction, SYRotateDegree degree) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    switch (m_yuvType)
    {
        case SYYuv_i420:
        {
            return i420Rotate(inYuv, width, height, outYuv, direction, degree);
        }
            break;
            
        case SYYuv_nv12:
        {
            return nv12Rotate(inYuv, width, height, outYuv, direction, degree);
        }
            break;
            
        case SYYuv_nv21:
        {
            return nv21Rotate(inYuv, width, height, outYuv, direction, degree);
        }
            break;
            
        default:
            break;
    }
    return SYErr_success;
}



#pragma mark - Private
#pragma mark -- 初始化参数
void SYRotate::initParam()
{
    m_yuvType = SYYuv_i420;
}

#pragma mark -- YUV-I420 旋转
SYKIT_API int SYRotate::i420Rotate(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYRotateDirection direction, SYRotateDegree degree) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const int len         = width * height; // 像素点数
    unsigned int vStart   = len + (len>>2); // V 数据起始位置
    unsigned int uvWidth  = (width>>1);     // UV 数据宽度
    unsigned int uvHeight = (height>>1);    // UV 数据高度
    unsigned int srcYPos  = 0;              // 数据源（Y 数据）位置
    unsigned int srcUPos  = len;            // 数据源（U 数据）位置
    unsigned int srcVPos  = vStart;         // 数据源（V 数据）位置
    unsigned int dstYPos  = 0;              // 目标（Y 数据）位置
    unsigned int dstUPos  = 0;              // 目标（U 数据）位置
    unsigned int dstVPos  = 0;              // 目标（V 数据）位置
    unsigned int yPos     = 0;              // 旋转后的 Y 数据 列 下标
    unsigned int uvPos    = 0;              // 旋转后的 UV 数据 列 下标
    
    switch (direction)  // 旋转方向
    {
        case SYRotate_clockwise:    // 顺时针
        {
            switch (degree) // 旋转角度
            {
                case SYRotate_90:   // 90°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        yPos = height - 1 - row;
                        if (0 == (row&1))
                        {
                            uvPos = uvHeight - 1 - (row>>1);
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = col * height + yPos;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                // 旋转 U 数据
                                dstUPos         = len + (col>>1) * uvHeight + uvPos;
                                outYuv[dstUPos] = inYuv[srcUPos++];
                                
                                // 旋转 V 数据
                                dstVPos         = vStart + (col>>1) * uvHeight + uvPos;
                                outYuv[dstVPos] = inYuv[srcVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_180:  // 180°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        yPos = (height - 1 - row) * width;
                        if (0 == (row&1))
                        {
                            uvPos = (uvHeight - 1 - (row>>1))  * uvWidth;
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = yPos + (width - 1 - col);
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                // 旋转 U 数据
                                dstUPos         = len + (uvPos + uvWidth - 1 - (col>>1));
                                outYuv[dstUPos] = inYuv[srcUPos++];
                                
                                // 旋转 V 数据
                                dstVPos         = vStart + (uvPos + uvWidth - 1 - (col>>1));
                                outYuv[dstVPos] = inYuv[srcVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_270:  // 270°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        if (0 == (row&1))
                        {
                            uvPos = (row>>1);
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = (width - 1 - col) * height + row;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                // 旋转 U 数据
                                dstUPos         = len + (uvWidth - 1 - (col>>1)) * uvHeight + uvPos;
                                outYuv[dstUPos] = inYuv[srcUPos++];
                                
                                // 旋转 V 数据
                                dstVPos         = vStart + (uvWidth - 1 - (col>>1)) * uvHeight + uvPos;
                                outYuv[dstVPos] = inYuv[srcVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_360:  // 360°
                {
                    memcpy(outYuv, inYuv, width * height * 1.5);
                }
                    break;
                    
                default:
                    break;
            }
        }
            break;
            
        case SYRotate_antiClockwise:    // 逆时针
        {
            switch (degree) // 旋转角度
            {
                case SYRotate_90:   // 90°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        if (0 == (row&1))
                        {
                            uvPos = (row>>1);
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = (width - 1 - col) * height + row;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                // 旋转 U 数据
                                dstUPos         = len + (uvWidth - 1 - (col>>1)) * uvHeight + uvPos;
                                outYuv[dstUPos] = inYuv[srcUPos++];
                                
                                // 旋转 V 数据
                                dstVPos         = vStart + (uvWidth - 1 - (col>>1)) * uvHeight + uvPos;
                                outYuv[dstVPos] = inYuv[srcVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_180:  // 180°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        yPos = (height - 1 - row) * width;
                        if (0 == (row&1))
                        {
                            uvPos = (uvHeight - 1 - (row>>1))  * uvWidth;
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = yPos + (width - 1 - col);
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                // 旋转 U 数据
                                dstUPos         = len + (uvPos + uvWidth - 1 - (col>>1));
                                outYuv[dstUPos] = inYuv[srcUPos++];
                                
                                // 旋转 V 数据
                                dstVPos         = vStart + (uvPos + uvWidth - 1 - (col>>1));
                                outYuv[dstVPos] = inYuv[srcVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_270:  // 270°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        yPos = height - 1 - row;
                        if (0 == (row&1))
                        {
                            uvPos = uvHeight - 1 - (row>>1);
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = col * height + yPos;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                // 旋转 U 数据
                                dstUPos         = len + (col>>1) * uvHeight + uvPos;
                                outYuv[dstUPos] = inYuv[srcUPos++];
                                
                                // 旋转 V 数据
                                dstVPos         = vStart + (col>>1) * uvHeight + uvPos;
                                outYuv[dstVPos] = inYuv[srcVPos++];
                            }
                        }
                    }
                }                    break;
                    
                case SYRotate_360:  // 360°
                {
                    memcpy(outYuv, inYuv, width * height * 1.5);
                }
                    break;
                    
                default:
                    break;
            }
        }
            break;
            
        default:
            break;
    }
    return SYErr_success;
}

#pragma mark -- YUV-NV12 旋转
SYKIT_API int SYRotate::nv12Rotate(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYRotateDirection direction, SYRotateDegree degree) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const int len         = width * height; // 像素点数
    unsigned int uvWidth  = (width>>1);     // UV 数据宽度
    unsigned int uvHeight = (height>>1);    // UV 数据高度
    unsigned int srcYPos  = 0;              // 数据源（Y 数据）位置
    unsigned int srcUVPos = len;            // 数据源（UV 数据）位置
    unsigned int dstYPos  = 0;              // 目标（Y 数据）位置
    unsigned int dstUVPos = 0;              // 目标（UV 数据）位置
    unsigned int yPos     = 0;              // 旋转后的 Y 数据 列 下标
    unsigned int uvPos    = 0;              // 旋转后的 UV 数据 列 下标
    
    switch (direction)  // 旋转方向
    {
        case SYRotate_clockwise:    // 顺时针
        {
            switch (degree) // 旋转角度
            {
                case SYRotate_90:   // 90°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        yPos = height - 1 - row;
                        if (0 == (row&1))
                        {
                            uvPos = ((uvHeight - 1 - (row>>1))<<1);
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = col * height + yPos;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            // 旋转 UV 数据
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                dstUVPos             = len + col * uvHeight + uvPos;
                                outYuv[dstUVPos]     = inYuv[srcUVPos++];
                                outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_180:  // 180°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        yPos = (height - 1 - row) * width;
                        if (0 == (row&1))
                        {
                            uvPos = (((uvHeight - 1 - (row>>1)) * uvWidth)<<1);
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = yPos + (width - 1 - col);
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            // 旋转 UV 数据
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                dstUVPos             = len + uvPos + (((width>>1) - 1 - (col>>1))<<1);
                                outYuv[dstUVPos]     = inYuv[srcUVPos++];
                                outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_270:  // 270°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = (width - 1 - col) * height + row;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            // 旋转 UV 数据
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                dstUVPos             = len + (((uvWidth - 1 - (col>>1)) * uvHeight)<<1) + row;
                                outYuv[dstUVPos]     = inYuv[srcUVPos++];
                                outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_360:  // 360°
                {
                    memcpy(outYuv, inYuv, width * height * 1.5);
                }
                    break;
                    
                default:
                    break;
            }
        }
            break;
            
        case SYRotate_antiClockwise:    // 逆时针
        {
            switch (degree) // 旋转角度
            {
                case SYRotate_90:   // 90°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = (width - 1 - col) * height + row;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            // 旋转 UV 数据
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                dstUVPos             = len + (((uvWidth - 1 - (col>>1)) * uvHeight)<<1) + row;
                                outYuv[dstUVPos]     = inYuv[srcUVPos++];
                                outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_180:  // 180°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        yPos = (height - 1 - row) * width;
                        if (0 == (row&1))
                        {
                            uvPos = (((uvHeight - 1 - (row>>1)) * uvWidth)<<1);
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = yPos + (width - 1 - col);
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            // 旋转 UV 数据
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                dstUVPos             = len + uvPos + (((width>>1) - 1 - (col>>1))<<1);
                                outYuv[dstUVPos]     = inYuv[srcUVPos++];
                                outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_270:  // 270°
                {
                    for(int row = 0; row < height; row++)   // 遍历所有行
                    {
                        yPos = height - 1 - row;
                        if (0 == (row&1))
                        {
                            uvPos = ((uvHeight - 1 - (row>>1))<<1);
                        }
                        for(int col = 0; col < width; col++)  // 遍历所有列
                        {
                            // 旋转 Y 数据
                            dstYPos         = col * height + yPos;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                            
                            // 旋转 UV 数据
                            if ((0 == (row&1)) && (0 == (col&1)))
                            {
                                dstUVPos             = len + col * uvHeight + uvPos;
                                outYuv[dstUVPos]     = inYuv[srcUVPos++];
                                outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                            }
                        }
                    }
                }
                    break;
                    
                case SYRotate_360:  // 360°
                {
                    memcpy(outYuv, inYuv, width * height * 1.5);
                }
                    break;
                    
                default:
                    break;
            }
        }
            break;
            
        default:
            break;
    }
    return SYErr_success;
}

#pragma mark -- YUV-NV21 旋转
SYKIT_API int SYRotate::nv21Rotate(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYRotateDirection direction, SYRotateDegree degree) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    
    return nv12Rotate(inYuv, width, height, outYuv, direction, degree);
}
