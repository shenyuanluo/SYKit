//
//  main.cpp
//  SYKitExample
//
//  Created by ShenYuanLuo on 2018/8/23.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include "SYYuvToRgb.h"
#include "SYRgbToYuv.h"
#include "SYYuvToBmp.h"
#include "SYRgbToBmp.h"
#include "SYClipper.h"

#include "SYRotate.h"
#include "SYMirror.h"
#include "SYYuvConverter.h"


#define YUV_WIDTH  480              // 视频帧宽
#define YUV_HEIGHT 360              // 视频帧高
#define I420_BUFF_SIZE  259200      // I420 缓冲大小（YUV_WIDTH * YUV_HEIGHT * 1.5）
#define NV12_BUFF_SIZE  259200      // NV12 缓冲大小（YUV_WIDTH * YUV_HEIGHT * 1.5）
#define NV21_BUFF_SIZE  259200      // NV12 缓冲大小（YUV_WIDTH * YUV_HEIGHT * 1.5）
#define RGB_565_BUFF_SIZE 345600    // RGB565缓冲大小（YUV_WIDTH * YUV_HEIGHT * 2）
#define RGB_24_BUFF_SIZE  518400    // RGB24 缓冲大小（YUV_WIDTH * YUV_HEIGHT * 3）

#define MARK_WIDTH  120             // 水印帧宽
#define MARK_HEIGHT 90              // 水印帧高
#define MARK_BUFF_SIZE 16200        // 水印帧缓冲大小（MARK_WIDTH * MARK_HEIGHT * 1.5）


#define LOOP_COUNT 1   // 循环次数

#define CLIP_COUNT 16    // 裁剪区域个数


#pragma mark -- 获取当前时间
long long currTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    
//    return tv.tv_sec;   // 秒
//    return tv.tv_sec * 1000 + tv.tv_usec/1000;    // 毫秒
    return tv.tv_sec * 1000000 + tv.tv_usec;    // 微妙
}

#pragma mark - Test convert to rgb
#pragma mark -- 测试 I420 转 RGB565
void testI420ToRgb565()
{
    SYYuvToRgb converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *yuv    = (unsigned char *)malloc(I420_BUFF_SIZE);
    unsigned char *rgb565 = (unsigned char *)malloc(RGB_565_BUFF_SIZE);
    if (NULL == yuv || NULL == rgb565)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fyuv    = fopen("XinWenLianBo_480x360_I420.yuv", "rb+");   // 打开 YUV 文件
    FILE *frgb565 = fopen("XinWenLianBo_480x360_RGB565.rgb", "wb+"); // 打开 RGB565 文件
    
    if (NULL == fyuv  || NULL == frgb565)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb565);
        yuv    = NULL;
        rgb565 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fyuv))
        {
            // 清空内存
            memset(yuv, 0, I420_BUFF_SIZE);
            memset(rgb565, 0, RGB_565_BUFF_SIZE);
            
            fread(yuv, 1, I420_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
            
            converter.SY_I420ToRgb565(yuv, YUV_WIDTH, YUV_HEIGHT, rgb565);     // 转换成 RGB565
            
            fwrite(rgb565, RGB_565_BUFF_SIZE, 1, frgb565);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb565, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb565);
    free(yuv);
    free(rgb565);
    yuv    = NULL;
    rgb565 = NULL;
}

#pragma mark -- 测试 NV12 转 RGB565
void testNv12ToRgb565()
{
    SYYuvToRgb converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *yuv    = (unsigned char *)malloc(NV12_BUFF_SIZE);
    unsigned char *rgb565 = (unsigned char *)malloc(RGB_565_BUFF_SIZE);
    if (NULL == yuv || NULL == rgb565)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fyuv    = fopen("XinWenLianBo_480x360_NV12.yuv", "rb+");    // 打开 YUV 文件
    FILE *frgb565 = fopen("XinWenLianBo_480x360_RGB565.rgb", "wb+");  // 打开 RGB565 文件
    
    if (NULL == fyuv || NULL == frgb565)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb565);
        yuv   = NULL;
        rgb565 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fyuv))
        {
            // 清空内存
            memset(yuv, 0, NV12_BUFF_SIZE);
            memset(rgb565, 0, RGB_565_BUFF_SIZE);
            
            fread(yuv, 1, NV12_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
            
            converter.SY_Nv12ToRgb565(yuv, YUV_WIDTH, YUV_HEIGHT, rgb565);    // 转换成 RGB565
            
            fwrite(rgb565, RGB_565_BUFF_SIZE, 1, frgb565);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb565, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb565);
    free(yuv);
    free(rgb565);
    yuv    = NULL;
    rgb565 = NULL;
}

#pragma mark -- 测试 NV21 转 RGB565
void testNv21ToRgb565()
{
    SYYuvToRgb converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *yuv   = (unsigned char *)malloc(NV21_BUFF_SIZE);
    unsigned char *rgb565 = (unsigned char *)malloc(RGB_565_BUFF_SIZE);
    if (NULL == yuv || NULL == rgb565)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fyuv   = fopen("XinWenLianBo_480x360_NV21.yuv", "rb+");   // 打开 YUV 文件
    FILE *frgb565 = fopen("XinWenLianBo_480x360_RGB565.rgb", "wb+");  // 打开 RGB565 文件
    
    if (NULL == fyuv || NULL == frgb565)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb565);
        yuv   = NULL;
        rgb565 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fyuv))
        {
            // 清空内存
            memset(yuv, 0, NV21_BUFF_SIZE);
            memset(rgb565, 0, RGB_565_BUFF_SIZE);
            
            fread(yuv, 1, NV21_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
            
            converter.SY_Nv21ToRgb565(yuv, YUV_WIDTH, YUV_HEIGHT, rgb565);    // 转换成 RGB565
            
            fwrite(rgb565, RGB_565_BUFF_SIZE, 1, frgb565);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb565, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb565);
    free(yuv);
    free(rgb565);
    yuv    = NULL;
    rgb565 = NULL;
}

