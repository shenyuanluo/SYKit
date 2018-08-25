//
//  SYRgbToBmp.h
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/7/19.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#ifndef SYRgbToBmp_h
#define SYRgbToBmp_h

#include <iostream>
#include "SYHeader.h"


class SYRgbToBmp
{
private:
    unsigned int m_bmpHeaderLen;    // BMP 头长度
    unsigned int m_bmpFileInfoLen;  // BMP 文件信息长度
    
    SYEndianType   m_eType;           // 大小端类型
    
    /**
     检查大小端
     
     @return 大小端类型
     */
    SYEndianType checkEndian();
    
public:
    SYRgbToBmp();
    
    ~SYRgbToBmp();
    
    
    /**
     RGB565 转 BMP
     
     @param inRgb RGB565 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outBmpPath BMP 文件路径（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Rgb565ToBmp(unsigned char *inRgb, unsigned int width, unsigned int height, const char *outBmpPath) const;
    
    /**
     RGB24 转 BMP
     
     @param inRgb RGB24 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outBmpPath BMP 文件路径（输出）
     @return 转换是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_Rgb24ToBmp(unsigned char *inRgb, unsigned int width, unsigned int height, const char *outBmpPath) const;
};

#endif /* SYRgbToBmp_h */
