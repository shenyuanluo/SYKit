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
    SY_SetFrameSize(width, height);
    
    initParam();
}

#pragma mark -- 析构函数
SYClipper::~SYClipper()
{
    if (m_srcY)
    {
        free(m_srcY);
        m_srcY = NULL;
    }
    if (m_srcU)
    {
        free(m_srcU);
        m_srcU = NULL;
    }
    if (m_srcV)
    {
        free(m_srcV);
        m_srcV = NULL;
    }
    if (m_dstYuv)
    {
        free(m_dstYuv);
        m_dstYuv = NULL;
    }
    if (m_dstRgb)
    {
        free(m_dstRgb);
        m_dstRgb = NULL;
    }
}

#pragma mark -- 设置源数据帧大小
SYKIT_API void SYClipper::SY_SetFrameSize(unsigned int width, unsigned int height)
{
    m_frameWidth  = width;
    m_frameHeight = height;
}

#pragma mark -- 设置 YUV 数据（格式）类型
SYKIT_API void SYClipper::SY_SetYuvType(SYYuvType yuvType)
{
    m_yuvType = yuvType;
}

#pragma mark -- 设置 RGB 数据（格式）类型
SYKIT_API void SYClipper::SY_SetRgbType(SYRgbType rgbType)
{
    m_rgbType = rgbType;
    if (m_dstRgb)
    {
        free(m_dstRgb);
        m_dstRgb = NULL;
    }
    switch (m_rgbType)
    {
        case SYRgb_rgb555:
        {
            m_dstRgbLen = (m_pixelCount<<1);
        }
            break;
            
        case SYRgb_rgb565:
        {
            m_dstRgbLen = (m_pixelCount<<1);
        }
            break;
            
        case SYRgb_rgb24:
        {
            m_dstRgbLen = m_pixelCount + (m_pixelCount<<1);
        }
            break;
            
        case SYRgb_rgb32:
        {
            m_dstRgbLen = (m_pixelCount<<2);
        }
            break;
            
        default:
            break;
    }
    m_dstRgb = (unsigned char*)malloc(m_dstRgbLen);
    memset(m_dstRgb, 0, m_dstRgbLen);
}