#pragma mark -- 测试 I420 转 RGB24
void testI420ToRgb24()
{
    SYYuvToRgb converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *yuv   = (unsigned char *)malloc(I420_BUFF_SIZE);
    unsigned char *rgb24 = (unsigned char *)malloc(RGB_24_BUFF_SIZE);
    if (NULL == yuv || NULL == rgb24)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fyuv   = fopen("XinWenLianBo_480x360_I420.yuv", "rb+");   // 打开 YUV 文件
    FILE *frgb24 = fopen("XinWenLianBo_480x360_RGB24.rgb", "wb+");  // 打开 RGB24 文件
    
    if (NULL == fyuv || NULL == frgb24)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb24);
        yuv   = NULL;
        rgb24 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fyuv))
        {
            // 清空内存
            memset(yuv, 0, I420_BUFF_SIZE);
            memset(rgb24, 0, RGB_24_BUFF_SIZE);
            
            fread(yuv, 1, I420_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
            
            converter.SY_I420ToRgb24(yuv, YUV_WIDTH, YUV_HEIGHT, rgb24);    // 转换成 RGB24
            
            fwrite(rgb24, RGB_24_BUFF_SIZE, 1, frgb24);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb24, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb24);
    free(yuv);
    free(rgb24);
    yuv   = NULL;
    rgb24 = NULL;
}

#pragma mark -- 测试 NV12 转 RGB24
void testNv12ToRgb24()
{
    SYYuvToRgb converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *yuv   = (unsigned char *)malloc(NV12_BUFF_SIZE);
    unsigned char *rgb24 = (unsigned char *)malloc(RGB_24_BUFF_SIZE);
    if (NULL == yuv || NULL == rgb24)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fyuv   = fopen("XinWenLianBo_480x360_NV12.yuv", "rb+");   // 打开 YUV 文件
    FILE *frgb24 = fopen("XinWenLianBo_480x360_RGB24.rgb", "wb+");  // 打开 RGB24 文件
    
    if (NULL == fyuv || NULL == frgb24)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb24);
        yuv   = NULL;
        rgb24 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fyuv))
        {
            // 清空内存
            memset(yuv, 0, NV12_BUFF_SIZE);
            memset(rgb24, 0, RGB_24_BUFF_SIZE);
            
            fread(yuv, 1, NV12_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
            
            converter.SY_Nv12ToRgb24(yuv, YUV_WIDTH, YUV_HEIGHT, rgb24);    // 转换成 RGB24
            
            fwrite(rgb24, RGB_24_BUFF_SIZE, 1, frgb24);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb24, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb24);
    free(yuv);
    free(rgb24);
    yuv   = NULL;
    rgb24 = NULL;
}

#pragma mark -- 测试 NV21 转 RGB24
void testNv21ToRgb24()
{
    SYYuvToRgb converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *yuv   = (unsigned char *)malloc(NV21_BUFF_SIZE);
    unsigned char *rgb24 = (unsigned char *)malloc(RGB_24_BUFF_SIZE);
    if (NULL == yuv || NULL == rgb24)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fyuv   = fopen("XinWenLianBo_480x360_NV21.yuv", "rb+");   // 打开 YUV 文件
    FILE *frgb24 = fopen("XinWenLianBo_480x360_RGB24.rgb", "wb+");  // 打开 RGB24 文件
    
    if (NULL == fyuv || NULL == frgb24)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb24);
        yuv   = NULL;
        rgb24 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fyuv))
        {
            // 清空内存
            memset(yuv, 0, NV21_BUFF_SIZE);
            memset(rgb24, 0, RGB_24_BUFF_SIZE);
            
            fread(yuv, 1, NV21_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
            
            converter.SY_Nv21ToRgb24(yuv, YUV_WIDTH, YUV_HEIGHT, rgb24);    // 转换成 RGB24
            
            fwrite(rgb24, RGB_24_BUFF_SIZE, 1, frgb24);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb24, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb24);
    free(yuv);
    free(rgb24);
    yuv   = NULL;
    rgb24 = NULL;
}

#pragma mark - Test convert to yuv
#pragma mark -- 测试 RGB565 转 I420
void testRgb565ToI420()
{
    SYRgbToYuv converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *rgb565 = (unsigned char *)malloc(RGB_565_BUFF_SIZE);
    unsigned char *yuv    = (unsigned char *)malloc(I420_BUFF_SIZE);
    if (NULL == rgb565 || NULL == yuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *frgb565 = fopen("XinWenLianBo_480x360_RGB565.rgb", "rb+");  // 打开 RGB565 文件
    FILE *fyuv    = fopen("XinWenLianBo_480x360_I420.yuv", "wb+");    // 打开 YUV 文件
    
    if (NULL == frgb565 || NULL == fyuv)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb565);
        yuv    = NULL;
        rgb565 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(frgb565))
        {
            // 清空内存
            memset(rgb565, 0, RGB_565_BUFF_SIZE);
            memset(yuv, 0, I420_BUFF_SIZE);
            
            fread(rgb565, RGB_565_BUFF_SIZE, 1, frgb565);  // 每次读取一帧 RGB565 数据
            
            converter.SY_Rgb565ToI420(rgb565, YUV_WIDTH, YUV_HEIGHT, yuv);   // 转换成 I420
            
            fwrite(yuv, I420_BUFF_SIZE, 1, fyuv);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb565, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb565);
    free(yuv);
    free(rgb565);
    yuv    = NULL;
    rgb565 = NULL;
}

#pragma mark -- 测试 RGB565 转 NV12
void testRgb565ToNv12()
{
    SYRgbToYuv converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *rgb565 = (unsigned char *)malloc(RGB_565_BUFF_SIZE);
    unsigned char *yuv    = (unsigned char *)malloc(NV12_BUFF_SIZE);
    if (NULL == rgb565 || NULL == yuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *frgb565 = fopen("XinWenLianBo_480x360_RGB565.rgb", "rb+");  // 打开 RGB565 文件
    FILE *fyuv    = fopen("XinWenLianBo_480x360_NV12.yuv", "wb+");    // 打开 YUV 文件
    
    if (NULL == frgb565 || NULL == fyuv)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb565);
        yuv    = NULL;
        rgb565 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(frgb565))
        {
            // 清空内存
            memset(rgb565, 0, RGB_565_BUFF_SIZE);
            memset(yuv, 0, NV12_BUFF_SIZE);
            
            fread(rgb565, RGB_565_BUFF_SIZE, 1, frgb565);  // 每次读取一帧 RGB565 数据
            
            converter.SY_Rgb565ToNv12(rgb565, YUV_WIDTH, YUV_HEIGHT, yuv);   // 转换成 NV12
            
            fwrite(yuv, NV12_BUFF_SIZE, 1, fyuv);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb565, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb565);
    free(yuv);
    free(rgb565);
    yuv    = NULL;
    rgb565 = NULL;
}

