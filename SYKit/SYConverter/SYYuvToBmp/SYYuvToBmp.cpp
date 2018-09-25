//
//  SYYuvToBmp.cpp
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/7/20.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "SYYuvToBmp.h"


#pragma mark - Public
#pragma mark -- 构造函数
SYYuvToBmp::SYYuvToBmp()
{
    m_mType   = SYMatrix_normal;
    m_cType   = SYConvert_normal;
    m_eType   = checkEndian();
    m_yuvType = SYYuv_i420;
    m_bmpHeaderLen   = sizeof(SYBitMapHeader);
    m_bmpFileInfoLen = sizeof(SYBitMapFileInfo);
    
    initYuvTable();
}

#pragma mark -- 析构函数
SYYuvToBmp::~SYYuvToBmp()
{
    
}


#pragma mark -- 设置转方法阵类型
SYKIT_API void SYYuvToBmp::SY_SetConvertType(SYConvertType cType)
{
    m_cType = cType;
}

#pragma mark -- 设置转换矩阵类型
SYKIT_API void SYYuvToBmp::SY_SetMatrixType(SYMatrixType mType)
{
    m_mType = mType;
}

#pragma mark -- 设置 YUV 数据格式类型
SYKIT_API void SYYuvToBmp::SY_SetYuvType(SYYuvType yuvType)
{
    m_yuvType = yuvType;
}

#pragma mark -- YUV 转 BMP
SYKIT_API int SYYuvToBmp::SY_YuvToBmp(unsigned char* inYuv, unsigned int width, unsigned int height, const char* outBmpPath) const
{
    if (inYuv == NULL || outBmpPath == NULL || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    FILE* fp_bmp = fopen(outBmpPath, "wb+");    // 打开 BMP 文件
    if(NULL == fp_bmp)
    {
        printf("Can not open output BMP file.\n");
        return SYErr_failure;
    }
    // BMP 头处理
    unsigned char bfType[2] = {'B', 'M'};  // BMP 文件类型标志
    unsigned int typeLen    = sizeof(bfType);
    unsigned int headerSize = typeLen + m_bmpHeaderLen + m_bmpFileInfoLen; // 整个 bmp 文件头部长度
    
    SYBitMapHeader   m_BMPHeader   = {0};     // BMP 头结构
    SYBitMapFileInfo m_BMPFileInfo = {0};     // BMP 文件信息
    
    m_BMPHeader.bfOffBits     = headerSize;
    m_BMPFileInfo.biSize      = m_bmpFileInfoLen;
    m_BMPFileInfo.biWidth     = width;
    m_BMPFileInfo.biHeight    = height;
    m_BMPFileInfo.biPlanes    = 1;
    m_BMPFileInfo.biBitCount  = 24;
    int rawBytePerRow         = (width * m_BMPFileInfo.biBitCount + 7)>>3; // 每行字节数（裸数据）
    int padBytePerRow         = (4 - rawBytePerRow) & 3;   // 每行需填充的字节数（确保每行字节数为 4 的倍数）
    m_BMPFileInfo.biSizeImage = height * (rawBytePerRow + padBytePerRow);
    m_BMPHeader.bfSize        = m_BMPFileInfo.biSizeImage + m_BMPHeader.bfOffBits;
    
    fwrite(bfType, 1, typeLen, fp_bmp); // 先写入 BMP 文件类型标志：'BM'
    fwrite(&m_BMPHeader, 1, m_bmpHeaderLen, fp_bmp);  // 写入 BMP 头
    fwrite(&m_BMPFileInfo, 1, m_bmpFileInfoLen, fp_bmp);  // 写入 BMP 文件信息
    
    // YUV 数据处理
    const long len = width * height;    // 像素点数
    unsigned char* yData = inYuv;  // Y 数据地址
    unsigned char* uData = NULL;   // U 数据地址
    unsigned char* vData = NULL;   // V 数据地址
    int y = 0, u = 0, v = 0;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    switch (m_yuvType)
    {
        case SYYuv_i420:
        {
            uData = yData + len;
            vData = uData + (len>>2);
        }
            break;
            
        case SYYuv_nv12:
        {
            uData = yData + len;
        }
            break;
            
        case SYYuv_nv21:
        {
            vData = yData + len;
        }
            break;
            
        default:
            break;
    }
    // 填充空白数据
    unsigned char *paddingData = (unsigned char*)malloc(padBytePerRow);
    memset(paddingData, 0, padBytePerRow);
    
    int uPos = 0;   // U 数据位置
    int vPos = 0;   // V 数据位置
    
    // BMP 存储像素数据与y轴方向相反（即，位图是底朝上的）
    for (int row = height - 1; row >= 0; row--)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            uPos = (row>>1) * width + (col - col%2);    // NV12
            vPos = (row>>1) * width + (col - col%2);    // NV21
            
            // 提取每个像素点 YUV 值（由于查表法在表格创建时，已经减去了 128，所以无需再减）
            switch (m_yuvType)
            {
                case SYYuv_i420:
                {
                    if (SYConvert_table == m_cType)
                    {
                        y = (int)yData[row * width + col];
                        u = (int)uData[(row>>1) * (width>>1) + (col>>1)];
                        v = (int)vData[(row>>1) * (width>>1) + (col>>1)];
                    }
                    else
                    {
                        y = (int)yData[row * width + col];
                        u = (int)(uData[(row>>1) * (width>>1) + (col>>1)] - 128);
                        v = (int)(vData[(row>>1) * (width>>1) + (col>>1)] - 128);
                    }
                }
                    break;
                    
                case SYYuv_nv12:
                {
                    if (SYConvert_table == m_cType)
                    {
                        y = (int)yData[row * width + col];
                        u = (int)uData[uPos];
                        v = (int)uData[uPos + 1];
                    }
                    else
                    {
                        y = (int)yData[row * width + col];
                        u = (int)(uData[uPos] - 128);
                        v = (int)(uData[uPos + 1] - 128);
                    }
                }
                    break;
                    
                case SYYuv_nv21:
                {
                    if (SYConvert_table == m_cType)
                    {
                        y = (int)yData[row * width + col];
                        v = (int)vData[vPos];
                        u = (int)vData[vPos + 1];
                    }
                    else
                    {
                        y = (int)yData[row * width + col];
                        v = (int)(vData[vPos] - 128);
                        u = (int)(vData[vPos + 1] - 128);
                    }
                }
                    break;
                    
                default:
                    break;
            }
            
            yuv2rgb(y, u, v, &r, &g, &b);   // 转换成 RGB 值
            
            // 在每行最后（在开头填充，有时会出现 BMP 像素错乱现象，原因未知）填充空白数据
            if (width - 1 == col && 0 < padBytePerRow)
            {
                fwrite(paddingData, 1, padBytePerRow, fp_bmp);
            }
            // BMP 数据存储形式： B1|G1|R1,B2|G2|R2
            fwrite(&b, 1, 1, fp_bmp);   // 写入 B 数据
            fwrite(&g, 1, 1, fp_bmp);   // 写入 G 数据
            fwrite(&r, 1, 1, fp_bmp);   // 写入 R 数据
        }
    }
    free(paddingData);
    paddingData = NULL;
    fclose(fp_bmp);
    fp_bmp = NULL;
    fflush(stdout);
    printf("BMP 文件转换完成：%s\n", outBmpPath);
    return SYErr_success;
}