#pragma mark -- 裁剪 YUV
SYKIT_API int SYClipper::SY_ClipYuv(unsigned char* srcYuv, unsigned int srcLen, unsigned char* dstYuv[0], unsigned int dstLen[0], SYRect clipRect[0], unsigned int clipCount)
{
    if (NULL == srcYuv || 0 == srcLen || srcLen != m_dstYuvLen || 0 == clipCount)
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
    
    // 处理裁剪区域坐标
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
        
        dstYuv[i] = clipY[i];
        
        switch (m_yuvType)
        {
            case SYYuv_i420:
            {
                clipU[i] = clipY[i] + clipPixelCount[i];
                clipV[i] = clipU[i] + (clipPixelCount[i]>>2);
                
            }
                break;
                
            case SYYuv_nv12:
            {
                clipU[i] = clipY[i] + clipPixelCount[i];
            }
                break;
                
            case SYYuv_nv21:
            {
                clipV[i] = clipY[i] + clipPixelCount[i];
            }
                break;
                
            default:
                break;
        }
    }
    
    switch (m_yuvType)
    {
        case SYYuv_i420:
        {
            m_srcY  = srcYuv;
            m_srcU  = m_srcY + m_pixelCount;
            m_srcV  = m_srcU + (m_pixelCount>>2);
        }
            break;
            
        case SYYuv_nv12:
        {
            m_srcY  = srcYuv;
            m_srcU  = m_srcY + m_pixelCount;
        }
            break;
            
        case SYYuv_nv21:
        {
            m_srcY  = srcYuv;
            m_srcV  = m_srcY + m_pixelCount;
        }
            break;
            
        default:
            break;
    }
    
    memset(m_dstYuv, 0, m_dstYuvLen);
    
    for (int row = 0; row < m_frameHeight; row++)  // 遍历所有行
    {
        for (int j = 0; j < clipCount; j++) // 遍历所有裁剪区域
        {
            if (row <= clipRect[j].tlY || row >= clipRect[j].brY)
            {
                continue;
            }
            int col = clipRect[j].tlX;
            switch (m_yuvType)
            {
                case SYYuv_i420:
                {
                    memcpy(clipY[j], m_srcY + row * m_frameWidth + col, clipWidths[j]);    // copy Y
                    clipY[j] += clipWidths[j];
                    
                    if (0 == (row & 1)) // 偶数行
                    {
                        memcpy(clipU[j], m_srcU + (row>>1) * (m_frameWidth>>1) + (col>>1), (clipWidths[j]>>1));  // copy U
                        memcpy(clipV[j], m_srcV + (row>>1) * (m_frameWidth>>1) + (col>>1), (clipWidths[j]>>1));  // copy V
                        clipU[j] += (clipWidths[j]>>1);
                        clipV[j] += (clipWidths[j]>>1);
                    }
                }
                    break;
                    
                case SYYuv_nv12:
                {
                    memcpy(clipY[j], m_srcY + row * m_frameWidth + col, clipWidths[j]);    // copy Y
                    clipY[j] += clipWidths[j];
                    
                    if (0 == (row & 1)) // 偶数行
                    {
                        memcpy(clipU[j], m_srcU + (row>>1) * m_frameWidth + col, clipWidths[j]);  // copy UV
                        clipU[j] += (clipWidths[j]);
                    }
                }
                    break;
                    
                case SYYuv_nv21:
                {
                    memcpy(clipY[j], m_srcY + row * m_frameWidth + col, clipWidths[j]);    // copy Y
                    clipY[j] += clipWidths[j];
                    
                    if (0 == (row & 1)) // 偶数行
                    {
                        memcpy(clipV[j], m_srcV + (row>>1) * m_frameWidth + col, clipWidths[j]);  // copy VU
                        clipV[j] += (clipWidths[j]);
                    }
                }
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    return SYErr_success;
}

#pragma mark -- 裁剪 RGB
SYKIT_API int SYClipper::SY_ClipRgb(unsigned char* srcRgb, unsigned int srcLen, unsigned char* dstRgb[0], unsigned int dstLen[0], SYRect clipRect[0], unsigned int clipCount)
{
    if (NULL == srcRgb || 0 == srcLen || srcLen != m_dstRgbLen || 0 == clipCount)
    {
        std::cout << "Can not clip rgb !" << std::endl;
        return SYErr_paramError;
    }

    unsigned int clipWidths[clipCount];     // 裁剪区域宽度
    unsigned int clipHeights[clipCount];    // 裁剪区域高度
    unsigned int clipPixelCount[clipCount]; // 裁剪区域像素点数
    unsigned int clipBytePerRow[clipCount]; // 每行裁剪的字节数
    unsigned char* clipRgb[clipCount];      // 裁剪区域 RGB 数据
    
    memset(m_dstRgb, 0, m_dstRgbLen);

    for (int i = 0; i < clipCount; i++)
    {
        clipRect[i]       = adaptRect(clipRect[i]);
        clipWidths[i]     = clipRect[i].brX - clipRect[i].tlX;
        clipHeights[i]    = clipRect[i].brY - clipRect[i].tlY;
        clipPixelCount[i] = clipWidths[i] * clipHeights[i];
        switch (m_rgbType)
        {
            case SYRgb_rgb555:
            {
                dstLen[i]         = (clipPixelCount[i]<<1);
                clipBytePerRow[i] = (clipWidths[i]<<1);
            }
                break;
                
            case SYRgb_rgb565:
            {
                dstLen[i]         = (clipPixelCount[i]<<1);
                clipBytePerRow[i] = (clipWidths[i]<<1);
            }
                break;
                
            case SYRgb_rgb24:
            {
                dstLen[i]         = clipPixelCount[i] + (clipPixelCount[i]<<1);
                clipBytePerRow[i] = clipWidths[i] + (clipWidths[i]<<1);
                
            }
                break;
                
            case SYRgb_rgb32:
            {
                dstLen[i]         = (clipPixelCount[i]<<2);
                clipBytePerRow[i] = (clipWidths[i]<<2);
            }
                break;
                
            default:
                break;
        }
        
        if (0 == i)
        {
            clipRgb[i] = m_dstRgb;
        }
        else
        {
            clipRgb[i] = clipRgb[i - 1] + dstLen[i - 1];
        }
        dstRgb[i] = clipRgb[i];
    }

    for (int row = 0; row < m_frameHeight; row++)  // 遍历所有行
    {
        for (int j = 0; j < clipCount; j++)
        {
            if (row <= clipRect[j].tlY || row >= clipRect[j].brY)
            {
                continue;
            }
            int col       = clipRect[j].tlX;
            int moveSpace = 0;
            switch (m_rgbType)
            {
                case SYRgb_rgb555:
                {
                    moveSpace = ((row * m_frameWidth + col)<<1);
                }
                    break;
                    
                case SYRgb_rgb565:
                {
                    moveSpace = ((row * m_frameWidth + col)<<1);
                }
                    break;
                    
                case SYRgb_rgb24:
                {
                    moveSpace = ((row * m_frameWidth + col) + ((row * m_frameWidth + col)<<1));
                }
                    break;
                    
                case SYRgb_rgb32:
                {
                    moveSpace = ((row * m_frameWidth + col)<<2);
                    
                }
                    break;
                    
                default:
                    break;
            }
            memcpy(clipRgb[j], srcRgb + moveSpace, clipBytePerRow[j]);  // copy rgb
            clipRgb[j] += clipBytePerRow[j];
        }
    }
    
    return SYErr_success;
}


#pragma mark - Private
#pragma mark -- 初始化参数
void SYClipper::initParam()
{
    m_yuvType      = SYYuv_i420;
    m_rgbType      = SYRgb_rgb24;
    m_pixelCount   = m_frameWidth * m_frameHeight;
    m_dstYuvLen    = m_pixelCount + (m_pixelCount>>1);
    m_dstRgbLen    = m_pixelCount + (m_pixelCount<<1);
    m_srcY         = NULL;
    m_srcU         = NULL;
    m_srcV         = NULL;
    m_dstYuv       = (unsigned char*)malloc(m_dstYuvLen);
    m_dstRgb       = (unsigned char*)malloc(m_dstRgbLen);
    memset(m_dstYuv, 0, m_dstYuvLen);
    memset(m_dstRgb, 0, m_dstRgbLen);
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
    // 保证裁剪区域 X 坐标为偶数，否则裁剪出来的 YUV-nv12 和 YUV-nv21 会出现花屏（像素错乱）现象，原因暂未知
    // 起初以为是宽度要 4 的倍数（跟 BMP 一样），经验证，并不是此原因；遗留。。。。
    if (0 != (dstRect.tlX & 1))
    {
        dstRect.tlX++;
    }
    // 保证裁剪区域 X 坐标为偶数，否则裁剪出来的 YUV-nv12 和 YUV-nv21 会出现花屏（像素错乱）现象，原因暂未知
    // 起初以为是宽度要 4 的倍数（跟 BMP 一样），经验证，并不是此原因；遗留。。。。
    if (0 != (dstRect.brX & 1))
    {
        dstRect.brX--;
    }
    
    return dstRect;
}
