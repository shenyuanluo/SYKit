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
    m_mType = SYMatrix_normal;
    m_cType = SYConvert_normal;
    m_eType = checkEndian();
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


#pragma mark -- I420 转 Bmp
SYKIT_API int SYYuvToBmp::SY_I420ToBmp(unsigned char* inYuv, unsigned int width, unsigned int height, const char* outBmpPath) const
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
    
    // YUV 数据处理
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uData, *vData;
    int y, u, v;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    yData = inYuv;              // 获取 Y 数据地址
    uData = yData + len;        // 获取 U 数据地址
    vData = uData + (len>>2);   // 获取 V 数据地址
    
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            // 提取每个像素点 YUV 值（由于查表法在表格创建时，已经减去了 128，所以在此处不用再减）
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
            
            yuv2rgb(y, u, v, &r, &g, &b);   // 转换成 RGB 值
            
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

#pragma mark -- NV12 转 BMP
SYKIT_API int SYYuvToBmp::SY_Nv12ToBmp(unsigned char* inYuv, unsigned int width, unsigned int height, const char* outBmpPath) const
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
    
    // YUV 数据处理
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uvData;
    int y, u, v;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    yData  = inYuv;              // 获取 Y 数据地址
    uvData = yData + len;        // 获取 UV 数据地址
    
    int uPos = 0;   // U 数据位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            uPos = (row>>1) * width + (col - col%2);
            
            // 提取每个像素点 YUV 值（由于查表法在表格创建时，已经减去了 128，所以在此处不用再减）
            if (SYConvert_table == m_cType)
            {
                y = (int)yData[row * width + col];
                u = (int)uvData[uPos];
                v = (int)uvData[uPos + 1];
            }
            else
            {
                y = (int)yData[row * width + col];
                u = (int)(uvData[uPos] - 128);
                v = (int)(uvData[uPos + 1] - 128);
            }
            
            yuv2rgb(y, u, v, &r, &g, &b);
            
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

#pragma mark -- NV21 转 RGB24
SYKIT_API int SYYuvToBmp::SY_Nv21ToBmp(unsigned char* inYuv, unsigned int width, unsigned int height, const char* outBmpPath) const
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
    
    // YUV 数据处理
    const long len = width * height;    // 像素点数
    unsigned char *yData, *vuData;
    int y, u, v;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    yData  = inYuv;              // 获取 Y 数据地址
    vuData = yData + len;        // 获取 VU 数据地址
    
    int vPos = 0;   // V 数据位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            vPos = (row>>1) * width + (col - col%2);
            
            // 提取每个像素点 YUV 值（由于查表法在表格创建时，已经减去了 128，所以在此处不用再减）
            if (SYConvert_table == m_cType)
            {
                y = (int)yData[row * width + col];
                v = (int)vuData[vPos];
                u = (int)vuData[vPos + 1];
            }
            else
            {
                y = (int)yData[row * width + col];
                v = (int)(vuData[vPos] - 128);
                u = (int)(vuData[vPos + 1] - 128);
            }
            
            yuv2rgb(y, u, v, &r, &g, &b);
            
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
