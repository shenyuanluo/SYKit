//
//  main.swift
//  SYKitExample-GPU
//
//  Created by ShenYuanLuo on 2023/4/10.
//  Copyright © 2023年 http://blog.shenyuanluo.com/ All rights reserved.
//

import Foundation

let rotater = SYRotateOnGPU()
let yuv2rgb = SYYuvToRgbOnGPU()

/// 测试旋转 NV12 图像
private func testRotateNV12() {
    print("testRotateNV12...")
    let fn  = "XinWenLianBo_480x360_NV12"
    let ifp = fn + ".yuv"
    let ofp = fn + "rotate.yuv"
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
    let mutex = DispatchSemaphore(value: 1)
    while ips.streamStatus != .atEnd {
        mutex.wait()
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
                }
            }
            mutex.signal()
        }
    }
    ips.close()
    
    // 这里只是为了避免执行完成 crash，因为 readData 比 conver&writeData 块，最后一帧还没写完
    mutex.signal()
    print("旋转 NV12 完成: \(ofp)")
}


/// 测试 NV12 转 RGB24
private func testNV12ToRGB24() {
    print("testNV12ToRGB24...")
    let fn  = "XinWenLianBo_480x360_NV12"
    let ifp = fn + ".yuv"
    let ofp = fn + "_gpu.rgb"
    guard let ips = InputStream(fileAtPath: ifp),
          let ops = OutputStream(toFileAtPath: ofp, append: false) else {
        fatalError("Can not find NV12 file path")
    }
    var srcCnt    = 0
    var dstCnt    = 0
    let width     = 480
    let height    = 360
    let yuvSize   = Int(Float(width * height) * 1.5)
    let rgbSize   = width * height * 3
    let inYUV     = UnsafeMutableRawPointer.allocate(byteCount: yuvSize, alignment: 1)
    inYUV.initializeMemory(as: UInt8.self, repeating: 0, count: yuvSize)
    ips.open()
    ops.open()
    let mutex = DispatchSemaphore(value: 1)
    while ips.streamStatus != .atEnd {
        mutex.wait()
        let rSize = ips.read(inYUV, maxLength: yuvSize)
        if rSize != yuvSize {
            continue
        }
        srcCnt += 1
        let start  = Date()
        yuv2rgb.nv12ToRGB24(inYuv: inYUV, width: width, height: height) { outRGB in
            if let outRGB = outRGB {
                let wSize = ops.write(outRGB, maxLength: rgbSize)
                dstCnt   += 1
                print("转换并写入: \(wSize)，耗时: \(Date().timeIntervalSince1970 - start.timeIntervalSince1970)s")
                if dstCnt == srcCnt, ips.streamStatus == .atEnd {
                    inYUV.deallocate()
                    ops.close()
                }
            }
            mutex.signal()
        }
    }
    ips.close()
    
    // 这里只是为了避免执行完成 crash，因为 readData 比 conver&writeData 块，最后一帧还没写完
    mutex.signal()
    print("NV12 ——> RGB24 转换完成: \(ofp)")
}


testRotateNV12()

testNV12ToRGB24()

