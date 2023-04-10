//
//  SYYuvToRgbShaders.metal
//  SYKitExample-GPU
//
//  Created by ShenYuanLuo on 2023/4/10.
//  Copyright © 2023年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include <metal_stdlib>
using namespace metal;


/// NV12 转 RGB24
kernel
void NV12ToRGB24(device const unsigned char* inYUV,                                    // YUV-缓存
                 device unsigned char*       outRGB,                                   // RGB-缓存
                 constant int&               width     [[ buffer(11) ]],               // YUV图像-宽度（像素）
                 constant int&               height    [[ buffer(12) ]],               // YUV图像-高度（像素）
                 uint                        index     [[ thread_position_in_grid ]])  // 当前计算线程下标（在网格中）
{
    int row = index / width;        // 当前「行」
    int col = index - row * width;  // 当前「列」
    if (row < 0 || row >= height || col < 0 || col >= width)    // 下标越(网格)界处理
    {
        return;
    }
    
    int ySize = width * height;                             
    int yPos  = row * width + col;
    int uPos  = ySize + (row>>1) * width + (col - col%2);
    int vPos  = uPos + 1;
    
    int rPos = (row * width + col) * 3;
    int gPos = rPos + 1;
    int bPos = rPos + 2;
    
    int y = (int)inYUV[yPos];           // 当前像素点 Y-值
    int u = (int)(inYUV[uPos] - 128);   // 当前像素点 U-值
    int v = (int)(inYUV[vPos] - 128);   // 当前像素点 V-值
    
    int r = (y               + 1.13983 * v);    // 当前像素点 R-值
    int g = (y - 0.39465 * u - 0.5806  * v);    // 当前像素点 G-值
    int b = (y + 2.03211 * u);                  // 当前像素点 B-值
    
    r = max(0, min(r, 255));    // 限制 R 取值范围
    g = max(0, min(g, 255));    // 限制 G 取值范围
    b = max(0, min(b, 255));    // 限制 B 取值范围
    
    outRGB[rPos] = (char)r;
    outRGB[gPos] = (char)g;
    outRGB[bPos] = (char)b;
}
