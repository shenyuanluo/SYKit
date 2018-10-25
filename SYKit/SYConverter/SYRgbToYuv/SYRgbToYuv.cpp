//
//  SYRgbToYuv.cpp
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/7/15.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "SYRgbToYuv.h"
#pragma mark - Public
#pragma mark -- 构造函数
SYRgbToYuv::SYRgbToYuv()
{
    m_mType = SYMatrix_normal;
    m_cType = SYConvert_normal;
    m_eType = checkEndian();
    
    initRgbTable();
}

#pragma mark -- 析构函数
SYRgbToYuv::~SYRgbToYuv()
{
    
}

#pragma mark -- 设置转方法阵类型
SYKIT_API void SYRgbToYuv::SY_SetConvertType(SYConvertType cType)
{
    m_cType = cType;
}

#pragma mark -- 设置转换矩阵类型
SYKIT_API void SYRgbToYuv::SY_SetMatrixType(SYMatrixType mType)
{
    m_mType = mType;
}

#pragma mark - RGB565
#pragma mark -- RGB565 转 I420
SYKIT_API int SYRgbToYuv::SY_Rgb565ToI420(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inRgb || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uData, *vData;
    unsigned char colorLow;     // 当前像素 rgb565 颜色值-低字节
    unsigned char colorHigh;    // 当前像素 rgb565 颜色值-高字节
    
    int y, u, v;    // 当前像素点 Y、U、V 值
    int r, g, b;    // 当前像素点 R、G、B 值
    yData = outYuv;             // 设置 Y 数据地址
    uData = yData + len;        // 设置 U 数据地址
    vData = uData + (len>>2);   // 设置 V 数据地址
    
    int yIdx = 0;   // Y 数据下标
    int uIdx = 0;   // U 数据下标
    int vIdx = 0;   // V 数据下标
    int isUVRow;    // 是否写入 U、V 数据（行）
    int isUVCol;    // 是否写入 U、V 数据（列）
    int curPos = 0; // 当前像素点 RGB 内存位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        isUVRow = 0;
        if(0 == (row&1)) isUVRow = 1;
        
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            isUVCol = 0;
            if(0 == (col&1)) isUVCol = 1;
            
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
            
            rgb2yuv(r, g, b, &y, &u, &v);
            
            yData[yIdx++] = y;  // 写入 Y 数据
            if (1 == isUVRow && 1 == isUVCol)
            {
                uData[uIdx++] = u;  // 写入 U 数据
                vData[vIdx++] = v;  // 写入 V 数据
            }
        }
    }
    return SYErr_success;
}

#pragma mark -- RGB565 转 NV12
SYKIT_API int SYRgbToYuv::SY_Rgb565ToNv12(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inRgb || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uvData;
    unsigned char colorLow;     // 当前像素 rgb565 颜色值-低字节
    unsigned char colorHigh;    // 当前像素 rgb565 颜色值-高字节
    
    int y, u, v;    // 当前像素点 Y、U、V 值
    int r, g, b;    // 当前像素点 R、G、B 值
    yData  = outYuv;        // 设置 Y 数据地址
    uvData = yData + len;   // 设置 UV 数据地址
    
    int yIdx  = 0;  // Y 数据下标
    int uvIdx = 0;  // UV 数据下标
    int isUVRow;    // 是否写入 UV 数据（行）
    int isUVCol;    // 是否写入 UV 数据（列）
    int curPos = 0; // 当前像素点 RGB 内存位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        isUVRow = 0;
        if(0 == (row&1)) isUVRow = 1;
        
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            isUVCol = 0;
            if(0 == (col&1)) isUVCol = 1;
            
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
            
            rgb2yuv(r, g, b, &y, &u, &v);
            
            yData[yIdx++] = y;  // 写入 Y 数据
            if (1 == isUVRow && 1 == isUVCol)
            {
                // 先写 U，再写 V
                uvData[uvIdx++] = u;
                uvData[uvIdx++] = v;
            }
        }
    }
    return SYErr_success;
}