#pragma mark -- 测试 RGB565 转 NV21
void testRgb565ToNv21()
{
    SYRgbToYuv converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *rgb565 = (unsigned char *)malloc(RGB_565_BUFF_SIZE);
    unsigned char *yuv    = (unsigned char *)malloc(NV21_BUFF_SIZE);
    if (NULL == rgb565 || NULL == yuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *frgb565 = fopen("XinWenLianBo_480x360_RGB565.rgb", "rb+");  // 打开 RGB565 文件
    FILE *fyuv    = fopen("XinWenLianBo_480x360_NV21.yuv", "wb+");    // 打开 YUV 文件
    
    if (NULL == frgb565 || NULL == fyuv)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb565);
        yuv    = NULL;
        rgb565 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(frgb565))
        {
            // 清空内存
            memset(rgb565, 0, RGB_565_BUFF_SIZE);
            memset(yuv, 0, NV21_BUFF_SIZE);
            
            fread(rgb565, RGB_565_BUFF_SIZE, 1, frgb565);  // 每次读取一帧 RGB565 数据
            
            converter.SY_Rgb565ToNv21(rgb565, YUV_WIDTH, YUV_HEIGHT, yuv);   // 转换成 NV21
            
            fwrite(yuv, NV21_BUFF_SIZE, 1, fyuv);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb565, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb565);
    free(yuv);
    free(rgb565);
    yuv    = NULL;
    rgb565 = NULL;
}

#pragma mark -- 测试 RGB24 转 I420
void testRgb24ToI420()
{
    SYRgbToYuv converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *rgb24 = (unsigned char *)malloc(RGB_24_BUFF_SIZE);
    unsigned char *yuv   = (unsigned char *)malloc(I420_BUFF_SIZE);
    if (NULL == rgb24 || NULL == yuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *frgb24 = fopen("XinWenLianBo_480x360_RGB24.rgb", "rb+");  // 打开 RGB24 文件
    FILE *fyuv   = fopen("XinWenLianBo_480x360_I420.yuv", "wb+");   // 打开 YUV 文件
    
    if (NULL == frgb24 || NULL == fyuv)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb24);
        yuv   = NULL;
        rgb24 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(frgb24))
        {
            // 清空内存
            memset(rgb24, 0, RGB_24_BUFF_SIZE);
            memset(yuv, 0, I420_BUFF_SIZE);
            
            fread(rgb24, RGB_24_BUFF_SIZE, 1, frgb24);  // 每次读取一帧 RGB24 数据
            
            converter.SY_Rgb24ToI420(rgb24, YUV_WIDTH, YUV_HEIGHT, yuv);   // 转换成 I420
            
            fwrite(yuv, I420_BUFF_SIZE, 1, fyuv);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb24, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb24);
    free(yuv);
    free(rgb24);
    yuv   = NULL;
    rgb24 = NULL;
}

#pragma mark -- 测试 RGB24 转 NV12
void testRgb24ToNv12()
{
    SYRgbToYuv converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *rgb24 = (unsigned char *)malloc(RGB_24_BUFF_SIZE);
    unsigned char *yuv   = (unsigned char *)malloc(NV12_BUFF_SIZE);
    if (NULL == rgb24 || NULL == yuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *frgb24 = fopen("XinWenLianBo_480x360_RGB24.rgb", "rb+");  // 打开 RGB24 文件
    FILE *fyuv   = fopen("XinWenLianBo_480x360_NV12.yuv", "wb+");   // 打开 YUV 文件
    
    if (NULL == frgb24 || NULL == fyuv)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb24);
        yuv   = NULL;
        rgb24 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(frgb24))
        {
            // 清空内存
            memset(rgb24, 0, RGB_24_BUFF_SIZE);
            memset(yuv, 0, NV12_BUFF_SIZE);
            
            fread(rgb24, RGB_24_BUFF_SIZE, 1, frgb24);  // 每次读取一帧 RGB24 数据
            
            converter.SY_Rgb24ToNv12(rgb24, YUV_WIDTH, YUV_HEIGHT, yuv);   // 转换成 NV12
            
            fwrite(yuv, NV12_BUFF_SIZE, 1, fyuv);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb24, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb24);
    free(yuv);
    free(rgb24);
    yuv   = NULL;
    rgb24 = NULL;
}

#pragma mark -- 测试 RGB24 转 NV21
void testRgb24ToNv21()
{
    SYRgbToYuv converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *rgb24 = (unsigned char *)malloc(RGB_24_BUFF_SIZE);
    unsigned char *yuv   = (unsigned char *)malloc(NV21_BUFF_SIZE);
    if (NULL == rgb24 || NULL == yuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *frgb24 = fopen("XinWenLianBo_480x360_RGB24.rgb", "rb+");  // 打开 RGB24 文件
    FILE *fyuv   = fopen("XinWenLianBo_480x360_NV21.yuv", "wb+");   // 打开 YUV 文件
    
    if (NULL == frgb24 || NULL == fyuv)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rgb24);
        yuv   = NULL;
        rgb24 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(frgb24))
        {
            // 清空内存
            memset(rgb24, 0, RGB_24_BUFF_SIZE);
            memset(yuv, 0, NV21_BUFF_SIZE);
            
            fread(rgb24, RGB_24_BUFF_SIZE, 1, frgb24);  // 每次读取一帧 RGB24 数据
            
            converter.SY_Rgb24ToNv21(rgb24, YUV_WIDTH, YUV_HEIGHT, yuv);   // 转换成 NV21
            
            fwrite(yuv, NV21_BUFF_SIZE, 1, fyuv);
        }
        endTime = currTime();
        
        fseek(fyuv, 0, SEEK_SET);
        fseek(frgb24, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fyuv);
    fclose(frgb24);
    free(yuv);
    free(rgb24);
    yuv   = NULL;
    rgb24 = NULL;
}


