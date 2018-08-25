//
//  SYYuvToRgb.cpp
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/7/15.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "SYYuvToRgb.h"

#pragma mark - Public
#pragma mark -- 构造函数
SYYuvToRgb::SYYuvToRgb()
{
    m_mType = SYMatrix_normal;
    m_cType = SYConvert_normal;
    m_eType = checkEndian();
    
    initYuvTable();
}

#pragma mark -- 析构函数
SYYuvToRgb::~SYYuvToRgb()
{
    
}

#pragma mark -- 设置转方法阵类型
SYKIT_API void SYYuvToRgb::SY_SetConvertType(SYConvertType cType)
{
    m_cType = cType;
}

#pragma mark -- 设置转换矩阵类型
SYKIT_API void SYYuvToRgb::SY_SetMatrixType(SYMatrixType mType)
{
    m_mType = mType;
}

#pragma mark - Out RGB565
#pragma mark -- I420 转 RGB565
SYKIT_API int SYYuvToRgb::SY_I420ToRgb565(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outRgb) const
{
    if (NULL == inYuv || NULL == outRgb || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uData, *vData;   // 分量 Y、U、V 数据
    int y, u, v;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    unsigned char colorLow;     // 转换后 rgb565 颜色值-低字节
    unsigned char colorHigh;    // 转换后 rgb565 颜色值-高字节
    yData = inYuv;              // 获取 Y 数据地址
    uData = yData + len;        // 获取 U 数据地址
    vData = uData + (len>>2);   // 获取 V 数据地址
    
    int idx = 0;    // 转换后 rgb565 内存地址偏移量
    // 遍历所有像素点
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            /*
             提取每个像素点 YUV 值
             由于 U(Cb)、V(Cr) 取值范围是 [﹣128, 127]；
             而在存储时，为了方便存储，跟 Y 数据一样，统一用一个(无符号)字节表示，即取值范围是 [0, 255];
             所以在读取时，需要 将 U(Cb)、V(Cr)的值减去 128，使其变为 ：[﹣128, 127]。
             */
            
            // 由于查表法在表格创建时，已经减去了 128，所以在此处不用再减
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
            
            yuv2rgb(y, u, v, &r, &g, &b);   // 计算对应 R、G、B 值
            
            // 重置
            colorLow  = 0;
            colorHigh = 0;
            
            // 填充高字节：R(5bit) + G(3bit)
            colorHigh = r & 0xF8;
            colorHigh |= (g & 0xE0)>>5;
            
            // 填充低字节：G(3bit) + B(5bit)
            colorLow = (g & 0x1C)<<3;
            colorLow |= (b & 0xF8)>>3;
            
            switch (m_eType)
            {
                case SYEndian_little: // 小端
                {
                    outRgb[idx++] = colorLow;
                    outRgb[idx++] = colorHigh;
                }
                    break;
                    
                case SYEndian_big:    // 大端
                {
                    outRgb[idx++] = colorHigh;
                    outRgb[idx++] = colorLow;
                }
                    break;
                    
                default:
                    break;
            }
        }
    }
    return SYErr_success;
}

#pragma mark -- NV12 转 RGB565
SYKIT_API int SYYuvToRgb::SY_Nv12ToRgb565(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outRgb) const
{
    if (inYuv == NULL || outRgb == NULL || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uvData;
    int y, u, v;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    unsigned char colorLow;     // 转换后 rgb565 颜色值-低字节
    unsigned char colorHigh;    // 转换后 rgb565 颜色值-高字节
    yData  = inYuv;              // 获取 Y 数据地址
    uvData = yData + len;        // 获取 UV 数据地址
    
    int idx  = 0;   // 转换后 rgb24 数据内存地址偏移量
    int uPos = 0;   // U 数据位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            /*
             提取每个像素点 YUV 值
             由于 U(Cb)、V(Cr) 取值范围是 [﹣128, 127]；
             而在存储时，为了方便存储，跟 Y 数据一样，统一用一个(无符号)字节表示，即取值范围是 [0, 255];
             所以在读取时，需要 将 U(Cb)、V(Cr)的值减去 128，使其变为 ：[﹣128, 127]。
             */
            
            uPos = (row>>1) * width + (col - col%2);
            
            // 由于查表法在表格创建时，已经减去了 128，所以在此处不用再减
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
            
            // 重置
            colorLow  = 0;
            colorHigh = 0;
            
            // 填充高字节：R(5bit) + G(3bit)
            colorHigh = r & 0xF8;
            colorHigh |= (g & 0xE0)>>5;
            
            // 填充低字节：G(3bit) + B(5bit)
            colorLow = (g & 0x1C)<<3;
            colorLow |= (b & 0xF8)>>3;
            
            switch (m_eType)
            {
                case SYEndian_little: // 小端
                {
                    outRgb[idx++] = colorLow;
                    outRgb[idx++] = colorHigh;
                }
                    break;
                    
                case SYEndian_big:    // 大端
                {
                    outRgb[idx++] = colorHigh;
                    outRgb[idx++] = colorLow;
                }
                    break;
                    
                default:
                    break;
            }
        }
    }
    return SYErr_success;
}