#pragma mark -- RGB565 转 NV21
SYKIT_API int SYRgbToYuv::SY_Rgb565ToNv21(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inRgb || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *vuData;
    unsigned char colorLow;     // 当前像素 rgb565 颜色值-低字节
    unsigned char colorHigh;    // 当前像素 rgb565 颜色值-高字节
    
    int y, u, v;    // 当前像素点 Y、U、V 值
    int r, g, b;    // 当前像素点 R、G、B 值
    yData  = outYuv;        // 设置 Y 数据地址
    vuData = yData + len;   // 设置 VU 数据地址
    
    int yIdx  = 0;  // Y 数据下标
    int vuIdx = 0;  // VU 数据下标
    int isVURow;    // 是否写入 VU 数据（行）
    int isVUCol;    // 是否写入 VU 数据（列）
    int curPos = 0; // 当前像素点 RGB 内存位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        isVURow = 0;
        if(0 == (row&1)) isVURow = 1;
        
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            isVUCol = 0;
            if(0 == (col&1)) isVUCol = 1;
            
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
            
            rgb2yuv(r, g, b, &y, &u, &v);
            
            yData[yIdx++] = y;  // 写入 Y 数据
            if (1 == isVURow && 1 == isVUCol)
            {
                // 先写 V，再写 U
                vuData[vuIdx++] = v;
                vuData[vuIdx++] = u;
            }
        }
    }
    return SYErr_success;
}

#pragma mark - RGB24
#pragma mark -- RGB24 转 I420
SYKIT_API int SYRgbToYuv::SY_Rgb24ToI420(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inRgb || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uData, *vData;
    int y, u, v;    // 当前像素点 Y、U、V 值
    int r, g, b;    // 当前像素点 R、G、B 值
    yData = outYuv;             // 设置 Y 数据地址
    uData = yData + len;        // 设置 U 数据地址
    vData = uData + (len>>2);   // 设置 V 数据地址
    
    int yIdx = 0;   // Y 数据下标
    int uIdx = 0;   // U 数据下标
    int vIdx = 0;   // V 数据下标
    int isUVRow;    // 是否写入 U、V 数据（行）
    int isUVCol;    // 是否写入 U、V 数据（列）
    int curPos = 0; // 当前像素点 RGB 内存位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        isUVRow = 0;
        if(0 == (row&1)) isUVRow = 1;
        
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            isUVCol = 0;
            if(0 == (col&1)) isUVCol = 1;
            
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
            rgb2yuv(r, g, b, &y, &u, &v);

            yData[yIdx++] = y;  // 写入 Y 数据
            if (1 == isUVRow && 1 == isUVCol)
            {
                uData[uIdx++] = u;  // 写入 U 数据
                vData[vIdx++] = v;  // 写入 V 数据
            }
        }
    }
    return SYErr_success;
}

#pragma mark -- RGB24 转 NV12
SYKIT_API int SYRgbToYuv::SY_Rgb24ToNv12(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inRgb || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *uvData;
    int y, u, v;    // 当前像素点 Y、U、V 值
    int r, g, b;    // 当前像素点 R、G、B 值
    yData = outYuv;             // 设置 Y 数据地址
    uvData = yData + len;       // 设置 UV 数据地址
    
    int yIdx  = 0;  // Y 数据下标
    int uvIdx = 0;  // UV 数据下标
    int isUVRow;    // 是否写入 UV 数据（行）
    int isUVCol;    // 是否写入 UV 数据（列）
    int curPos = 0; // 当前像素点 RGB 内存位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        isUVRow = 0;
        if(0 == (row&1)) isUVRow = 1;
        
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            isUVCol = 0;
            if(0 == (col&1)) isUVCol = 1;
            
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
            rgb2yuv(r, g, b, &y, &u, &v);
            
            yData[yIdx++] = y;  // 写入 Y 数据
            if (1 == isUVRow && 1 == isUVCol)
            {
                // 先写 U，再写 V
                uvData[uvIdx++] = u;
                uvData[uvIdx++] = v;
            }
        }
    }
    return SYErr_success;
}