#pragma mark - Test convert to bmp
#pragma mark -- 测试 YUV 转 BMP
void testYuvToBmp()
{
    SYYuvToBmp converter;
    converter.SY_SetMatrixType(SYMatrix_normal);
    converter.SY_SetConvertType(SYConvert_normal);
    
    unsigned char *yuv   = (unsigned char *)malloc(I420_BUFF_SIZE);
    if (NULL == yuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    // I420
    converter.SY_SetYuvType(SYYuv_i420);
    FILE *fyuv   = fopen("XinWenLianBo_480x360_I420.yuv", "rb+");   // 打开 YUV 文件
    
    // Nv12
//    converter.SY_SetYuvType(SYYuv_nv12);
//    FILE *fyuv   = fopen("XinWenLianBo_480x360_NV12.yuv", "rb+");   // 打开 YUV 文件
    
    // NV21
//    converter.SY_SetYuvType(SYYuv_nv21);
//    FILE *fyuv   = fopen("XinWenLianBo_480x360_NV21.yuv", "rb+");   // 打开 YUV 文件
    
    if (NULL == fyuv)
    {
        printf("Open file failure!\n");
        free(yuv);
        yuv   = NULL;
        
        return;
    }
    static int count = 0;
    char fileName[64];
    while (!feof(fyuv))
    {
        // 清空内存
        memset(yuv, 0, I420_BUFF_SIZE);
        fread(yuv, 1, I420_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
        count++;
        sprintf(fileName, "./BMP/XinWenLianBo_480x360_Frame_%d.bmp", count);
        converter.SY_YuvToBmp(yuv, YUV_WIDTH, YUV_HEIGHT, fileName);    // 转换成 BMP
    }
    fclose(fyuv);
    free(yuv);
    yuv   = NULL;
}


#pragma mark -- RGB 转 BMP
void testRgbToBmp()
{
    SYRgbToBmp converter;
    
    // RGB565
//    converter.SY_SetRgbType(SYRgb_rgb565);
//    size_t bufLen = RGB_565_BUFF_SIZE;
//    FILE *frgb = fopen("XinWenLianBo_480x360_RGB565.rgb", "rb+");  // 打开 RGB565 文件
    
    // RGB24
    converter.SY_SetRgbType(SYRgb_rgb24);
    size_t bufLen = RGB_24_BUFF_SIZE;
    FILE *frgb = fopen("XinWenLianBo_480x360_RGB24.rgb", "rb+");  // 打开 RGB24 文件
    
    if (NULL == frgb)
    {
        printf("Open file failure!\n");
        return;
    }
    
    unsigned char *rgb = (unsigned char *)malloc(bufLen);
    if (NULL == rgb)
    {
        printf("Malloc data buffer failure!\n");
        fclose(frgb);
        return;
    }
    
    static int count = 0;
    char fileName[64];
    while (!feof(frgb))
    {
        // 清空内存
        memset(rgb, 0, bufLen);
        fread(rgb, 1, bufLen, frgb);  // 每次读取一帧 RGB565 数据
        count++;
        sprintf(fileName, "./BMP/XinWenLianBo_480x360_Frame_%d.bmp", count);
        converter.SY_RgbToBmp(rgb, YUV_WIDTH, YUV_HEIGHT, fileName);
    }
    fclose(frgb);
    free(rgb);
    rgb = NULL;
}

#pragma mark - Test clipper
#pragma mark -- 裁剪 YUV
void testClipYuv()
{
    unsigned char *srcYuv = (unsigned char *)malloc(I420_BUFF_SIZE);
    if (NULL == srcYuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fyuv = fopen("XinWenLianBo_480x360_I420.yuv", "rb+");   // 打开 YUV 文件
    if (NULL == fyuv)
    {
        printf("Open file failure!\n");
        free(srcYuv);
        srcYuv = NULL;
        
        return;
    }
    unsigned char* dstYuv[CLIP_COUNT];
    unsigned int   dstLen[CLIP_COUNT];
    SYRect       clipRect[CLIP_COUNT];    // 平均分割 16 份
    
    // First row
    clipRect[0] = {0 * (YUV_WIDTH>>2), 0, 1 * (YUV_WIDTH>>2), 1 * (YUV_HEIGHT>>2)};
    clipRect[1] = {1 * (YUV_WIDTH>>2), 0, 2 * (YUV_WIDTH>>2), 1 * (YUV_HEIGHT>>2)};
    clipRect[2] = {2 * (YUV_WIDTH>>2), 0, 3 * (YUV_WIDTH>>2), 1 * (YUV_HEIGHT>>2)};
    clipRect[3] = {3 * (YUV_WIDTH>>2), 0, 4 * (YUV_WIDTH>>2), 1 * (YUV_HEIGHT>>2)};
    // Second row
    clipRect[4] = {0 * (YUV_WIDTH>>2), (YUV_HEIGHT>>2), 1 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2)};
    clipRect[5] = {1 * (YUV_WIDTH>>2), (YUV_HEIGHT>>2), 2 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2)};
    clipRect[6] = {2 * (YUV_WIDTH>>2), (YUV_HEIGHT>>2), 3 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2)};
    clipRect[7] = {3 * (YUV_WIDTH>>2), (YUV_HEIGHT>>2), 4 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2)};
    // third row
    clipRect[8]  = {0 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2), 1 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2)};
    clipRect[9]  = {1 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2), 2 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2)};
    clipRect[10] = {2 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2), 3 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2)};
    clipRect[11] = {3 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2), 4 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2)};
    // Fourth row
    clipRect[12] = {0 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2), 1 * (YUV_WIDTH>>2), 4 * (YUV_HEIGHT>>2)};
    clipRect[13] = {1 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2), 2 * (YUV_WIDTH>>2), 4 * (YUV_HEIGHT>>2)};
    clipRect[14] = {2 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2), 3 * (YUV_WIDTH>>2), 4 * (YUV_HEIGHT>>2)};
    clipRect[15] = {3 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2), 4 * (YUV_WIDTH>>2), 4 * (YUV_HEIGHT>>2)};
    
    SYClipper cliper(YUV_WIDTH, YUV_HEIGHT);
    cliper.SY_SetYuvType(SYYuv_i420);
    SYYuvToBmp converter;
    converter.SY_SetConvertType(SYConvert_table);
    converter.SY_SetYuvType(SYYuv_i420);
    
    unsigned int frameNo = 0;
    char fileName[64];
    
    FILE* supYuv = fopen("./BMP/SupYuv.yuv", "wb+");
    FILE* subYuv = fopen("./BMP/SubYuv.yuv", "wb+");
    
    while (!feof(fyuv))
    {
        memset(srcYuv, 0, I420_BUFF_SIZE);
        fread(srcYuv, 1, I420_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
        
        if (342 == frameNo)
        {
            cliper.SY_ClipYuv(srcYuv, I420_BUFF_SIZE, dstYuv, dstLen, clipRect, CLIP_COUNT);
            
            fwrite(srcYuv, 1, I420_BUFF_SIZE, supYuv);
            sprintf(fileName, "./BMP/Frame_%d.bmp", frameNo);
            converter.SY_YuvToBmp(srcYuv, YUV_WIDTH, YUV_HEIGHT, fileName);
            
            for (int j = 0; j < CLIP_COUNT; j++)
            {
                if (11 == j)
                {
                    fwrite(dstYuv[j], 1, (clipRect[j].brX - clipRect[j].tlX) * (clipRect[j].brY - clipRect[j].tlY) * 1.5, subYuv);
                }
                sprintf(fileName, "./BMP/Frame_%d_Clip_%d.bmp", frameNo, j);
                converter.SY_YuvToBmp(dstYuv[j], clipRect[j].brX - clipRect[j].tlX, clipRect[j].brY - clipRect[j].tlY, fileName);    // 转换成 BMP
            }
            printf("裁剪完成：width-%d, height-%d\n", clipRect[0].brX - clipRect[0].tlX, clipRect[0].brY - clipRect[0].tlY);
            break;
        }
        frameNo++;
    }
    fclose(supYuv);
    fclose(subYuv);
    fclose(fyuv);
    free(srcYuv);
    srcYuv = NULL;
}

