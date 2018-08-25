//
//  SYRgbToBmp.cpp
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/7/19.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "SYRgbToBmp.h"
#include <unistd.h>


#pragma mark - Public
#pragma mark -- 构造函数
SYRgbToBmp::SYRgbToBmp()
{
    m_bmpHeaderLen   = sizeof(SYBitMapHeader);
    m_bmpFileInfoLen = sizeof(SYBitMapFileInfo);
    m_eType          = checkEndian();
}

#pragma mark -- 析构函数
SYRgbToBmp::~SYRgbToBmp()
{
    
}

#pragma mark -- RGB565 转 BMP
SYKIT_API int SYRgbToBmp::SY_Rgb565ToBmp(unsigned char *inRgb, unsigned int width, unsigned int height, const char *outBmpPath) const
{
    if (NULL == inRgb || NULL == outBmpPath || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    FILE* fp_bmp = fopen(outBmpPath, "wb+");    // 打开 BMP 文件
    if(NULL == fp_bmp)
    {
        printf("Can not open output BMP file.\n");
        return SYErr_failure;
    }
    unsigned char bfType[2] = {'B', 'M'};  // BMP 文件类型标志
    unsigned int typeLen    = sizeof(bfType);
    unsigned int headerSize = typeLen + m_bmpHeaderLen + m_bmpFileInfoLen; // 整个文件头部长度
    unsigned int datalen    = width * height * 3;  // RGB24 数据长度
    
    SYBitMapHeader   m_BMPHeader   = {0};     // BMP 头结构
    SYBitMapFileInfo m_BMPFileInfo = {0};     // BMP 文件信息
    
    m_BMPHeader.bfSize        = datalen + headerSize;
    m_BMPHeader.bfbfOffBits   = headerSize;
    m_BMPFileInfo.biSize      = m_bmpFileInfoLen;
    m_BMPFileInfo.biWidth     = width;
    m_BMPFileInfo.biHeight    = -height; // BMP 存储像素数据与y轴方向相反（即，位图是底朝上的）。
    m_BMPFileInfo.biPlanes    = 1;
    m_BMPFileInfo.biBitcount  = 24;
    m_BMPFileInfo.biSizeImage = datalen;
    
    fwrite(bfType, 1, typeLen, fp_bmp); // 先写入 BMP 文件类型标志：'BM'
    fwrite(&m_BMPHeader, 1, m_bmpHeaderLen, fp_bmp);  // 写入 BMP 头
    fwrite(&m_BMPFileInfo, 1, m_bmpFileInfoLen, fp_bmp);  // 写入 BMP 文件信息
    
    unsigned char colorLow;     // 当前像素 rgb565 颜色值-低字节
    unsigned char colorHigh;    // 当前像素 rgb565 颜色值-高字节
    
    unsigned char r, g, b;      // 当前像素点 R、G、B 值
    unsigned int curPos = 0;    // 当前像素点 RGB 内存位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            curPos = (row * width + col) * 2;
            
            // 提取每个像素点 RGB 值
            switch (m_eType)
            {
                case SYEndian_little: // 小端
                {
                    colorLow  = inRgb[curPos];
                    colorHigh = inRgb[curPos + 1];
                }
                    break;
                    
                case SYEndian_big:    // 大端
                {
                    colorLow  = inRgb[curPos + 1];
                    colorHigh = inRgb[curPos];
                }
                    break;
                    
                default:
                    break;
            }
            
            // 提取颜色值 R、G、B 分量
            r = colorHigh & 0xF8;       // 获取高字节高 5 个bit，作为 R 的高 5 位
            g = (colorHigh & 0x07)<<5;  // 获取高字节低 3 个bit，作为 G 的高 3 位
            g |= (colorLow & 0xE0)>>3;  // 获取低字节高 3 个bit，作为 G 的中 3 位
            b = (colorLow & 0x1F)<<3;   // 获取低字节高 5 个bit，作为 B 的高 5 位
            
            // BMP 数据存储形式： B1|G1|R1,B2|G2|R2
            fwrite(&b, 1, 1, fp_bmp);   // 写入 B 数据
            fwrite(&g, 1, 1, fp_bmp);   // 写入 G 数据
            fwrite(&r, 1, 1, fp_bmp);   // 写入 R 数据
        }
    }
    fclose(fp_bmp);
    fp_bmp = NULL;
    fflush(stdout);
    printf("BMP 文件转换完成：%s\n", outBmpPath);
    
    return SYErr_success;
}

