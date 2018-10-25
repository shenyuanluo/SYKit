//
//  SYYuvToBmp.h
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/7/20.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//


/*
 YUV 保存为 BMP 图像
 */

#ifndef SYYuvToBmp_h
#define SYYuvToBmp_h

#include <iostream>
#include "SYHeader.h"


class SYYuvToBmp
{
private:
    SYMatrixType  m_mType;      // 转换矩阵类型
    SYConvertType m_cType;      // 转换方法类型
    SYEndianType  m_eType;      // 大小端类型
    SYYuvType     m_yuvType;    // YUV 数据格式类型
    
    int m_rv[256];  // 计算 R 值使用的 V 表
    int m_gu[256];  // 计算 G 值使用的 U 表
    int m_gv[256];  // 计算 G 值使用的 V 表
    int m_bu[256];  // 计算 B 值使用的 U 表
    
    unsigned int m_bmpHeaderLen;    // BMP 头长度
    unsigned int m_bmpFileInfoLen;  // BMP 文件信息长度
    
    /**
     检查大小端
     
     @return 大小端类型
     */
    SYEndianType checkEndian();
    
    /**
     初始化 YUV 颜色表
     */
    void initYuvTable();
    
    /**
     将单个像素 YUV 转成 RGB
     
     @param y 像素点的 Y 值（取值范围：0~255）
     @param u 像素点的 U(Cb) 值（取值范围：-128~127）
     @param v 像素点的 V(Cr) 值（取值范围：-128~127）
     @param r 像素点的 R 值（取值范围：0~255）
     @param g 像素点的 G 值（取值范围：0~255）
     @param b 像素点的 B 值（取值范围：0~255）
     */
    void yuv2rgb(int y, int u, int v, int* r, int* g, int* b) const;
    
    
public:
    SYYuvToBmp();
    
    ~SYYuvToBmp();
    
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
     设置 YUV 数据格式类型（默认：I420）

     @param yuvType yuv 数据类型
     */
    SYKIT_API void SY_SetYuvType(SYYuvType yuvType);
    
    /**
     YUV 转 BMP
     
     @param inYuv YUV 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outBmpPath BMP 文件路径（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_YuvToBmp(unsigned char* inYuv, unsigned int width, unsigned int height, const char* outBmpPath) const;
};

#endif /* SYYuvToBmp_h */