#pragma mark -- 裁剪 RGB
void testClipRgb()
{
    unsigned char *srcRgb = (unsigned char *)malloc(RGB_24_BUFF_SIZE);
    if (NULL == srcRgb)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *frgb = fopen("XinWenLianBo_480x360_RGB565.rgb", "rb+");   // 打开 RGB 文件
    if (NULL == frgb)
    {
        printf("Open file failure!\n");
        free(srcRgb);
        srcRgb = NULL;
        
        return;
    }
    unsigned char* dstRgb[CLIP_COUNT];
    unsigned int   dstLen[CLIP_COUNT];
    SYRect       clipRect[CLIP_COUNT];    // 平均分割 16 份

    // First row
    clipRect[0] = {0 * (YUV_WIDTH>>2), 0, 1 * (YUV_WIDTH>>2), 1 * (YUV_HEIGHT>>2)};
    clipRect[1] = {1 * (YUV_WIDTH>>2), 0, 2 * (YUV_WIDTH>>2), 1 * (YUV_HEIGHT>>2)};
    clipRect[2] = {2 * (YUV_WIDTH>>2), 0, 3 * (YUV_WIDTH>>2), 1 * (YUV_HEIGHT>>2)};
    clipRect[3] = {3 * (YUV_WIDTH>>2), 0, 4 * (YUV_WIDTH>>2), 1 * (YUV_HEIGHT>>2)};
    // Second row
    clipRect[4] = {0 * (YUV_WIDTH>>2), (YUV_HEIGHT>>2), 1 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2)};
    clipRect[5] = {1 * (YUV_WIDTH>>2), (YUV_HEIGHT>>2), 2 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2)};
    clipRect[6] = {2 * (YUV_WIDTH>>2), (YUV_HEIGHT>>2), 3 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2)};
    clipRect[7] = {3 * (YUV_WIDTH>>2), (YUV_HEIGHT>>2), 4 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2)};
    // third row
    clipRect[8]  = {0 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2), 1 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2)};
    clipRect[9]  = {1 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2), 2 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2)};
    clipRect[10] = {2 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2), 3 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2)};
    clipRect[11] = {3 * (YUV_WIDTH>>2), 2 * (YUV_HEIGHT>>2), 4 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2)};
    // Fourth row
    clipRect[12] = {0 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2), 1 * (YUV_WIDTH>>2), 4 * (YUV_HEIGHT>>2)};
    clipRect[13] = {1 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2), 2 * (YUV_WIDTH>>2), 4 * (YUV_HEIGHT>>2)};
    clipRect[14] = {2 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2), 3 * (YUV_WIDTH>>2), 4 * (YUV_HEIGHT>>2)};
    clipRect[15] = {3 * (YUV_WIDTH>>2), 3 * (YUV_HEIGHT>>2), 4 * (YUV_WIDTH>>2), 4 * (YUV_HEIGHT>>2)};
    
    SYClipper cliper(YUV_WIDTH, YUV_HEIGHT);
    cliper.SY_SetRgbType(SYRgb_rgb565);
    SYRgbToBmp converter;
    converter.SY_SetRgbType(SYRgb_rgb565);
    
    unsigned int frameNo = 0;
    char fileName[64];
    
    FILE* supRgb = fopen("./BMP/SupRgb.rgb", "wb+");
    FILE* subRgb = fopen("./BMP/SubRgb.rgb", "wb+");
    
    while (!feof(frgb))
    {
        memset(srcRgb, 0, RGB_565_BUFF_SIZE);
        fread(srcRgb, 1, RGB_565_BUFF_SIZE, frgb);  // 每次读取一帧 YUV 数据
        
        if (342 == frameNo)
        {
            cliper.SY_ClipRgb(srcRgb, RGB_565_BUFF_SIZE, dstRgb, dstLen, clipRect, CLIP_COUNT);
            
            fwrite(srcRgb, 1, RGB_565_BUFF_SIZE, supRgb);
            sprintf(fileName, "./BMP/Frame_%d.bmp", frameNo);
            converter.SY_RgbToBmp(srcRgb, YUV_WIDTH, YUV_HEIGHT, fileName);
            
            for (int j = 0; j < CLIP_COUNT; j++)
            {
                if (11 == j)
                {
                    fwrite(dstRgb[j], 1, (clipRect[j].brX - clipRect[j].tlX) * (clipRect[j].brY - clipRect[j].tlY) * 2, subRgb);
                }
                sprintf(fileName, "./BMP/Frame_%d_Clip_%d.bmp", frameNo, j);
                converter.SY_RgbToBmp(dstRgb[j], clipRect[j].brX - clipRect[j].tlX, clipRect[j].brY - clipRect[j].tlY, fileName);    // 转换成 BMP
            }
            printf("裁剪完成：width-%d, height-%d\n", clipRect[0].brX - clipRect[0].tlX, clipRect[0].brY - clipRect[0].tlY);
            break;
        }
        frameNo++;
    }
    fclose(supRgb);
    fclose(subRgb);
    fclose(frgb);
    free(srcRgb);
    srcRgb = NULL;
}