#pragma mark -- NV21 转 RGB565
SYKIT_API int SYYuvToRgb::SY_Nv21ToRgb565(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outRgb) const
{
    if (inYuv == NULL || outRgb == NULL || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *vuData;
    int y, u, v;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    unsigned char colorLow;     // 转换后 rgb565 颜色值-低字节
    unsigned char colorHigh;    // 转换后 rgb565 颜色值-高字节
    yData  = inYuv;              // 获取 Y 数据地址
    vuData = yData + len;        // 获取 VU 数据地址
    
    int idx  = 0;   // 转换后 rgb24 数据内存地址偏移量
    int vPos = 0;   // V 数据位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            /*
             提取每个像素点 YUV 值
             由于 U(Cb)、V(Cr) 取值范围是 [﹣128, 127]；
             而在存储时，为了方便存储，跟 Y 数据一样，统一用一个(无符号)字节表示，即取值范围是 [0, 255];
             所以在读取时，需要 将 U(Cb)、V(Cr)的值减去 128，使其变为 ：[﹣128, 127]。
             */
            
            vPos = (row>>1) * width + (col - col%2);
            
            // 由于查表法在表格创建时，已经减去了 128，所以在此处不用再减
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
            
            // 重置
            colorLow  = 0;
            colorHigh = 0;
            
            // 填充高字节：R(5bit) + G(3bit)
            colorHigh = r & 0xF8;
            colorHigh |= (g & 0xE0)>>5;
            
            // 填充低字节：G(3bit) + B(5bit)
            colorLow = (g & 0x1C)<<3;
            colorLow |= (b & 0xF8)>>3;
            
            switch (m_eType)
            {
                case SYEndian_little: // 小端
                {
                    outRgb[idx++] = colorLow;
                    outRgb[idx++] = colorHigh;
                }
                    break;
                    
                case SYEndian_big:    // 大端
                {
                    outRgb[idx++] = colorHigh;
                    outRgb[idx++] = colorLow;
                }
                    break;
                    
                default:
                    break;
            }
        }
    }
    return SYErr_success;
}

#pragma mark - Out RGB24
#pragma mark -- I420 转 RGB24
SYKIT_API int SYYuvToRgb::SY_I420ToRgb24(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outRgb) const
{
    if (inYuv == NULL || outRgb == NULL || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uData, *vData;
    int y, u, v;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    yData = inYuv;              // 获取 Y 数据地址
    uData = yData + len;        // 获取 U 数据地址
    vData = uData + (len>>2);   // 获取 V 数据地址
    
    int idx = 0;    // 转换后 rgb24 数据内存地址偏移量
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            /*
             提取每个像素点 YUV 值
             由于 U(Cb)、V(Cr) 取值范围是 [﹣128, 127]；
             而在存储时，为了方便存储，跟 Y 数据一样，统一用一个(无符号)字节表示，即取值范围是 [0, 255];
             所以在读取时，需要 将 U(Cb)、V(Cr)的值减去 128，使其变为 ：[﹣128, 127]。
             */
            
            // 由于查表法在表格创建时，已经减去了 128，所以在此处不用再减
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
            
            yuv2rgb(y, u, v, &r, &g, &b);
            
            switch (m_eType)
            {
                case SYEndian_little: // 小端
                {
                    outRgb[idx++] = (char)r;
                    outRgb[idx++] = (char)g;
                    outRgb[idx++] = (char)b;
                }
                    break;
                    
                case SYEndian_big:    // 大端
                {
                    outRgb[idx++] = (char)b;
                    outRgb[idx++] = (char)g;
                    outRgb[idx++] = (char)r;
                }
                    break;
                    
                default:
                    break;
            }
        }
    }
    return SYErr_success;
}

