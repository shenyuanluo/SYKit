//
//  SYRotate.h
//  SYKit <https://github.com/shenyuanluo/SYKit>
//
//  Created by ShenYuanLuo on 2018/10/23.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//


/*
 YUV 旋转
 */

#ifndef SYRotate_h
#define SYRotate_h

#include <iostream>
#include "SYHeader.h"


class SYRotate
{
private:
    SYYuvType m_yuvType;          // 旋转 YUV 数据（格式）类型
    
    /**
     初始化默认参数
     */
    void initParam();
    
    /**
     I420 旋转
     
     @param inYuv I420 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv 旋转后 I420 数据（输出）
     @param direction 旋转方向，参见‘SYRotateDirection’
     @param degree 旋转角度，参见‘SYRotateDegree’
     @return 旋转是否成功，参见‘SYErrType’
     */
    int i420Rotate(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYRotateDirection direction, SYRotateDegree degree) const;
    
    /**
     NV12 旋转
     
     @param inYuv NV12 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv 旋转后 NV12 数据（输出）
     @param direction 旋转方向，参见‘SYRotateDirection’
     @param degree 旋转角度，参见‘SYRotateDegree’
     @return 旋转是否成功，参见‘SYErrType’
     */
    int nv12Rotate(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYRotateDirection direction, SYRotateDegree degree) const;
    
    /**
     NV21 旋转
     
     @param inYuv NV21 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv 旋转后 NV21 数据（输出）
     @param direction 旋转方向，参见‘SYRotateDirection’
     @param degree 旋转角度，参见‘SYRotateDegree’
     @return 旋转是否成功，参见‘SYErrType’
     */
    int nv21Rotate(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYRotateDirection direction, SYRotateDegree degree) const;

public:
    SYRotate();
    
    ~SYRotate();
    
    /**
     设置旋转 YUV 数据格式类型（默认：I420）
     
     @param yuvType yuv 数据格式类型
     */
    SYKIT_API void SY_SetYuvType(SYYuvType yuvType);
    
    /**
     YUV 旋转
     
     @param inYuv YUV 数据（输入）
     @param width 帧-宽度
     @param height 帧-高度
     @param outYuv 旋转后 YUV 数据（输出）
     @param direction 旋转方向，参见‘SYRotateDirection’
     @param degree 旋转角度，参见‘SYRotateDegree’
     @return 旋转是否成功，参见‘SYErrType’
     */
    SYKIT_API int SY_RotateYuv(unsigned char* inYuv, unsigned int width, unsigned int height, unsigned char* outYuv, SYRotateDirection direction, SYRotateDegree degree) const;
    
};

#endif /* SYRotate_h */