#pragma mark - YUV 格式相互转换
#pragma mark -- I420 转 NV12
void testI420ToNv12()
{
    SYYuvConverter converter;
    
    unsigned char *i420 = (unsigned char *)malloc(I420_BUFF_SIZE);
    unsigned char *nv12 = (unsigned char *)malloc(NV12_BUFF_SIZE);
    if (NULL == i420 || NULL == nv12)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fI420 = fopen("XinWenLianBo_480x360_I420.yuv", "rb+");  // 打开 I420 文件
    FILE *fNv12 = fopen("XinWenLianBo_480x360_NV12.yuv", "wb+");  // 打开 NV12 文件
    
    if (NULL == fI420 || NULL == fNv12)
    {
        printf("Open file failure!\n");
        free(i420);
        free(nv12);
        i420 = NULL;
        nv12 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fI420))
        {
            // 清空内存
            memset(i420, 0, I420_BUFF_SIZE);
            memset(nv12, 0, NV12_BUFF_SIZE);
            
            fread(i420, 1, I420_BUFF_SIZE, fI420);  // 每次读取一帧 YUV 数据
            
            converter.SY_I420ToNv12(i420, YUV_WIDTH, YUV_HEIGHT, nv12); // 转换成 NV12
            
            fwrite(nv12, NV12_BUFF_SIZE, 1, fNv12);
        }
        endTime = currTime();
        
        fseek(fI420, 0, SEEK_SET);
        fseek(fNv12, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fI420);
    fclose(fNv12);
    free(i420);
    free(nv12);
    i420   = NULL;
    nv12 = NULL;
}

#pragma mark -- I420 转 NV21
void testI420ToNv21()
{
    SYYuvConverter converter;
    
    unsigned char *i420 = (unsigned char *)malloc(I420_BUFF_SIZE);
    unsigned char *nv21 = (unsigned char *)malloc(NV21_BUFF_SIZE);
    if (NULL == i420 || NULL == nv21)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fI420   = fopen("XinWenLianBo_480x360_I420.yuv", "rb+");  // 打开 I420 文件
    FILE *fNv21 = fopen("XinWenLianBo_480x360_NV21.yuv", "wb+");  // 打开 NV21 文件
    
    if (NULL == fI420 || NULL == fNv21)
    {
        printf("Open file failure!\n");
        free(i420);
        free(nv21);
        i420 = NULL;
        nv21 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fI420))
        {
            // 清空内存
            memset(i420, 0, I420_BUFF_SIZE);
            memset(nv21, 0, NV21_BUFF_SIZE);
            
            fread(i420, 1, I420_BUFF_SIZE, fI420);  // 每次读取一帧 YUV 数据
            
            converter.SY_I420ToNv21(i420, YUV_WIDTH, YUV_HEIGHT, nv21); // 转换成 NV21
            
            fwrite(nv21, NV21_BUFF_SIZE, 1, fNv21);
        }
        endTime = currTime();
        
        fseek(fI420, 0, SEEK_SET);
        fseek(fNv21, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fI420);
    fclose(fNv21);
    free(i420);
    free(nv21);
    i420   = NULL;
    nv21 = NULL;
}

#pragma mark -- NV12 转 I420
void testNv12ToI420()
{
    SYYuvConverter converter;
    
    unsigned char *i420 = (unsigned char *)malloc(I420_BUFF_SIZE);
    unsigned char *nv12 = (unsigned char *)malloc(NV12_BUFF_SIZE);
    if (NULL == i420 || NULL == nv12)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fI420   = fopen("XinWenLianBo_480x360_I420.yuv", "wb+");  // 打开 I420 文件
    FILE *fNv12 = fopen("XinWenLianBo_480x360_NV12.yuv", "rb+");  // 打开 NV12 文件
    
    if (NULL == fI420 || NULL == fNv12)
    {
        printf("Open file failure!\n");
        free(i420);
        free(nv12);
        i420 = NULL;
        nv12 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fNv12))
        {
            // 清空内存
            memset(i420, 0, I420_BUFF_SIZE);
            memset(nv12, 0, NV12_BUFF_SIZE);
            
            fread(nv12, 1, NV12_BUFF_SIZE, fNv12);  // 每次读取一帧 YUV 数据
            
            converter.SY_Nv12ToI420(nv12, YUV_WIDTH, YUV_HEIGHT, i420); // 转换成 I420
            
            fwrite(i420, I420_BUFF_SIZE, 1, fI420);
        }
        endTime = currTime();
        
        fseek(fI420, 0, SEEK_SET);
        fseek(fNv12, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fI420);
    fclose(fNv12);
    free(i420);
    free(nv12);
    i420   = NULL;
    nv12 = NULL;
}

