//
//  SYYuvConverter.cpp
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by shenyuanluo on 2018/10/12.
//  Copyright © 2018 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "SYYuvConverter.h"


#pragma mark - Pubcic
#pragma mark -- 构造函数
SYYuvConverter::SYYuvConverter()
{
    
}

#pragma mark -- 析构函数
SYYuvConverter::~SYYuvConverter()
{
    
}

#pragma mark - I420 转换
#pragma mark -- I420 转 NV12
SYKIT_API int SYYuvConverter::SY_I420ToNv12(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len       = width * height; // 像素点数
    const long uvLen     = (len>>2);       // U 或 V 数据长度
    unsigned char *outUV = outYuv + len;   // UV 数据
    
    // 缓存 U、V 数据
    unsigned char* uBuffer = (unsigned char*)malloc(uvLen);
    unsigned char* vBuffer = (unsigned char*)malloc(uvLen);
    memset(uBuffer, 0, uvLen);
    memset(vBuffer, 0, uvLen);
    
    memcpy(outYuv, inYuv, len); // 复制 Y 数据
    memcpy(uBuffer, (inYuv + len), uvLen); // 缓存 U 数据
    memcpy(vBuffer, (inYuv + len + uvLen), uvLen); // 缓存 V 数据
    
    for (int i = 0; i < uvLen; i++)
    {
        memcpy(outUV++, uBuffer + i, 1);   // 先复制 U 数据
        memcpy(outUV++, vBuffer + i, 1);   // 在复制 V 数据
    }
    
    if (uBuffer)
    {
        free(uBuffer);
        uBuffer = NULL;
    }
    if (vBuffer)
    {
        free(vBuffer);
        vBuffer = NULL;
    }
    return SYErr_success;
}

#pragma mark -- I420 转 NV21
SYKIT_API int SYYuvConverter::SY_I420ToNv21(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len       = width * height; // 像素点数
    const long uvLen     = (len>>2);       // U 或 V 数据长度
    unsigned char *outUV = outYuv + len;   // UV 数据
    
    // 缓存 U、V 数据
    unsigned char* uBuffer = (unsigned char*)malloc(uvLen);
    unsigned char* vBuffer = (unsigned char*)malloc(uvLen);
    memset(uBuffer, 0, uvLen);
    memset(vBuffer, 0, uvLen);
    
    memcpy(outYuv, inYuv, len); // 复制 Y 数据
    memcpy(uBuffer, (inYuv + len), uvLen); // 缓存 U 数据
    memcpy(vBuffer, (inYuv + len + uvLen), uvLen); // 缓存 V 数据
    
    for (int i = 0; i < uvLen; i++)
    {
        memcpy(outUV++, vBuffer + i, 1);   // 先复制 V 数据
        memcpy(outUV++, uBuffer + i, 1);   // 再复制 U 数据
    }
    
    if (uBuffer)
    {
        free(uBuffer);
        uBuffer = NULL;
    }
    if (vBuffer)
    {
        free(vBuffer);
        vBuffer = NULL;
    }
    return SYErr_success;
}

#pragma mark - NV12 转换
#pragma mark -- NV12 转 I420
SYKIT_API int SYYuvConverter::SY_Nv12ToI420(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len      = width * height;           // 像素点数
    const long uvLen    = (len>>1);                 // UV 数据长度
    unsigned char *outU = outYuv + len;             // U 数据
    unsigned char *outV = outYuv + len + (len>>2);  // V 数据
    
    // 缓存 UV 数据
    unsigned char* uvBuffer = (unsigned char*)malloc(uvLen);
    memset(uvBuffer, 0, uvLen);
    
    memcpy(outYuv, inYuv, len); // 复制 Y 数据
    memcpy(uvBuffer, (inYuv + len), uvLen); // 缓存 UV 数据
    
    for (int i = 0; i < uvLen; i += 2)
    {
        memcpy(outU++, uvBuffer + i, 1);        // 先复制 U 数据
        memcpy(outV++, uvBuffer + i + 1, 1);    // 再复制 V 数据
    }
    
    if (uvBuffer)
    {
        free(uvBuffer);
        uvBuffer = NULL;
    }
    return SYErr_success;
}

#pragma mark -- NV12 转 NV21
SYKIT_API int SYYuvConverter::SY_Nv12ToNv21(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len       = width * height;           // 像素点数
    const long uvLen     = (len>>1);                 // UV 数据长度
    unsigned char *outVU = outYuv + len;             // VU 数据
    
    // 缓存 UV 数据
    unsigned char* uvBuffer = (unsigned char*)malloc(uvLen);
    memset(uvBuffer, 0, uvLen);
    
    memcpy(outYuv, inYuv, len); // 复制 Y 数据
    memcpy(uvBuffer, (inYuv + len), uvLen); // 缓存 UV 数据
    
    for (int i = 0; i < uvLen; i += 2)
    {
        memcpy(outVU++, uvBuffer + i + 1, 1);    // 先复制 V 数据
        memcpy(outVU++, uvBuffer + i, 1);        // 再复制 U 数据
    }
    
    if (uvBuffer)
    {
        free(uvBuffer);
        uvBuffer = NULL;
    }
    return SYErr_success;
}

#pragma mark - NV12 转换
#pragma mark -- NV21 转 I420
SYKIT_API int SYYuvConverter::SY_Nv21ToI420(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len      = width * height;           // 像素点数
    const long uvLen    = (len>>1);                 // UV 数据长度
    unsigned char *outU = outYuv + len;             // U 数据
    unsigned char *outV = outYuv + len + (len>>2);  // V 数据
    
    // 缓存 VU 数据
    unsigned char* vuBuffer = (unsigned char*)malloc(uvLen);
    memset(vuBuffer, 0, uvLen);
    
    memcpy(outYuv, inYuv, len); // 复制 Y 数据
    memcpy(vuBuffer, (inYuv + len), uvLen); // 缓存 UV 数据
    
    for (int i = 0; i < uvLen; i += 2)
    {
        memcpy(outV++, vuBuffer + i, 1);        // 先复制 V 数据
        memcpy(outU++, vuBuffer + i + 1, 1);    // 再复制 U 数据
    }
    
    if (vuBuffer)
    {
        free(vuBuffer);
        vuBuffer = NULL;
    }
    return SYErr_success;
}

#pragma mark -- NV21 转 NV12
SYKIT_API int SYYuvConverter::SY_Nv21ToNv12(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv) const
{
    if (NULL == inYuv || NULL == outYuv || 0 == width || 0 == height)
    {
        return SYErr_paramError;
    }
    const long len       = width * height;           // 像素点数
    const long vuLen     = (len>>1);                 // VU 数据长度
    unsigned char *outUV = outYuv + len;             // UV 数据
    
    // 缓存 UV 数据
    unsigned char* uvBuffer = (unsigned char*)malloc(vuLen);
    memset(uvBuffer, 0, vuLen);
    
    memcpy(outYuv, inYuv, len); // 复制 Y 数据
    memcpy(uvBuffer, (inYuv + len), vuLen); // 缓存 UV 数据
    
    for (int i = 0; i < vuLen; i += 2)
    {
        memcpy(outUV++, uvBuffer + i + 1, 1);    // 先复制 U 数据
        memcpy(outUV++, uvBuffer + i, 1);        // 再复制 V 数据
    }
    
    if (uvBuffer)
    {
        free(uvBuffer);
        uvBuffer = NULL;
    }
    return SYErr_success;
}