#pragma mark -- RGB24 转 NV21
SYKIT_API int SYRgbToYuv::SY_Rgb24ToNv21(unsigned char* inRgb, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inRgb || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len = width * height;    // 像素点数
    unsigned char *yData, *vuData;
    int y, u, v;    // 当前像素点 Y、U、V 值
    int r, g, b;    // 当前像素点 R、G、B 值
    yData = outYuv;             // 设置 Y 数据地址
    vuData = yData + len;       // 设置 VU 数据地址
    
    int yIdx  = 0;  // Y 数据下标
    int vuIdx = 0;  // UV 数据下标
    int isVURow;    // 是否写入 VU 数据（行）
    int isVUCol;    // 是否写入 VU 数据（列）
    int curPos = 0; // 当前像素点 RGB 内存位置
    for (int row = 0; row < height; row++)  // 遍历所有行
    {
        isVURow = 0;
        if(0 == (row&1)) isVURow = 1;
        
        for (int col = 0; col < width; col++)   // 遍历所有列
        {
            isVUCol = 0;
            if(0 == (col&1)) isVUCol = 1;
            
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
            rgb2yuv(r, g, b, &y, &u, &v);
            
            yData[yIdx++] = y;  // 写入 Y 数据
            if (1 == isVURow && 1 == isVUCol)
            {
                // 先写 V，再写 U
                vuData[vuIdx++] = v;
                vuData[vuIdx++] = u;
            }
        }
    }
    return SYErr_success;
}


#pragma mark - Private
#pragma mark -- 检查大小端
SYEndianType SYRgbToYuv::checkEndian() const
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

#pragma mark -- 初始化 RGB 颜色表
void SYRgbToYuv::initRgbTable()
{
    int yr = 0;     // 计算 Y 值 R 系数
    int yg = 0;     // 计算 Y 值 G 系数
    int yb = 0;     // 计算 Y 值 B 系数
    int ur = 0;     // 计算 U 值 R 系数
    int ug = 0;     // 计算 U 值 G 系数
    int ub = 0;     // 计算 U 值 B 系数
    int vr = 0;     // 计算 V 值 R 系数
    int vg = 0;     // 计算 V 值 G 系数
    int vb = 0;     // 计算 V 值 B 系数
    switch (m_mType)
    {
        case SYMatrix_normal:  // 常规标准
        {
            yr = 76;
            yg = 150;
            yb = 29;
            ur = 44;
            ug = 87;
            ub = 131;
            vr = 131;
            vg = 110;
            vb = 21;
        }
            break;
            
        case SYMatrix_bt_601:  // BT.601 标准
        {
            yr = 77;
            yg = 150;
            yb = 29;
            ur = 38;
            ug = 74;
            ub = 112;
            vr = 157;
            vg = 132;
            vb = 26;
        }
            break;
            
        case SYMatrix_bt_709:  // BT.709 标准
        {
            yr = 54;
            yg = 183;
            yb = 18;
            ur = 26;
            ug = 86;
            ub = 112;
            vr = 157;
            vg = 143;
            vb = 14;
        }
            break;
            
        default:
            break;
    }
    
    for (int i = 0; i < 256; i++)
    {
        m_yr[i] = (i * yr)>>8;
        m_yg[i] = (i * yg)>>8;
        m_yb[i] = (i * yb)>>8;
        m_ur[i] = (i * ur)>>8;
        m_ug[i] = (i * ug)>>8;
        m_ub[i] = (i * ub)>>8;
        m_vr[i] = (i * vr)>>8;
        m_vg[i] = (i * vg)>>8;
        m_vb[i] = (i * vb)>>8;
    }
}

