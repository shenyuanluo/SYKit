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
            i420Rotate(inYuv, width, height, outYuv, direction, degree);
        }
            break;
            
        case SYYuv_nv12:
        {
            nv12Rotate(inYuv, width, height, outYuv, direction, degree);
        }
            break;
            
        case SYYuv_nv21:
        {
            nv21Rotate(inYuv, width, height, outYuv, direction, degree);
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
    const int len          = width * height; // 像素点数
    unsigned int uWidth    = (width>>1);     // U 数据宽度
    unsigned int uHeight   = (height>>1);    // U 数据高度
    unsigned int vWidth    = (width>>1);     // V 数据宽度
    unsigned int vHeight   = (height>>1);    // V 数据高度
    unsigned int srcYPos   = 0;      // 数据源（Y 数据）位置
    unsigned int srcUPos   = len;    // 数据源（U 数据）位置
    unsigned int srcVPos   = len + (len>>2);    // 数据源（V 数据）位置
    unsigned int dstYPos   = 0;      // 目标（Y 数据）位置
    unsigned int dstUPos   = 0;      // 目标（U 数据）位置
    unsigned int dstVPos   = 0;      // 目标（V 数据）位置
    unsigned int newYRow   = 0;                // 旋转后的 Y 数据 行下标
    unsigned int newYCol   = 0;                // 旋转后的 Y 数据 列 下标
    unsigned int newURow   = 0;                // 旋转后的 UV 数据 行 下标
    unsigned int newVRow   = 0;                // 旋转后的 V 数据 行 下标
    unsigned int newUCol   = 0;                // 旋转后的 UV 数据 列 下标
    unsigned int newVCol  = 0;                // 旋转后的 V 数据 列 下标
    unsigned int newYWidth = height;           // 旋转后 U 数据宽度
    unsigned int newUWidth = uHeight;         // 旋转后 U 数据高度
    unsigned int newVWidth = vHeight;         // 旋转后 U 数据高度
    
    switch (direction)  // 旋转方向
    {
        case SYRotate_clockwise:    // 顺时针
        {
            switch (degree) // 旋转角度
            {
                case SYRotate_90:   // 90°
                {
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYCol = height - 1 - oldRow;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYRow         = oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 U 数据
                    for (int oldRow = 0; oldRow < uHeight; oldRow++)
                    {
                        newUCol = uHeight - 1 - oldRow;
                        for (int oldCol = 0; oldCol < uWidth; oldCol++)
                        {
                            newURow             = oldCol;
                            dstUPos             = len + newURow * newUWidth + newUCol;
                            outYuv[dstUPos]     = inYuv[srcUPos++];
                        }
                    }
                    // 旋转 V 数据
                    for (int oldRow = 0; oldRow < vHeight; oldRow++)
                    {
                        newVCol = vHeight - 1 - oldRow;
                        for (int oldCol = 0; oldCol < vWidth; oldCol++)
                        {
                            newVRow             = oldCol;
                            dstVPos             = len + (len>>2) + newVRow * newVWidth + newVCol;
                            outYuv[dstVPos]     = inYuv[srcVPos++];
                        }
                    }
                }
                    break;
                    
                case SYRotate_180:  // 180°
                {
                    newYWidth   = width;
                    newUWidth  = uWidth;
                    newVWidth  = vWidth;
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYRow = (height - 1 - oldRow) * 1;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYCol         = width - 1 - oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 U 数据
                    for (int oldRow = 0; oldRow < uHeight; oldRow++)
                    {
                        newURow = uHeight - 1 - oldRow;
                        for (int oldCol = 0; oldCol < uWidth; oldCol++)
                        {
                            newUCol             = uWidth - 1 - oldCol;
                            dstUPos             = len + newURow * newUWidth + newUCol;
                            outYuv[dstUPos]     = inYuv[srcUPos++];
                        }
                    }
                    // 旋转 U 数据
                    for (int oldRow = 0; oldRow < vHeight; oldRow++)
                    {
                        newVRow = vHeight - 1 - oldRow;
                        for (int oldCol = 0; oldCol < vWidth; oldCol++)
                        {
                            newVCol             = vWidth - 1 - oldCol;
                            dstVPos             = len + (len>>2) + newVRow * newVWidth + newVCol;
                            outYuv[dstVPos]     = inYuv[srcVPos++];
                        }
                    }
                }
                    break;
                    
                case SYRotate_270:  // 270°
                {
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYCol = oldRow;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYRow         = width - 1 - oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 U 数据
                    for (int oldRow = 0; oldRow < uHeight; oldRow++)
                    {
                        newUCol = oldRow;
                        for (int oldCol = 0; oldCol < uWidth; oldCol++)
                        {
                            newURow             = uWidth - 1 - oldCol;
                            dstUPos             = len + newURow * newUWidth + newUCol;
                            outYuv[dstUPos]     = inYuv[srcUPos++];
                        }
                    }
                    // 旋转 V 数据
                    for (int oldRow = 0; oldRow < vHeight; oldRow++)
                    {
                        newVCol = oldRow;
                        for (int oldCol = 0; oldCol < vWidth; oldCol++)
                        {
                            newVRow             = vWidth - 1 - oldCol;
                            dstVPos             = len + (len>>2) + newVRow * newVWidth + newVCol;
                            outYuv[dstVPos]     = inYuv[srcVPos++];
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
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYCol = oldRow;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYRow         = width - 1 - oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 U 数据
                    for (int oldRow = 0; oldRow < uHeight; oldRow++)
                    {
                        newUCol = oldRow;
                        for (int oldCol = 0; oldCol < uWidth; oldCol++)
                        {
                            newURow             = uWidth - 1 - oldCol;
                            dstUPos             = len + newURow * newUWidth + newUCol;
                            outYuv[dstUPos]     = inYuv[srcUPos++];
                        }
                    }
                    // 旋转 V 数据
                    for (int oldRow = 0; oldRow < vHeight; oldRow++)
                    {
                        newVCol = oldRow;
                        for (int oldCol = 0; oldCol < vWidth; oldCol++)
                        {
                            newVRow             = vWidth - 1 - oldCol;
                            dstVPos             = len + (len>>2) + newVRow * newVWidth + newVCol;
                            outYuv[dstVPos]     = inYuv[srcVPos++];
                        }
                    }
                }
                    break;
                    
                case SYRotate_180:  // 180°
                {
                    newYWidth   = width;
                    newUWidth  = uWidth;
                    newVWidth  = vWidth;
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYRow = (height - 1 - oldRow) * 1;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYCol         = width - 1 - oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 U 数据
                    for (int oldRow = 0; oldRow < uHeight; oldRow++)
                    {
                        newURow = uHeight - 1 - oldRow;
                        for (int oldCol = 0; oldCol < uWidth; oldCol++)
                        {
                            newUCol             = uWidth - 1 - oldCol;
                            dstUPos             = len + newURow * newUWidth + newUCol;
                            outYuv[dstUPos]     = inYuv[srcUPos++];
                        }
                    }
                    // 旋转 U 数据
                    for (int oldRow = 0; oldRow < vHeight; oldRow++)
                    {
                        newVRow = vHeight - 1 - oldRow;
                        for (int oldCol = 0; oldCol < vWidth; oldCol++)
                        {
                            newVCol             = vWidth - 1 - oldCol;
                            dstVPos             = len + (len>>2) + newVRow * newVWidth + newVCol;
                            outYuv[dstVPos]     = inYuv[srcVPos++];
                        }
                    }
                }
                    break;
                    
                case SYRotate_270:  // 270°
                {
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYCol = height - 1 - oldRow;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYRow         = oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 U 数据
                    for (int oldRow = 0; oldRow < uHeight; oldRow++)
                    {
                        newUCol = uHeight - 1 - oldRow;
                        for (int oldCol = 0; oldCol < uWidth; oldCol++)
                        {
                            newURow             = (oldCol<<0);
                            dstUPos             = len + newURow * newUWidth + newUCol;
                            outYuv[dstUPos]     = inYuv[srcUPos++];
                        }
                    }
                    // 旋转 V 数据
                    for (int oldRow = 0; oldRow < vHeight; oldRow++)
                    {
                        newVCol = vHeight - 1 - oldRow;
                        for (int oldCol = 0; oldCol < vWidth; oldCol++)
                        {
                            newVRow             = (oldCol<<0);
                            dstVPos             = len + (len>>2) + newVRow * newVWidth + newVCol;
                            outYuv[dstVPos]     = inYuv[srcVPos++];
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
    const int len           = width * height;   // 像素点数
    unsigned int uvWidth    = (width>>1);       // UV 数据宽度
    unsigned int uvHeight   = (height>>1);      // UV 数据高度
    unsigned int srcYPos    = 0;                // 数据源（Y 数据）位置
    unsigned int srcUVPos   = len;              // 数据源（UV 数据）位置
    unsigned int dstYPos    = 0;                // 目标（Y 数据）位置
    unsigned int dstUVPos   = 0;                // 目标（UV 数据）位置
    unsigned int newYRow    = 0;                // 旋转后的 Y 数据 行下标
    unsigned int newYCol    = 0;                // 旋转后的 Y 数据 列 下标
    unsigned int newUVRow   = 0;                // 旋转后的 UV 数据 行 下标
    unsigned int newUVCol   = 0;                // 旋转后的 UV 数据 列 下标
    unsigned int newYWidth  = height;           // 旋转后 UV 数据宽度
    unsigned int newUVWidth = uvHeight;         // 旋转后 UV 数据高度
    
    switch (direction)  // 旋转方向
    {
        case SYRotate_clockwise:    // 顺时针
        {
            switch (degree) // 旋转角度
            {
                case SYRotate_90:   // 90°
                {
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYCol = height - 1 - oldRow;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYRow         = oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 UV 数据
                    for (int oldRow = 0; oldRow < uvHeight; oldRow++)
                    {
                        newUVCol = ((uvHeight - 1 - oldRow)<<1);
                        for (int oldCol = 0; oldCol < uvWidth; oldCol++)
                        {
                            newUVRow             = (oldCol<<1);
                            dstUVPos             = len + newUVRow * newUVWidth + newUVCol;
                            outYuv[dstUVPos]     = inYuv[srcUVPos++];
                            outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                        }
                    }
                }
                    break;
                    
                case SYRotate_180:  // 180°
                {
                    newYWidth   = width;
                    newUVWidth  = uvWidth;
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYRow = (height - 1 - oldRow) * 1;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYCol         = width - 1 - oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 UV 数据
                    for (int oldRow = 0; oldRow < uvHeight; oldRow++)
                    {
                        newUVRow = ((uvHeight - 1 - oldRow)<<1);
                        for (int oldCol = 0; oldCol < uvWidth; oldCol++)
                        {
                            newUVCol             = ((uvWidth - 1 - oldCol) <<1);
                            dstUVPos             = len + newUVRow * newUVWidth + newUVCol;
                            outYuv[dstUVPos]     = inYuv[srcUVPos++];
                            outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                        }
                    }
                }
                    break;
                    
                case SYRotate_270:  // 270°
                {
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYCol = oldRow;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYRow         = width - 1 - oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 UV 数据
                    for (int oldRow = 0; oldRow < uvHeight; oldRow++)
                    {
                        newUVCol = (oldRow<<1);
                        for (int oldCol = 0; oldCol < uvWidth; oldCol++)
                        {
                            newUVRow             = ((uvWidth - 1 - oldCol)<<1);
                            dstUVPos             = len + newUVRow * newUVWidth + newUVCol;
                            outYuv[dstUVPos]     = inYuv[srcUVPos++];
                            outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
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
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYCol = oldRow;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYRow         = width - 1 - oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 UV 数据
                    for (int oldRow = 0; oldRow < uvHeight; oldRow++)
                    {
                        newUVCol = (oldRow<<1);
                        for (int oldCol = 0; oldCol < uvWidth; oldCol++)
                        {
                            newUVRow             = ((uvWidth - 1 - oldCol)<<1);
                            dstUVPos             = len + newUVRow * newUVWidth + newUVCol;
                            outYuv[dstUVPos]     = inYuv[srcUVPos++];
                            outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                        }
                    }
                }
                    break;
                    
                case SYRotate_180:  // 180°
                {
                    newYWidth   = width;
                    newUVWidth  = uvWidth;
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYRow = (height - 1 - oldRow) * 1;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYCol         = width - 1 - oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 UV 数据
                    for (int oldRow = 0; oldRow < uvHeight; oldRow++)
                    {
                        newUVRow = ((uvHeight - 1 - oldRow)<<1);
                        for (int oldCol = 0; oldCol < uvWidth; oldCol++)
                        {
                            newUVCol             = ((uvWidth - 1 - oldCol) <<1);
                            dstUVPos             = len + newUVRow * newUVWidth + newUVCol;
                            outYuv[dstUVPos]     = inYuv[srcUVPos++];
                            outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
                        }
                    }
                }
                    break;
                    
                case SYRotate_270:  // 270°
                {
                    // 旋转 Y 数据
                    for(int oldRow = 0; oldRow < height; oldRow++)   // 遍历所有行
                    {
                        newYCol = height - 1 - oldRow;
                        for(int oldCol = 0; oldCol < width; oldCol++)  // 遍历所有列
                        {
                            newYRow         = oldCol;
                            dstYPos         = newYRow * newYWidth + newYCol;
                            outYuv[dstYPos] = inYuv[srcYPos++];
                        }
                    }
                    // 旋转 UV 数据
                    for (int oldRow = 0; oldRow < uvHeight; oldRow++)
                    {
                        newUVCol = ((uvHeight - 1 - oldRow)<<1);
                        for (int oldCol = 0; oldCol < uvWidth; oldCol++)
                        {
                            newUVRow             = (oldCol<<1);
                            dstUVPos             = len + newUVRow * newUVWidth + newUVCol;
                            outYuv[dstUVPos]     = inYuv[srcUVPos++];
                            outYuv[dstUVPos + 1] = inYuv[srcUVPos++];
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
