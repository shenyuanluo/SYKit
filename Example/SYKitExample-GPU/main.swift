//
//  main.swift
//  SYKitExample-GPU
//
//  Created by ShenYuanLuo on 2023/4/10.
//  Copyright © 2023 shenyuanluo. All rights reserved.
//

import Foundation

let rotater = SYRotateOnGPU()

/// 测试旋转 NV12 图像
private func testRotateNV12() {
    let fn  = "XinWenLianBo_480x360_NV12"
    let ifp = fn + ".yuv"
    let ofp = fn + "rotate_left.yuv"
    guard let ips = InputStream(fileAtPath: ifp),
          let ops = OutputStream(toFileAtPath: ofp, append: false)else {
        fatalError("Can not find NV12 file path")
    }
    var srcCnt    = 0
    var dstCnt    = 0
    let direction = SYRotate_counterClockwise
    let width     = 480
    let height    = 360
    let buffLen   = Int(Float(width * height) * 1.5)
    let pointer   = UnsafeMutableRawPointer.allocate(byteCount: buffLen, alignment: 1)
    pointer.initializeMemory(as: UInt8.self, repeating: 0, count: buffLen)
    ips.open()
    ops.open()
    while ips.streamStatus != .atEnd {
        let rSize = ips.read(pointer, maxLength: buffLen)
        if rSize != buffLen {
            continue
        }
        srcCnt += 1
        let start  = Date()
        rotater.nv12Rotate(inYuv: pointer,
                                width: width,
                                height: height,
                                direction: direction) { outYuv in
            if let outYuv = outYuv {
                let wSize = ops.write(outYuv, maxLength: buffLen)
                dstCnt   += 1
                
                print("旋转并写入: \(wSize)，耗时: \(Date().timeIntervalSince1970 - start.timeIntervalSince1970)s")
                if dstCnt == srcCnt, ips.streamStatus == .closed {
                    pointer.deallocate()
                    ops.close()
                    print("旋转 NV12 完成: \(ofp)")
                }
            }
        }
    }
    ips.close()
}


testRotateNV12()