#pragma mark -- NV12 转 NV21
void testNv12ToNv21()
{
    SYYuvConverter converter;
    
    unsigned char *nv12 = (unsigned char *)malloc(NV12_BUFF_SIZE);
    unsigned char *nv21 = (unsigned char *)malloc(NV21_BUFF_SIZE);
    if (NULL == nv12 || NULL == nv21)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fNv12 = fopen("XinWenLianBo_480x360_NV12.yuv", "rb+");  // 打开 NV12 文件
    FILE *fNv21 = fopen("XinWenLianBo_480x360_NV21.yuv", "wb+");  // 打开 NV21 文件
    
    
    if (NULL == fNv21 || NULL == fNv12)
    {
        printf("Open file failure!\n");
        free(nv21);
        free(nv12);
        nv21 = NULL;
        nv12 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fNv12))
        {
            // 清空内存
            memset(nv12, 0, NV12_BUFF_SIZE);
            memset(nv21, 0, NV21_BUFF_SIZE);
            
            fread(nv12, 1, NV12_BUFF_SIZE, fNv12);  // 每次读取一帧 YUV 数据
            
            converter.SY_Nv12ToNv21(nv12, YUV_WIDTH, YUV_HEIGHT, nv21); // 转换成 NV21
            
            fwrite(nv21, NV21_BUFF_SIZE, 1, fNv21);
        }
        endTime = currTime();
        
        fseek(fNv21, 0, SEEK_SET);
        fseek(fNv12, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fNv21);
    fclose(fNv12);
    free(nv21);
    free(nv12);
    nv21   = NULL;
    nv12 = NULL;
}

#pragma mark -- NV21 转 I420
void testNv21ToI420()
{
    SYYuvConverter converter;
    
    unsigned char *i420 = (unsigned char *)malloc(I420_BUFF_SIZE);
    unsigned char *nv21 = (unsigned char *)malloc(NV21_BUFF_SIZE);
    if (NULL == i420 || NULL == nv21)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fI420   = fopen("XinWenLianBo_480x360_I420.yuv", "wb+");  // 打开 I420 文件
    FILE *fNv21 = fopen("XinWenLianBo_480x360_NV21.yuv", "rb+");  // 打开 NV21 文件
    
    if (NULL == fI420 || NULL == fNv21)
    {
        printf("Open file failure!\n");
        free(i420);
        free(nv21);
        i420 = NULL;
        nv21 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fNv21))
        {
            // 清空内存
            memset(i420, 0, I420_BUFF_SIZE);
            memset(nv21, 0, NV12_BUFF_SIZE);
            
            fread(nv21, 1, NV21_BUFF_SIZE, fNv21);  // 每次读取一帧 YUV 数据
            
            converter.SY_Nv21ToI420(nv21, YUV_WIDTH, YUV_HEIGHT, i420); // 转换成 I420
            
            fwrite(i420, I420_BUFF_SIZE, 1, fI420);
        }
        endTime = currTime();
        
        fseek(fI420, 0, SEEK_SET);
        fseek(fNv21, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fI420);
    fclose(fNv21);
    free(i420);
    free(nv21);
    i420   = NULL;
    nv21 = NULL;
}

#pragma mark -- NV21 转 NV12
void testNv21ToNv12()
{
    SYYuvConverter converter;
    
    unsigned char *nv12 = (unsigned char *)malloc(NV12_BUFF_SIZE);
    unsigned char *nv21 = (unsigned char *)malloc(NV21_BUFF_SIZE);
    if (NULL == nv12 || NULL == nv21)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    FILE *fNv12 = fopen("XinWenLianBo_480x360_NV12.yuv", "wb+");  // 打开 NV12 文件
    FILE *fNv21 = fopen("XinWenLianBo_480x360_NV21.yuv", "rb+");  // 打开 NV21 文件
    
    
    if (NULL == fNv21 || NULL == fNv12)
    {
        printf("Open file failure!\n");
        free(nv21);
        free(nv12);
        nv21 = NULL;
        nv12 = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fNv21))
        {
            // 清空内存
            memset(nv12, 0, NV12_BUFF_SIZE);
            memset(nv21, 0, NV21_BUFF_SIZE);
            
            fread(nv21, 1, NV21_BUFF_SIZE, fNv21);  // 每次读取一帧 YUV 数据
            
            converter.SY_Nv21ToNv12(nv21, YUV_WIDTH, YUV_HEIGHT, nv12); // 转换成 NV12
            
            fwrite(nv12, NV12_BUFF_SIZE, 1, fNv12);
        }
        endTime = currTime();
        
        fseek(fNv21, 0, SEEK_SET);
        fseek(fNv12, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fNv21);
    fclose(fNv12);
    free(nv21);
    free(nv12);
    nv21   = NULL;
    nv12 = NULL;
}

