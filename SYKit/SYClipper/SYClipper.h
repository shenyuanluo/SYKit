//
//  SYClipper.h
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/8/17.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//


/*
 YUV 裁剪（指定区域）
 */

#ifndef SYClipper_h
#define SYClipper_h

#include <iostream>
#include "SYHeader.h"


class SYClipper
{
private:
    SYYuvType           m_yuvType;          // 裁剪 YUV 数据（格式）类型
    SYRgbType           m_rgbType;          // 裁剪 RGB 数据（格式）类型
    unsigned int        m_frameWidth;       // YUV/RGB 数据帧宽度
    unsigned int        m_frameHeight;      // YUV/RGB 数据帧高度
    unsigned int        m_pixelCount;       // 像素点数（= m_frameWidth × m_frameHeight）
    unsigned char*      m_srcY;             // 源数据·Y 平面
    unsigned char*      m_srcU;             // 源数据·U 平面
    unsigned char*      m_srcV;             // 源数据·V 平面
    unsigned char*      m_dstYuv;           // 裁剪后的 YUV 数据缓冲
    unsigned int        m_dstYuvLen;        // 裁剪后的 YUV 数据大小
    unsigned char*      m_dstRgb;           // 裁剪后的 RGB 数据缓冲
    unsigned int        m_dstRgbLen;        // 裁剪后的 RGB 数据大小
    
    /**
     初始化默认参数
     */
    void initParam();

    
    /**
     适配裁剪区域（保证 tlX <= brX；tlY <= brY）

     @param srcRect 源裁剪区域
     @return 适配后的裁剪区域
     */
    SYRect inline adaptRect(SYRect srcRect);
    
public:
    /**
     构造函数
     
     @param width 帧宽度
     @param height 帧高度
     */
    SYClipper(unsigned int width, unsigned int height);
    
    ~SYClipper();
    
    /**
     设置源数据帧大小

     @param width 帧宽度
     @param height 帧高度
     */
    SYKIT_API void SY_SetFrameSize(unsigned int width, unsigned int height);
    
    /**
     设置裁剪 YUV 数据格式类型（默认：I420）

     @param yuvType yuv 数据格式类型
     */
    SYKIT_API void SY_SetYuvType(SYYuvType yuvType);
    
    /**
     设置裁剪 RGB 数据格式类型（默认：RGB24）
     
     @param rgbType rgb 数据格式类型
     */
    SYKIT_API void SY_SetRgbType(SYRgbType rgbType);
    
    /**
     裁剪 YUV

     @param srcYuv 源 YUV 数据
     @param srcLen 源 YUV 数据大小
     @param dstYuv 裁剪后 YUV 数据数组
     @param dstLen 裁剪区域数据大小数组
     @param clipRect 裁剪区域数组，参见‘SYRect’
     @param clipCount 裁剪区域数量
     @return 裁剪是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_ClipYuv(unsigned char* srcYuv, unsigned int srcLen, unsigned char* dstYuv[0], unsigned int dstLen[0], SYRect clipRect[0], unsigned int clipCount);
    
    /**
     裁剪 RGB

     @param srcRgb 源 RGB 数据
     @param srcLen 源 RGB 数据大小
     @param dstRgb 裁剪后 RGB 数据数组
     @param dstLen 裁剪区域数据大小数据
     @param clipRect 裁剪区域数组，参见‘SYRect’
     @param clipCount 裁剪区域数量
     @return 裁剪是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_ClipRgb(unsigned char* srcRgb, unsigned int srcLen, unsigned char* dstRgb[0], unsigned int dstLen[0], SYRect clipRect[0], unsigned int clipCount);
    
};

#endif /* SYClipper_h */
