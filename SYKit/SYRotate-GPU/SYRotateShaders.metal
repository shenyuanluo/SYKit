//
//  SYRotateShaders.metal
//  SYKitExample-GPU
//
//  Created by ShenYuanLuo on 2023/4/10.
//  Copyright © 2023年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include <metal_stdlib>
#include "SYRotateHeader.h"
using namespace metal;


/// 旋转 YUV-nv12 图像
kernel
void NV12Rotate(device const unsigned char* inYUV,                                    // 原始图像
                device unsigned char*       outYUV,                                   // 结果图像
                constant int&               width     [[ buffer(11) ]],               // 图像-宽度（像素）
                constant int&               height    [[ buffer(12) ]],               // 图像-高度（像素）
                constant int&               direction [[ buffer(13) ]],               // 是否向右旋转（0：向左，1：向右；旋转 90度）
                uint                        index     [[ thread_position_in_grid ]])  // 当前计算线程下标（在网格中）
{
    switch (direction)
    {
        case SYRotate_clockwise:    // 顺时针
        {
            int row = index / width;        // 当前「行」
            int col = index - row * width;  // 当前「列」
            if (row < 0 || row >= height || col < 0 || col >= width)    // 下标越(网格)界处理
            {
                return;
            }
            int srcYPos     = row * width  + col;                   // 原始「Y」下标
            int dstYPos     = col * height + (height - 1 - row);    // 目的「Y」下标
            outYUV[dstYPos] = inYUV[srcYPos];                       // 旋转「Y」
            
            if ((0 == (row&1))&& (0 == (col&1)))    // YUV-420：4 个 「Y」数据共用一对「UV」数据
            {
                int uvOffset         = width * height;  // 「UV」平面偏移
                int uvHeight         = (height>>1);     // 「UV」平面-高度
                int srcUVRow         = (row>>1);        // 原始「UV」行-下标
                int dstUVCol         = ((uvHeight - 1 - srcUVRow)<<1);         // 目的「UV」列-下标
                int srcUVPos         = uvOffset + srcUVRow * width + col;      // 原始「UV」下标
                int dstUVPos         = uvOffset + col * uvHeight   + dstUVCol; // 目的「UV」下标
                outYUV[dstUVPos]     = inYUV[srcUVPos];                        // 旋转「U」
                outYUV[dstUVPos + 1] = inYUV[srcUVPos + 1];                    // 旋转「V」
            }
        }
            break;
            
        case SYRotate_counterClockwise:    // 逆时针
        {
            int row = index / width;        // 当前「行」
            int col = index - row * width;  // 当前「列」
            if (row < 0 || row >= height || col < 0 || col >= width)    // 下标越(网格)界处理
            {
                return;
            }
            int srcYPos     = row * width  + col;                   // 原始「Y」下标
            int dstYPos     = (width - 1 - col) * height + row;     // 目的「Y」下标
            outYUV[dstYPos] = inYUV[srcYPos];                       // 旋转「Y」
            
            if ((0 == (row&1))&& (0 == (col&1)))    // YUV-420：4 个 「Y」数据共用一对「UV」数据
            {
                int uvOffset         = width * height;  // 「UV」平面偏移
                int uvHeight         = (height>>1);     // 「UV」平面-高度
                int uvWidth          = (width>>1);     // UV 数据宽度
                int srcUVRow         = (row>>1);        // 原始「UV」行-下标
                int srcUVCol         = (col>>1);        // 原始「UV」列-下标
                int dstUVRow         = ((uvWidth - 1 - srcUVCol)<<1);           // 目的「UV」行-下标
                int srcUVPos         = uvOffset + srcUVRow * width    + col;    // 原始「UV」下标
                int dstUVPos         = uvOffset + dstUVRow * uvHeight + row;    // 目的「UV」下标
                outYUV[dstUVPos]     = inYUV[srcUVPos];                         // 旋转「U」
                outYUV[dstUVPos + 1] = inYUV[srcUVPos + 1];                     // 旋转「V」
            }
        }
            break;
    }
    
}
