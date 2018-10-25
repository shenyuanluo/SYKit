//
//  SYRgbToYuv.h
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/7/15.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//


/*
 RGB 转 YUV
 */

#ifndef SYRgbToYuv_h
#define SYRgbToYuv_h

#include <iostream>
#include "SYHeader.h"


/* ========== RGB --> YUV Convert Matrix ==========
 1、常规转换标准
 __ __     __                               __ __ __
 | Y'|     |  0.29882,   0.58681,   0.114363 | | R |
 | U |  =  | -0.172485, -0.338718,  0.511207 | | G |
 | V |     |  0.51155,  -0.42811,  -0.08343  | | B |
 -- --     --                               -- -- --
 
 2、标清电视标准：BT.601
 __ __     __                            __ __ __
 | Y'|     |  0.299,    0.587,    0.114   | | R |
 | U |  =  | -0.14713, -0.28886,  0.436   | | G |
 | V |     |  0.615,   -0.51499, -0.10001 | | B |
 -- --     --                            -- -- --
 
 3、高清电视标准：BT.709
 __ __     __                            __ __ __
 | Y |     |  0.2126,   0.7152,   0.0722  | | R |
 | U |  =  | -0.09991, -0.33609,  0.436   | | G |
 | V |     |  0.615,   -0.55861, -0.05639 | | B |
 -- --     --                            -- -- --
 
 */

class SYRgbToYuv
{
private:
    SYMatrixType  m_mType;    // 转换矩阵类型
    SYConvertType m_cType;    // 转换方法类型
    SYEndianType  m_eType;    // 大小端类型
    
    int m_yr[256];  // 计算 Y 值使用的 R 表
    int m_yg[256];  // 计算 Y 值使用的 G 表
    int m_yb[256];  // 计算 Y 值使用的 B 表
    int m_ur[256];  // 计算 U 值使用的 R 表
    int m_ug[256];  // 计算 U 值使用的 G 表
    int m_ub[256];  // 计算 U 值使用的 B 表
    int m_vr[256];  // 计算 V 值使用的 R 表
    int m_vg[256];  // 计算 V 值使用的 G 表
    int m_vb[256];  // 计算 V 值使用的 B 表
    
    /**
     检查大小端
     
     @return 大小端类型
     */
    SYEndianType checkEndian() const;
    
    /**
     初始化 RGB 颜色表
     */
    void initRgbTable();
    
    /**
     将单个像素 RGB 转成 YUV
     
     @param r 像素点的 R 值（取值范围：0~255）
     @param g 像素点的 G 值（取值范围：0~255）
     @param b 像素点的 B 值（取值范围：0~255）
     @param y 像素点的 Y 值（取值范围：0~255）
     @param u 像素点的 U(Cb) 值（取值范围：0~255）
     @param v 像素点的 V(Cr) 值（取值范围：0~255）
     */
    void rgb2yuv(int r, int g, int b, int* y, int* u, int* v) const;
    
public:
    SYRgbToYuv();
    
    ~SYRgbToYuv();
    
    /**
     设置转换方法类型
     
     @param cType 转换方法类型
     */
    SYKIT_API void SY_SetConvertType(SYConvertType cType);
    
    /**
     设置转换矩阵类型
     
     @param mType 转换矩阵类型
     */
    SYKIT_API void SY_SetMatrixType(SYMatrixType mType);
    
    /**
     RGB565 转 I420
     
     @param inRgb RGB565 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv I420 数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Rgb565ToI420(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     RGB565 转 NV12
     
     @param inRgb RGB565 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv NV12 数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Rgb565ToNv12(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     RGB565 转 NV21
     
     @param inRgb RGB565 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv NV21 数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Rgb565ToNv21(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     RGB24 转 I420

     @param inRgb RGB24 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv I420 数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Rgb24ToI420(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     RGB24 转 NV12
     
     @param inRgb RGB24 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv NV12 数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Rgb24ToNv12(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
    /**
     RGB24 转 NV21
     
     @param inRgb RGB24 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv NV21 数据（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Rgb24ToNv21(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const;
    
};

#endif /* SYRgbToYuv_h */