#pragma mark - Private
#pragma mark -- 检查大小端
SYEndianType SYYuvToBmp::checkEndian()
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

#pragma mark -- 初始化 YUV 颜色表
void SYYuvToBmp::initYuvTable()
{
    int rv = 0;     // 计算 R 值 V 系数
    int gu = 0;     // 计算 G 值 U 系数
    int gv = 0;     // 计算 G 值 V 系数
    int bu = 0;     // 计算 B 值 U 系数
    switch (m_mType)
    {
        case SYMatrix_normal:  // 常规标准
        {
            rv = 351;
            gu = 86;
            gv = 179;
            bu = 444;
        }
            break;
            
        case SYMatrix_bt_601:  // BT.601 标准
        {
            rv = 292;
            gu = 101;
            gv = 149;
            bu = 520;
        }
            break;
            
        case SYMatrix_bt_709:  // BT.709 标准
        {
            rv = 328;
            gu = 55;
            gv = 97;
            bu = 545;
        }
            break;
            
        default:
            break;
    }
    
    for (int i = 0; i < 256; i++)
    {
        // U(Cb)、V(Cr) 记得减去 128
        m_rv[i] = ((i - 128) * rv)>>8;
        m_gu[i] = ((i - 128) * gu)>>8;
        m_gv[i] = ((i - 128) * gv)>>8;
        m_bu[i] = ((i - 128) * bu)>>8;
    }
}

