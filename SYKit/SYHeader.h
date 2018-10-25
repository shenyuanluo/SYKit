//
//  SYHeader.h
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/7/16.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#ifndef SYHeader_h
#define SYHeader_h


#define SYKIT_API


/*
 数据格式类型:（注意大小端）
 
 I420 内存布局
                   W
         +--------------------+
         |Y0Y1Y2Y3...         |
         |...                 |
         |                    |   H
         |                    |
         |                    |
         |                    |
         +--------------------+
         |U0U1...   |
         |...       |   H/2
         |          |
         +----------+
         |V0V1...   |
         |...       |  H/2
         |          |
         +----------+
             w/2
 
 YV12 内存布局
                   W
         +--------------------+
         |Y0Y1Y2Y3...         |
         |...                 |
         |                    |   H
         |                    |
         |                    |
         |                    |
         +--------------------+
         |V0V1...   |
         |...       |   H/2
         |          |
         +----------+
         |U0U1...   |
         |...       |  H/2
         |          |
         +----------+
             w/2
 
 
 NV12 内存布局
                   W
         +--------------------+
         |Y0Y1Y2Y3...         |
         |...                 |
         |                    |   H
         |                    |
         |                    |
         |                    |
         +--------------------+
         |U0V0U1V1...         |
         |...                 |   H/2
         |                    |
         +--------------------+
                   W
 
 
 NV21 内存布局
                   W
         +--------------------+
         |Y0Y1Y2Y3...         |
         |...                 |
         |                    |   H
         |                    |
         |                    |
         |                    |
         +--------------------+
         |V0U0V1U1...         |
         |...                 |   H/2
         |                    |
         +--------------------+
                   W
 
 
 RGB555 内存布局（最高位不用）
                      R                   G                   B
        +---------------------------------------------------------------+
  高字节 | x | R | R | R | R | R | G | G | G | G | G | B | B | B | B | B |  低字节
        +---------------------------------------------------------------+
          0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
 
 
 RGB565 内存布局
                   R                     G                     B
         +---------------------------------------------------------------+
   高字节 | R | R | R | R | R | G | G | G | G | G | G | B | B | B | B | B |  低字节
         +---------------------------------------------------------------+
           0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
 
 
 RGB24 内存布局
                        B                               G                               R
        +-----------------------------------------------------------------------------------------------+
  高字节 | B | B | B | B | B | B | B | B | G | G | G | G | G | G | G | G | R | R | R | R | R | R | R | R |   低字节
        +-----------------------------------------------------------------------------------------------+
          0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23
 
 RGB32 内存布局（最后 1 个字节保留）
                        B                               G                               R                           Reserved
        +-------------------------------------------------------------------------------------------------------------------------------+
   高字节| B | B | B | B | B | B | B | B | G | G | G | G | G | G | G | G | R | R | R | R | R | R | R | R |   |   |   |   |   |   |   |   |  低字节
        +-------------------------------------------------------------------------------------------------------------------------------+
          0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
 
 */
typedef enum __syPixFmtType {
    SYPixFmt_unknow         = -1,   // 未知
    SYPixFmt_i420           = 0,    // I420
    SYPixFmt_yv12           = 1,    // YV12
    SYPixFmt_nv12           = 2,    // NV12
    SYPixFmt_nv21           = 3,    // NV21
    SYPiXFmt_rgb555         = 4,    // RGB 555
    SYPiXFmt_rgb565         = 5,    // RGB 565
    SYPiXFmt_rgb24          = 6,    // RGB 24
    SYPiXFmt_rgb32          = 7,    // RGB 32
}SYPixFmtType;

/* YUV 数据格式类型 */
typedef enum __syYuvType {
    SYYuv_unknow            = SYPixFmt_unknow,
    SYYuv_i420              = SYPixFmt_i420,
    SYYuv_yv12              = SYPixFmt_yv12,
    SYYuv_nv12              = SYPixFmt_nv12,
    SYYuv_nv21              = SYPixFmt_nv21,
}SYYuvType;

/* RGB 数据格式类型 */
typedef enum __syRgbType {
    SYRgb_unknow            = SYPixFmt_unknow,
    SYRgb_rgb555            = SYPiXFmt_rgb555,
    SYRgb_rgb565            = SYPiXFmt_rgb565,
    SYRgb_rgb24             = SYPiXFmt_rgb24,
    SYRgb_rgb32             = SYPiXFmt_rgb32,   
}SYRgbType;

/* BMP 颜色深度（每个象素所需要的位数）类型 */
typedef enum __syBmpBitCount {
    SYBmpBitCount_15        = 0,    // RGB555
    SYBmpBitCount_16        = 1,    // RGB565
    SYBmpBitCount_24        = 2,    // RGB24
    SYBmpBitCount_32        = 3,    // RGB32
}SYBmpBitCount;

/* (帧画面）旋转度数 */
typedef enum __syRotateDegree {
    SYRotate_90             = 0,    // 旋转 90°
    SYRotate_180            = 1,    // 旋转 180°
    SYRotate_270            = 2,    // 旋转 270°
    SYRotate_360            = 3,    // 旋转 360°
}SYRotateDegree;

/*
 （帧画面）旋转方向类型
 SYRotate_clockwise：顺时针旋转画面
                                                11  6   1
        1   2   3   4    5                      12  7   2
        6   7   8   9   10      -------->       13  8   3
        11  12  13  14  15                      14  9   4
                                                15 10   5
 
 SYRotate_antiClockwise：逆时针旋转画面
                                                5   10  15
        1   2   3   4    5                      4   9   14
        6   7   8   9   10      -------->       3   8   13
        11  12  13  14  15                      2   7   12
                                                1   6   11
 */