#pragma mark -- NV12 转 RGB24
SYKIT_API int SYYuvToRgb::SY_Nv12ToRgb24(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outRgb) const
{
    if (inYuv == NULL || outRgb == NULL || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uvData;
    int y, u, v;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    yData  = inYuv;              // 获取 Y 数据地址
    uvData = yData + len;        // 获取 UV 数据地址
    
    int idx  = 0;   // 转换后 rgb24 数据内存地址偏移量
    int uPos = 0;   // U 数据位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            /*
             提取每个像素点 YUV 值
             由于 U(Cb)、V(Cr) 取值范围是 [﹣128, 127]；
             而在存储时，为了方便存储，跟 Y 数据一样，统一用一个(无符号)字节表示，即取值范围是 [0, 255];
             所以在读取时，需要 将 U(Cb)、V(Cr)的值减去 128，使其变为 ：[﹣128, 127]。
             */
            
            uPos = (row>>1) * width + (col - col%2);
            
            // 由于查表法在表格创建时，已经减去了 128，所以在此处不用再减
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
            
            switch (m_eType)
            {
                case SYEndian_little: // 小端
                {
                    outRgb[idx++] = (char)r;
                    outRgb[idx++] = (char)g;
                    outRgb[idx++] = (char)b;                    
                }
                    break;
                    
                case SYEndian_big:    // 大端
                {
                    outRgb[idx++] = (char)b;
                    outRgb[idx++] = (char)g;
                    outRgb[idx++] = (char)r;
                }
                    break;
                    
                default:
                    break;
            }
        }
    }
    return SYErr_success;
}

#pragma mark -- NV21 转 RGB24
SYKIT_API int SYYuvToRgb::SY_Nv21ToRgb24(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outRgb) const
{
    if (inYuv == NULL || outRgb == NULL || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *vuData;
    int y, u, v;      // 当前像素点 Y、U、V 值
    int r, g, b;      // 当前像素点 R、G、B 值
    yData  = inYuv;              // 获取 Y 数据地址
    vuData = yData + len;        // 获取 VU 数据地址
    
    int idx  = 0;   // 转换后 rgb24 数据内存地址偏移量
    int vPos = 0;   // V 数据位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            /*
             提取每个像素点 YUV 值
             由于 U(Cb)、V(Cr) 取值范围是 [﹣128, 127]；
             而在存储时，为了方便存储，跟 Y 数据一样，统一用一个(无符号)字节表示，即取值范围是 [0, 255];
             所以在读取时，需要 将 U(Cb)、V(Cr)的值减去 128，使其变为 ：[﹣128, 127]。
             */
            
            vPos = (row>>1) * width + (col - col%2);
            
            // 由于查表法在表格创建时，已经减去了 128，所以在此处不用再减
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
            
            switch (m_eType)
            {
                case SYEndian_little: // 小端
                {
                    outRgb[idx++] = (char)r;
                    outRgb[idx++] = (char)g;
                    outRgb[idx++] = (char)b;
                }
                    break;
                    
                case SYEndian_big:    // 大端
                {
                    outRgb[idx++] = (char)b;
                    outRgb[idx++] = (char)g;
                    outRgb[idx++] = (char)r;
                }
                    break;
                    
                default:
                    break;
            }
        }
    }
    return SYErr_success;
}

#pragma mark - Private
#pragma mark -- 检查大小端
SYEndianType SYYuvToRgb::checkEndian()
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
void SYYuvToRgb::initYuvTable()
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
void SYYuvToRgb::yuv2rgb(int y, int u, int v, int* r, int* g, int* b) const
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