#pragma mark -- 将单个像素 YUV 转成 RGB
void SYYuvToBmp::yuv2rgb(int y, int u, int v, int* r, int* g, int* b) const
{
    switch (m_mType)
    {
        case SYMatrix_normal:
        {
            switch (m_cType)
            {
                case SYConvert_normal: // 常规方法：浮点运算，精度高
                {
                    *r = y                  + (1.370705 * v);
                    *g = y - (0.337633 * u) - (0.698001 * v);
                    *b = y + (1.732446 * u);
                }
                    break;
                    
                case SYConvert_bitMult:  // 通过位移来避免浮点运算，精度低
                {
                    *r = ((256 * y             + (351 * v))>>8);
                    *g = ((256 * y - (86  * u) - (179 * v))>>8);
                    *b = ((256 * y + (444 * u))            >>8);
                }
                    break;
                    
                case SYConvert_bitAdd:   // 通过位移来避免乘法运算，精度低
                {
                    *r = (((y<<8) + (v<<8) + (v<<6) + (v<<4) + (v<<3) + (v<<2) + (v<<1) + v)                   >> 8);
                    *g = (((y<<8) - (u<<6) - (u<<4) - (u<<2) - (u<<1) - (v<<7) - (v<<5) - (v<<4) - (v<<1) - v) >> 8);
                    *b = (((y<<8) + (u<<8) + (u<<7) + (u<<5) + (u<<4) + (u<<3) + (u<<2))                       >> 8);
                }
                    break;
                    
                case SYConvert_table: // 查表法（也是位移计算），精度低
                {
                    *r = y + m_rv[v];
                    *g = y - m_gu[u] - m_gv[v];
                    *b = y + m_bu[u];
                }
                    break;
                    
                default:
                    break;
            }
        }
            break;
            
        case SYMatrix_bt_601:
        {
            switch (m_cType)
            {
                case SYConvert_normal: // 常规方法：浮点运算，精度高
                {
                    *r = (y               + 1.13983 * v);
                    *g = (y - 0.39465 * u - 0.5806  * v);
                    *b = (y + 2.03211 * u);
                }
                    break;
                    
                case SYConvert_bitMult:  // 通过位移来避免浮点运算，精度低
                {
                    *r = ((256 * y           + 292 * v)>>8);
                    *g = ((256 * y - 101 * u - 149 * v)>>8);
                    *b = ((256 * y + 520 * u)          >>8);
                }
                    break;
                    
                case SYConvert_bitAdd:   // 通过位移来避免乘法运算，精度低
                {
                    *r = (((y<<8) + (v<<8) + (v<<5) + (v<<2))                                   >>8);
                    *g = (((y<<8) + (u<<6) + (u<<5) + (u<<2) + u + (v<<7) + (v<<4) + (v<<2) + v)>>8);
                    *b = (((y<<8) + (u<<9) + (u<<3))                                            >>8);
                }
                    break;
                    
                case SYConvert_table: // 查表法（也是位移计算），精度低
                {
                    *r = y + m_rv[v];
                    *g = y - m_gu[u] - m_gv[v];
                    *b = y + m_bu[u];
                }
                    break;
                    
                default:
                    break;
            }
        }
            break;
            
        case SYMatrix_bt_709:
        {
            switch (m_cType)
            {
                case SYConvert_normal: // 常规方法：浮点运算，精度高
                {
                    *r = (y               + 1.28033 * v);
                    *g = (y - 0.21482 * u - 0.38059 * v);
                    *b = (y + 2.12798 * u);
                }
                    break;
                    
                case SYConvert_bitMult:  // 通过位移来避免浮点运算，精度低
                {
                    *r = ((256 * y           + 328 * v)>>8);
                    *g = ((256 * y - 55  * u - 97  * v)>>8);
                    *b = ((256 * y + 545 * u)          >>8);
                }
                    break;
                    
                case SYConvert_bitAdd:   // 通过位移来避免乘法运算，精度低
                {
                    *r = (((y<<8) + (v<<8) + (v<<6) + (v<<3))                                   >>8);
                    *g = (((y<<8) + (u<<5) + (u<<4) + (u<<2) + (u<<1) + u + (v<<6) + (v<<5) + v)>>8);
                    *b = (((y<<8) + (u<<9) + (u<<5) +u)                                         >>8);
                }
                    break;
                    
                case SYConvert_table: // 查表法（也是位移计算），精度低
                {
                    *r = y + m_rv[v];
                    *g = y - m_gu[u] - m_gv[v];
                    *b = y + m_bu[u];
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
    *r = CLAMP(*r, 0, 255);
    *g = CLAMP(*g, 0, 255);
    *b = CLAMP(*b, 0, 255);
}
