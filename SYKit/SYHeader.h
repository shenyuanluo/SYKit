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


/* 数据格式类型 */
typedef enum __syPixFmtType {
    SYPixFmt_unknow         = -1,   // 未知
    SYPixFmt_i420           = 0,    // I420
    SYPixFmt_nv12           = 1,    // NV12
    SYPixFmt_nv21           = 2,    // NV21
}SYPixFmtType;


/* 错误码类型 */
typedef enum __syErrType {
    SYErr_success           = 0,    // 成功
    SYErr_failure           = -1,   // 失败
    SYErr_paramError        = -2,   // 参数出错
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
//    unsigned short  bfType;             // 2 byte -- BMP 文件类型标识（必须为 'BM' 即：0x4D42）
    unsigned int    bfSize;             // 4 byte -- BMP 文件大小（单位：字节）
    unsigned short  bfReserverd1;       // 2 byte -- BMP 文件保留字（必须为 0）
    unsigned short  bfReserverd2;       // 2 byte -- BMP 文件保留字（必须为 0）
    unsigned int    bfbfOffBits;        // 4 byte -- BMP 文件头到数据的偏移量（单位：字节）
}SYBitMapHeader;

/* BMP 文件信息头结构体：40 字节 */
typedef struct __bmpFileInfo
{
    unsigned int    biSize;             // 4 byte -- 该结构大小（单位：字节）
    unsigned int    biWidth;            // 4 byte --图像帧宽度（单位：像素）
    unsigned int    biHeight;           // 4 byte --图像帧高度（单位：像素）
    unsigned short  biPlanes;           // 2 byte --目标设备的级别（必须为 1）
    unsigned short  biBitcount;         // 2 byte --颜色深度（每个象素所需要的位数）
    unsigned int    biCompression;      // 4 byte --BMP 的压缩类型
    unsigned int    biSizeImage;        // 4 byte --BMP 的大小（单位：字节）
    unsigned int    biXPelsPermeter;    // 4 byte --BMP 水平分辨率，每米像素数
    unsigned int    biYPelsPermeter;    // 4 byte --BMP 垂直分辨率，每米像素数
    unsigned int    biClrUsed;          // 4 byte --BMP 实际使用的颜色表中的颜色数
    unsigned int    biClrImportant;     // 4 byte --BMP 显示过程中重要的颜色数
}SYBitMapFileInfo;
#pragma mark - SYConverter End

#endif /* SYHeader_h */
