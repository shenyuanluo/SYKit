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


/// 旋转 NV12 图像
kernel
void NV12Rotate(device const unsigned char* inYUV,                                      // 原始图像
                device unsigned char*       outYUV,                                     // 结果图像
                constant int&               width     [[ buffer(11) ]],                 // 图像-宽度（像素）
                constant int&               height    [[ buffer(12) ]],                 // 图像-高度（像素）
                constant int&               direction [[ buffer(13) ]],                 // 旋转-方向
                constant int&               angle     [[ buffer(14) ]],                 // 旋转-角度
                uint                        index     [[ thread_position_in_grid ]])    // 当前计算线程下标（在网格中）
{
    int row = index / width;        // 当前「行」
    int col = index - row * width;  // 当前「列」
    if (row < 0 || row >= height || col < 0 || col >= width)    // 下标越(网格)界处理
    {
        return;
    }
    int uvOffset = width * height;                              //  UV 平面-偏移
    int uvHeight = (height>>1);                                 //  UV 平面-高度
    int uvWidth  = width;                                       //  UV 平面-宽度
    int srcYPos  = row * width  + col;                          // 原始 Y 下标
    int dstYPos  = srcYPos;                                     // 目的 Y 下标
    int srcUVRow = (row>>1);                                    // 原始 UV 行-下标
    int srcUVCol = col;                                         // 原始 UV 列-下标
    int srcUVPos = uvOffset + srcUVRow * uvWidth + srcUVCol;    // 原始 UV 下标
    int dstUVRow = srcUVRow;                                    // 目的 UV 行-下标
    int dstUVCol = srcUVCol;                                    // 目的 UV 行-下标
    int dstUVPos = srcUVPos;                                    // 目的 UV 行-下标
    
    if ((SYRotate_clockwise == direction && SYRotate_90 == angle)
        || (SYRotate_counterClockwise == direction && SYRotate_270 == angle))       // 右转 90°
    {
        dstYPos = col * height + (height - 1 - row);
        if ((0 == (row&1))&& (0 == (col&1)))
        {
            dstUVRow = col;
            dstUVCol = ((uvHeight - 1 - srcUVRow)<<1);
            dstUVPos = uvOffset + dstUVRow * uvHeight + dstUVCol;
        }
    }
    else if ((SYRotate_clockwise == direction && SYRotate_180 == angle)
             || (SYRotate_counterClockwise == direction && SYRotate_180 == angle))  // 右转 180°
    {
        dstYPos = (height - 1 - row) * width + (width - 1 - col);
        if ((0 == (row&1)) && (0 == (col&1)))
        {
            dstUVRow = uvHeight - 1 - srcUVRow;
            dstUVCol = width - col;
            dstUVPos = uvOffset + dstUVRow * width + dstUVCol;
        }
    }
    else if ((SYRotate_clockwise == direction && SYRotate_270 == angle)
             || (SYRotate_counterClockwise == direction && SYRotate_90 == angle))   // 右转 270°
    {
        dstYPos = (width - 1 - col) * height + row;
        if ((0 == (row&1))&& (0 == (col&1)))
        {
            uvWidth          = (width>>1);
            
            srcUVCol         = (col>>1);
            dstUVRow         = ((uvWidth - 1 - srcUVCol)<<1);
            srcUVPos         = uvOffset + srcUVRow * width    + col;
            dstUVPos         = uvOffset + dstUVRow * uvHeight + row;
        }
    }
    
    outYUV[dstYPos] = inYUV[srcYPos];   // 旋转 Y
    // YUV-420：4 个  Y 数据共用一对 UV 数据
    if ((0 == (row&1))&& (0 == (col&1)))
    {
        outYUV[dstUVPos]     = inYUV[srcUVPos];     // 旋转 U
        outYUV[dstUVPos + 1] = inYUV[srcUVPos + 1]; // 旋转 V
    }
    
}