#pragma mark - 旋转
#pragma mark -- Yuv 旋转
void testRotateYuv()
{
    SYRotate rotate;
    SYYuvToBmp converter;
    
    unsigned char *yuv     = (unsigned char *)malloc(I420_BUFF_SIZE);
    unsigned char *rotateYuv = (unsigned char *)malloc(I420_BUFF_SIZE);
    if (NULL == yuv || NULL == rotateYuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    // I420
    FILE *fyuv = fopen("XinWenLianBo_480x360_I420.yuv", "rb+");  // 打开 YUv 文件
    FILE *frotateYuv = fopen("XinWenLianBo_480x360_I420_rotate.yuv", "wb+");  // 打开 Yuv 文件
    rotate.SY_SetYuvType(SYYuv_i420);
    converter.SY_SetYuvType(SYYuv_i420);
    
//    // NV12
//    FILE *fyuv = fopen("XinWenLianBo_480x360_NV12.yuv", "rb+");  // 打开 YUv 文件
//    FILE *frotateYuv = fopen("XinWenLianBo_480x360_NV12_rotate.yuv", "wb+");  // 打开 Yuv 文件
//    rotate.SY_SetYuvType(SYYuv_nv12);
//    converter.SY_SetYuvType(SYYuv_nv12);
    
//    // NV21
//    FILE *fyuv = fopen("XinWenLianBo_480x360_NV21.yuv", "rb+");  // 打开 YUv 文件
//    FILE *frotateYuv = fopen("XinWenLianBo_480x360_NV21_rotate.yuv", "wb+");  // 打开 Yuv 文件
//    rotate.SY_SetYuvType(SYYuv_nv21);
//    converter.SY_SetYuvType(SYYuv_nv21);
    
    
    if (NULL == fyuv || NULL == frotateYuv)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(rotateYuv);
        yuv = NULL;
        rotateYuv = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    int count           = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fyuv))
        {
            // 清空内存
            memset(yuv, 0, I420_BUFF_SIZE);
            memset(rotateYuv, 0, I420_BUFF_SIZE);
            
            fread(yuv, 1, I420_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
            count++;
            if (342 == count)
            {
                memset(rotateYuv, 0, I420_BUFF_SIZE);
                rotate.SY_RotateYuv(yuv, YUV_WIDTH, YUV_HEIGHT, rotateYuv, SYRotate_clockwise, SYRotate_270);
                fwrite(rotateYuv, I420_BUFF_SIZE, 1, frotateYuv);

                converter.SY_YuvToBmp(rotateYuv, YUV_HEIGHT, YUV_WIDTH, "./BMP/XinWenLianBo_Frame.bmp");
                break;
            }
        }
        endTime = currTime();
        
        fseek(frotateYuv, 0, SEEK_SET);
        fseek(fyuv, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(frotateYuv);
    fclose(fyuv);
    free(rotateYuv);
    free(yuv);
    rotateYuv   = NULL;
    yuv = NULL;
}

#pragma mark - 镜像
#pragma mark -- YUV 镜像
void testMirrorYuv()
{
    SYMirror mirror;
    SYYuvToBmp converter;
    
    unsigned char *yuv       = (unsigned char *)malloc(I420_BUFF_SIZE);
    unsigned char *mirrorYuv = (unsigned char *)malloc(I420_BUFF_SIZE);
    if (NULL == yuv || NULL == mirrorYuv)
    {
        printf("Malloc data buffer failure!\n");
        return;
    }
    // I420
    FILE *fyuv = fopen("XinWenLianBo_480x360_I420.yuv", "rb+");  // 打开 YUv 文件
    FILE *fmirrorYuv = fopen("XinWenLianBo_480x360_I420_mirror.yuv", "wb+");  // 打开 Yuv 文件
    mirror.SY_SetYuvType(SYYuv_i420);
    converter.SY_SetYuvType(SYYuv_i420);
    
//    // NV12
//    FILE *fyuv = fopen("XinWenLianBo_480x360_NV12.yuv", "rb+");  // 打开 YUv 文件
//    FILE *fmirrorYuv = fopen("XinWenLianBo_480x360_NV12_mirror.yuv", "wb+");  // 打开 Yuv 文件
//    mirror.SY_SetYuvType(SYYuv_nv12);
//    converter.SY_SetYuvType(SYYuv_nv12);
    
//    // NV21
//    FILE *fyuv = fopen("XinWenLianBo_480x360_NV21.yuv", "rb+");  // 打开 YUv 文件
//    FILE *fmirrorYuv = fopen("XinWenLianBo_480x360_NV21_mirror.yuv", "wb+");  // 打开 Yuv 文件
//    mirror.SY_SetYuvType(SYYuv_nv21);
//    converter.SY_SetYuvType(SYYuv_nv21);
    
    
    if (NULL == fyuv || NULL == fmirrorYuv)
    {
        printf("Open file failure!\n");
        free(yuv);
        free(mirrorYuv);
        yuv = NULL;
        mirrorYuv = NULL;
        
        return;
    }
    long long sTime     = 0;
    long long eTime     = 0;
    long totalTime      = 0;
    long long startTime = 0;
    long long endTime   = 0;
    long interval       = 0;
    int count           = 0;
    
    sTime = currTime();
    for (int i = 0; i < LOOP_COUNT; i++)
    {
        startTime = currTime();
        while (!feof(fyuv))
        {
            // 清空内存
            memset(yuv, 0, I420_BUFF_SIZE);
            memset(mirrorYuv, 0, I420_BUFF_SIZE);
            
            fread(yuv, 1, I420_BUFF_SIZE, fyuv);  // 每次读取一帧 YUV 数据
            count++;
            if (342 == count)
            {
                memset(mirrorYuv, 0, I420_BUFF_SIZE);
                mirror.SY_MirrorYuv(yuv, YUV_WIDTH, YUV_HEIGHT, mirrorYuv, SYMirror_horizontal);
                fwrite(mirrorYuv, I420_BUFF_SIZE, 1, fmirrorYuv);
                
                converter.SY_YuvToBmp(mirrorYuv, YUV_WIDTH, YUV_HEIGHT, "./BMP/XinWenLianBo_Frame.bmp");
                break;
            }
        }
        endTime = currTime();
        
        fseek(fmirrorYuv, 0, SEEK_SET);
        fseek(fyuv, 0, SEEK_SET);
        
        interval = endTime - startTime;
        fflush(stdout);
        printf("%ld\n", interval);
    }
    eTime = currTime();
    totalTime = eTime - sTime;
    printf("Total time : %ld\n", totalTime);
    
    fclose(fmirrorYuv);
    fclose(fyuv);
    free(mirrorYuv);
    free(yuv);
    mirrorYuv   = NULL;
    yuv = NULL;
}



int main(int argc, const char * argv[])
{
//    testI420ToRgb565();
    
//    testNv12ToRgb565();
    
//    testNv21ToRgb565();
    
//    testI420ToRgb24();
    
//    testNv12ToRgb24();
    
//    testNv21ToRgb24();
    
//    testRgb565ToI420();
    
//    testRgb565ToNv12();
    
//    testRgb565ToNv21();
    
//    testRgb24ToI420();
    
//    testRgb24ToNv12();
    
//    testRgb24ToNv21();
    
//    testYuvToBmp();
    
//    testRgbToBmp();
    
//    testClipYuv();
    
//    testClipRgb();
    
//    testRotateYuv();
    
//    testMirrorYuv();
    
//    testI420ToNv12();
    
//    testI420ToNv21();

//    testNv12ToI420();

//    testNv12ToNv21();

//    testNv21ToI420();

    testNv21ToNv12();

    return 0;
}