#pragma mark -- RGB24 转 BMP
SYKIT_API int SYRgbToBmp::SY_Rgb24ToBmp(unsigned char *inRgb, unsigned int width, unsigned int height, const char *outBmpPath) const
{
    if (NULL == inRgb || NULL == outBmpPath || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    FILE* fp_bmp = fopen(outBmpPath, "wb+");    // 打开 BMP 文件
    if(NULL == fp_bmp)
    {
        printf("Can not open output BMP file.\n");
        return SYErr_failure;
    }
    unsigned char bfType[2] = {'B', 'M'};  // BMP 文件类型标志
    unsigned int typeLen    = sizeof(bfType);
    unsigned int headerSize = typeLen + m_bmpHeaderLen + m_bmpFileInfoLen; // 整个文件头部长度
    unsigned int datalen    = width * height * 3;  // RGB24 数据长度
    
    SYBitMapHeader   m_BMPHeader   = {0};     // BMP 头结构
    SYBitMapFileInfo m_BMPFileInfo = {0};     // BMP 文件信息
    
    m_BMPHeader.bfSize        = datalen + headerSize;
    m_BMPHeader.bfbfOffBits   = headerSize;
    m_BMPFileInfo.biSize      = m_bmpFileInfoLen;
    m_BMPFileInfo.biWidth     = width;
    m_BMPFileInfo.biHeight    = -height; // BMP 存储像素数据与y轴方向相反（即，位图是底朝上的）。
    m_BMPFileInfo.biPlanes    = 1;
    m_BMPFileInfo.biBitcount  = 24;
    m_BMPFileInfo.biSizeImage = datalen;
    
    fwrite(bfType, 1, typeLen, fp_bmp); // 先写入 BMP 文件类型标志：'BM'
    fwrite(&m_BMPHeader, 1, m_bmpHeaderLen, fp_bmp);  // 写入 BMP 头
    fwrite(&m_BMPFileInfo, 1, m_bmpFileInfoLen, fp_bmp);  // 写入 BMP 文件信息
    
    unsigned char r, g, b;      // 当前像素点 R、G、B 值
    unsigned int curPos = 0;    // 当前像素点 RGB 内存位置
    for(int row = 0; row < height; row++)   // 遍历所有行
    {
        for(int col = 0; col < width; col++)    // 遍历所有列
        {
            curPos = (row * width + col) * 3;
            
            // 提取每个像素点 RGB 值
            switch (m_eType)
            {
                case SYEndian_little: // 小端
                {
                    r = inRgb[curPos];
                    g = inRgb[curPos + 1];
                    b = inRgb[curPos + 2];
                }
                    break;
                    
                case SYEndian_big:    // 大端
                {
                    b = inRgb[curPos];
                    g = inRgb[curPos + 1];
                    r = inRgb[curPos + 2];
                }
                    break;
                    
                default:
                    break;
            }
            // BMP 数据存储形式： B1|G1|R1,B2|G2|R2
            fwrite(&b, 1, 1, fp_bmp);   // 写入 B 数据
            fwrite(&g, 1, 1, fp_bmp);   // 写入 G 数据
            fwrite(&r, 1, 1, fp_bmp);   // 写入 R 数据
        }
    }
    fclose(fp_bmp);
    fp_bmp = NULL;
    fflush(stdout);
    printf("BMP 文件转换完成：%s\n", outBmpPath);
    
    return SYErr_success;
}


#pragma mark - Private
#pragma mark -- 检查大小端
SYEndianType SYRgbToBmp::checkEndian()
{
    union {
        char c[4];
        unsigned int num;
    } endianUnion = {'l', '?', '?', 'b'};
    
    if ('l' == (char)endianUnion.num)   // 取首字节判断
    {
        return SYEndian_little;
    }
    else // 'b' == (char)endianUnion.num
    {
        return SYEndian_big;
    }
}