typedef enum __syRotateDirection {
    SYRotate_clockwise      = 0,    // 顺时针
    SYRotate_antiClockwise  = 1,    // 逆时针
}SYRotateDirection;

/*
 （帧画面）镜像方向类型
 SYMirror_horizontal：水平镜像
 
        1   2   3   4   5                       5   4   3   2   1
        6   7   8   9   10      -------->       10  9   8   7   6
        11  12  13  14  15                      15  14  13  12  11
 
 
 SYMirror_vertical：垂直镜像
 
        1   2   3   4   5                       11  12  13  14  15
        6   7   8   9   10      -------->       6   7   8   9   10
        11  12  13  14  15                      1   2   3   4   5
 */
typedef enum __syMirrorDirection {
    SYMirror_horizontal     = 0,    // 水平镜像
    SYMirror_vertical       = 1,    // 垂直镜像
}SYMirrorDirection;


/* 错误码类型 */
typedef enum __syErrType {
    SYErr_success           = 0,    // 成功
    SYErr_failure           = -1,   // 失败
    SYErr_paramError        = -2,   // 参数出错
    SYErr_fileFmtError      = -3,   // 文件格式错误
}SYErrType;


/* 矩形区域结构体 */
typedef struct __syRect {
    unsigned int tlX;           // 左上角 X 坐标（Top-Left）
    unsigned int tlY;           // 左上角 Y 坐标（Top-Left）
    unsigned int brX;           // 右下角 X 坐标（Bottom-Right）
    unsigned int brY;           // 右下角 Y 坐标（Bottom-Right）
}SYRect;


#pragma mark - SYConverter Start
// 夹紧函数定义：限制在两者范围之间
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

/* 转换矩阵类型 */
typedef enum __matrixType {
    SYMatrix_normal         = 0,    // 常规转换标准
    SYMatrix_bt_601         = 1,    // 标清电视标准：BT.601
    SYMatrix_bt_709         = 2,    // 高清电视标准：BT.709
}SYMatrixType;

/* 转换方法类型 */
typedef enum __convertType {
    SYConvert_normal        = 0,    // 常规方法：浮点运算，精度高
    SYConvert_bitMult       = 1,    // 通过位移来避免浮点运算，精度低
    SYConvert_bitAdd        = 2,    // 通过位移来避免乘法运算，精度低
    SYConvert_table         = 3,    // 查表法（也是位移计算），精度低
}SYConvertType;

/* 大小端类型 */
typedef enum __endianType {
    SYEndian_little         = 0,    // 小端
    SYEndian_big            = 1,    // 大端
}SYEndianType;

/* RGB 颜色结构体 */
typedef struct __rgb {
    unsigned char R;    // R 分量
    unsigned char G;    // G 分量
    unsigned char B;    // B 分量
}SYRGB;

/* YUV 颜色结构体 */
typedef struct __yuv {
    unsigned char Y;    // Y 分量
    unsigned char U;    // U 分量
    unsigned char V;    // V 分量
}SYYUV;


/* BMP 头结构体：14 字节（注意内存对齐问题） */
typedef struct __bmpHeader
{
    /* 注意，在这个 BMP 头结构体里，bfType 是必需的，
     但是不能通过结构体属性实现，必须现在文件写入，
     因为直接在结构体这里，会引发内存对齐问题，导致 BMP 图像无法正常显示
     （或者通过编译指令：#pragma pack(n)，n = 1,2,4,8,16 来改变内存对齐系数）*/
//    unsigned char   bfType[2];          // 2 byte -- BMP 文件类型标识（必须为 'BM' 即：0x424D ）
    unsigned int    bfSize;             // 4 byte -- BMP 文件大小（单位：字节）
    unsigned short  bfReserverd1;       // 2 byte -- BMP 文件保留字（必须为 0）
    unsigned short  bfReserverd2;       // 2 byte -- BMP 文件保留字（必须为 0）
    unsigned int    bfOffBits;          // 4 byte -- BMP 文件头到数据的偏移量（单位：字节）
}SYBitMapHeader;

/* BMP 文件信息头结构体：40 字节 */
typedef struct __bmpFileInfo
{
    unsigned int    biSize;             // 4 byte -- 该结构大小（单位：字节；通常为 40 字节，即 0x28）
    unsigned int    biWidth;            // 4 byte -- 图像帧宽度（单位：像素）
    unsigned int    biHeight;           // 4 byte -- 图像帧高度（单位：像素）
    unsigned short  biPlanes;           // 2 byte -- 色彩平面(color plane)数量（必须为 1）
    unsigned short  biBitCount;         // 2 byte -- 颜色深度（每个象素所需要的位数）
    unsigned int    biCompression;      // 4 byte -- BMP 的压缩类型（通常不压缩，即 BI_RGB，对应值为 0）
    unsigned int    biSizeImage;        // 4 byte -- BMP 的大小（单位：字节）
    unsigned int    biXPelsPermeter;    // 4 byte -- BMP 水平分辨率（像素/米）
    unsigned int    biYPelsPermeter;    // 4 byte -- BMP 垂直分辨率（像素/米）
    unsigned int    biClrUsed;          // 4 byte -- BMP 实际使用的颜色表中的颜色数
    unsigned int    biClrImportant;     // 4 byte -- BMP 显示过程中重要的颜色数
}SYBitMapFileInfo;

/* BMP 文件结构体 */
typedef struct __bitMap
{
    SYBitMapHeader      bfHeader;
    SYBitMapFileInfo    bfInfo;
    unsigned char*      bfData;
}SYBitMap;


#pragma mark - SYConverter End

#endif /* SYHeader_h */