#pragma mark -- 将单个像素 RGB 转成 YUV
void SYRgbToYuv::rgb2yuv(int r, int g, int b, int* y, int* u, int* v) const
{
    switch (m_mType)
    {
        case SYMatrix_normal:
        {
            switch (m_cType)
            {
                case SYConvert_normal: // 常规方法：浮点运算，精度高
                {
                    *y =  0.29882  * r + 0.58681  * g + 0.114363 * b;
                    *u = -0.172485 * r - 0.338718 * g + 0.511207 * b;
                    *v =  0.51155  * r - 0.42811  * g - 0.08343  * b;
                }
                    break;
                    
                case SYConvert_bitMult:  // 通过位移来避免浮点运算，精度低
                {
                    *y = ( 76  * r + 150 * g + 29  * b)>>8;
                    *u = (-44  * r - 87  * g + 131 * b)>>8;
                    *v = ( 131 * r - 110 * g - 21  * b)>>8;
                }
                    break;
                    
                case SYConvert_bitAdd:   // 通过位移来避免乘法运算，精度低
                {
                    *y = ( (r<<6) + (r<<3) + (r<<2) + (g<<7) + (g<<4) + (g<<2) + (g<<1) + (b<<4) + (b<<3) + (b<<2) + b)>>8;
                    *u = (-(r<<5) - (r<<3) - (r<<2) - (g<<6) - (g<<4) - (g<<2) - (g<<1) - g + (b<<7) + (b<<1) + b)>>8;
                    *v = ((r<<7) + (r<<1) + r - (g<<6) - (g<<5) - (g<<3) - (g<<2) - (g<<1) - (b<<4) - (b<<2) - b)>>8;
                }
                    break;
                    
                case SYConvert_table: // 查表法（也是位移计算），精度低
                {
                    *y =  m_yr[r] + m_yg[g] + m_yb[b];
                    *u = -m_ur[r] - m_ug[g] + m_ub[b];
                    *v =  m_vr[r] - m_vg[g] - m_vb[b];
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
                    *y =  0.299   * r + 0.587   * g + 0.114   * b;
                    *u = -0.14713 * r - 0.28886 * g + 0.436   * b;
                    *v =  0.615   * r - 0.51499 * g - 0.10001 * b;
                }
                    break;
                    
                case SYConvert_bitMult:  // 通过位移来避免浮点运算，精度低
                {
                    *y = ( 77  * r + 150 * g + 29  * b)>>8;
                    *u = (-38  * r - 74  * g + 112 * b)>>8;
                    *v = ( 157 * r - 132 * g - 26  * b)>>8;
                }
                    break;
                    
                case SYConvert_bitAdd:   // 通过位移来避免乘法运算，精度低
                {
                    *y = ( (r<<6) + (r<<3) + (r<<2) + r + (g<<7) +(g<<4) + (g<<2) + (g<<1) + (b<<4) + (b<<3) + (b<<2) + b)>>8;
                    *u = (-(r<<5) - (r<<2) - (r<<1) - (g<<6) - (g<<3) - (g<<1) + (b<<6) + (b<<5) + (b<<4))>>8;
                    *v = ((r<<7) + (r<<4) + (r<<3) + (r<<2) + r - (g<<7) - (g<<2) - (b<<4) - (b<<3) - (b<<1))>>8;
                }
                    break;
                    
                case SYConvert_table: // 查表法（也是位移计算），精度低
                {
                    *y =  m_yr[r] + m_yg[g] + m_yb[b];
                    *u = -m_ur[r] - m_ug[g] + m_ub[b];
                    *v =  m_vr[r] - m_vg[g] - m_vb[b];
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
                    *y =  0.2126  * r + 0.7152  * g + 0.0722  * b;
                    *u = -0.09991 * r - 0.33609 * g + 0.436   * b;
                    *v =  0.615   * r - 0.55861 * g - 0.05639 * b;
                }
                    break;
                    
                case SYConvert_bitMult:  // 通过位移来避免浮点运算，精度低
                {
                    *y = ( 54  * r + 183 * g + 18  * b)>>8;
                    *u = (-26  * r - 86  * g + 112 * b)>>8;
                    *v = ( 157 * r - 143 * g - 14  * b)>>8;
                }
                    break;
                    
                case SYConvert_bitAdd:   // 通过位移来避免乘法运算，精度低
                {
                    *y = ( (r<<5) + (r<<4) + (r<<2) + (r<<1) + (g<<7) + (g<<5) + (g<<4) + (g<<2) + (g<<1) + g + (b<<4) + (b<<1))>>8;
                    *u = (-(r<<4) - (r<<3) - (r<<1) - (g<<6) - (g<<4) - (g<<2) - (g<<1) + (b<<6) + (b<<5) + (b<<4))>>8;
                    *v = ( (r<<7) + (r<<4) + (r<<3) + (r<<2) + r - (g<<7) - (g<<3) - (g<<2) - (g<<1) - g - (b<<3) - (b<<2) - (b<<1))>>8;
                }
                    break;
                    
                case SYConvert_table: // 查表法（也是位移计算），精度低
                {
                    *y =  m_yr[r] + m_yg[g] + m_yb[b];
                    *u = -m_ur[r] - m_ug[g] + m_ub[b];
                    *v =  m_vr[r] - m_vg[g] - m_vb[b];
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
    /*
     每个像素点 Y'CbCr 值
     由于 U(Cb)、V(Cr) 取值范围是 [﹣128, 127]；
     而在存储时，为了方便存储，跟 Y 数据一样，统一用一个(无符号)字节表示，即取值范围是 [0, 255];
     所以在写入时，需要 将 U(Cb)、V(Cr)的值加上 128，使其变为 ：[0, 255]。
     */
    *y = CLAMP(*y, 0, 255);
    *u = CLAMP(*u + 128, 0, 255);
    *v = CLAMP(*v + 128, 0, 255);
}
