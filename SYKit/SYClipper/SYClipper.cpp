//
//  SYClipper.cpp
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/8/17.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "SYClipper.h"


#pragma mark - Public
#pragma mark -- 构造函数
SYClipper::SYClipper(unsigned int width, unsigned int height)
{
    m_frameWidth  = width;
    m_frameHeight = height;
    
    initParam();
}

#pragma mark -- 析构函数
SYClipper::~SYClipper()
{
    if (m_dstYuv)
    {
        free(m_dstYuv);
        m_dstYuv = NULL;
    }
}

#pragma mark -- 设置裁剪数据（格式）类型
SYKIT_API void SYClipper::SY_SetClipDataType(SYPixFmtType clipType)
{
    m_pixFmtType = clipType;
}

#pragma mark -- 裁剪 YUV
SYKIT_API int SYClipper::SY_ClipYuv(unsigned char* srcYuv, unsigned int srcLen, unsigned char* dstYuv[0], unsigned int dstLen[0], SYRect clipRect[0], unsigned int clipCount)
{
    if (NULL == srcYuv || 0 == srcLen || srcLen != m_frameDataLen || 0 == clipCount)
    {
        std::cout << "Can not clip yuv !" << std::endl;
        return SYErr_paramError;
    }
    
    unsigned int clipWidths[clipCount];     // 裁剪区域宽度
    unsigned int clipHeights[clipCount];    // 裁剪区域高度
    unsigned int clipPixelCount[clipCount]; // 裁剪区域像素点数
    unsigned char* clipY[clipCount];        // 裁剪区域 Y 平面
    unsigned char* clipU[clipCount];        // 裁剪区域 U 平面
    unsigned char* clipV[clipCount];        // 裁剪区域 V 平面
    
    m_srcY  = srcYuv;
    m_srcU  = m_srcY + m_pixelCount;
    m_srcV  = m_srcU + (m_pixelCount>>2);
    memset(m_dstYuv, 0, m_frameDataLen);
    
    for (int i = 0; i < clipCount; i++)
    {
        clipRect[i]       = adaptRect(clipRect[i]);
        clipWidths[i]     = clipRect[i].brX - clipRect[i].tlX;
        clipHeights[i]    = clipRect[i].brY - clipRect[i].tlY;
        clipPixelCount[i] = clipWidths[i] * clipHeights[i];
        dstLen[i]         = clipPixelCount[i] + (clipPixelCount[i]>>1);
        if (0 == i)
        {
            clipY[i] = m_dstYuv;
        }
        else
        {
            clipY[i] = clipY[i - 1] + dstLen[i - 1];
        }
        clipU[i]  = clipY[i] + clipPixelCount[i];
        clipV[i]  = clipU[i] + (clipPixelCount[i]>>2);
        dstYuv[i] = clipY[i];
    }
    
    for (int row = 0; row < m_frameHeight; row++)  // 遍历所有行
    {
        for (int j = 0; j < clipCount; j++)
        {
            if (row <= clipRect[j].tlY || row >= clipRect[j].brY)
            {
                continue;
            }
            int col = clipRect[j].tlX;
            memcpy(clipY[j], m_srcY + row * m_frameWidth + col, clipWidths[j]);    // copy Y
            memcpy(clipU[j], m_srcU + (row>>1) * (m_frameWidth>>1) + (col>>1), (clipWidths[j]>>1));  // copy U
            memcpy(clipV[j], m_srcV + (row>>1) * (m_frameWidth>>1) + (col>>1), (clipWidths[j]>>1));  // copy V
            
            clipY[j] += clipWidths[j];
            if (0 == (row & 1)) // 偶数
            {
                clipU[j] += (clipWidths[j]>>1);
                clipV[j] += (clipWidths[j]>>1);
            }
        }
    }
    
    return SYErr_success;
}


#pragma mark - Private
#pragma mark -- 初始化参数
void SYClipper::initParam()
{
    m_pixFmtType   = SYPixFmt_i420;
    m_pixelCount   = m_frameWidth * m_frameHeight;
    m_frameDataLen = m_pixelCount + (m_pixelCount>>1);
    m_srcY         = NULL;
    m_srcU         = NULL;
    m_srcV         = NULL;
    m_dstYuv       = (unsigned char*)malloc(m_frameDataLen);
    memset(m_dstYuv, 0, m_frameDataLen);
}

#pragma mark -- 适配裁剪区域
SYRect inline SYClipper::adaptRect(SYRect srcRect)
{
    SYRect dstRect;
    // 适配 X
    if (srcRect.tlX <= srcRect.brX)
    {
        dstRect.tlX = srcRect.tlX;
        dstRect.brX = srcRect.brX;
    }
    else
    {
        dstRect.tlX = srcRect.brX;
        dstRect.brX = srcRect.tlX;
    }
    // 适配 Y
    if (srcRect.tlY <= srcRect.brY)
    {
        dstRect.tlY = srcRect.tlY;
        dstRect.brY = srcRect.brY;
    }
    else
    {
        dstRect.tlY = srcRect.brY;
        dstRect.brY = srcRect.tlY;
    }
    // 像素点不能为奇数，否则会黑屏
    if (0 != ((dstRect.brX - dstRect.tlX) & 1))
    {
        dstRect.brX--;
    }
    if (0 != ((dstRect.brY - dstRect.tlY) & 1))
    {
        dstRect.brY--;
    }
    
    return dstRect;
}
