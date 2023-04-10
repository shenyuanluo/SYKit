//
//  SYRotateHeader.h
//  SYKitExample-GPU
//
//  Created by ShenYuanLuo on 2023/4/10.
//  Copyright © 2023年 http://blog.shenyuanluo.com/ All rights reserved.
//

#ifndef SYRotateHeader_h
#define SYRotateHeader_h

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
typedef enum {
    SYRotate_clockwise          = 0,    // 顺时针
    SYRotate_counterClockwise   = 1,    // 逆时针
}SYRotateDirection;

#endif /* SYRotateHeader_h */
