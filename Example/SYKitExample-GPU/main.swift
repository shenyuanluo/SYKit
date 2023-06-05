//
//  main.swift
//  SYKitExample-GPU
//
//  Created by ShenYuanLuo on 2023/4/10.
//  Copyright © 2023年 http://blog.shenyuanluo.com/ All rights reserved.
//

import Foundation

let YUV_WIDTH           = 1920  // 视频帧宽
let YUV_HEIGHT          = 1440  // 视频帧高

let I420_BUFF_SIZE      = Int(Float(YUV_WIDTH * YUV_HEIGHT) * 1.5)  // I420 缓冲大小
let YV12_BUFF_SIZE      = Int(Float(YUV_WIDTH * YUV_HEIGHT) * 1.5)  // YV12 缓冲大小
let NV12_BUFF_SIZE      = Int(Float(YUV_WIDTH * YUV_HEIGHT) * 1.5)  // NV12 缓冲大小
let NV21_BUFF_SIZE      = Int(Float(YUV_WIDTH * YUV_HEIGHT) * 1.5)  // NV21 缓冲大小

let RGB_565_BUFF_SIZE   = Int(Float(YUV_WIDTH * YUV_HEIGHT) * 2)    // RGB565 缓冲大小
let RGB_24_BUFF_SIZE    = Int(Float(YUV_WIDTH * YUV_HEIGHT) * 3)    // RGB24 缓冲大小
 


let rotater = SYRotateOnGPU()
let yuv2rgb = SYYuvToRgbOnGPU()

/// 测试旋转 NV12 图像
private func testRotateYUV(type: SYYuvType,
                           fileName: String,
                           direction: SYRotateDirection,
                           angle: SYRotateDegree) {
    var bufferSize = I420_BUFF_SIZE
    switch type {
    case SYYuv_i420:
        print("test Rotate i420 ...")
        bufferSize = I420_BUFF_SIZE
    case SYYuv_yv12:
        print("test Rotate yv12 ...")
        bufferSize = YV12_BUFF_SIZE
    case SYYuv_nv12:
        print("test Rotate nv12 ...")
        bufferSize = NV12_BUFF_SIZE
    case SYYuv_nv21:
        print("test Rotate nv21 ...")
        bufferSize = NV21_BUFF_SIZE
    default:
        return
    }
    
    let inputFilePath = fileName + ".yuv"
    var oputFilePath  = fileName + "_gpu"
    
    switch direction {
    case SYRotate_clockwise:
        oputFilePath += "_right"
    case SYRotate_antiClockwise:
        oputFilePath += "_left"
    default:
        return
    }
    
    switch angle {
    case SYRotate_90:
        oputFilePath += "_90.yuv"
    case SYRotate_180:
        oputFilePath += "_180.yuv"
    case SYRotate_270:
        oputFilePath += "_270.yuv"
    case SYRotate_360:
        oputFilePath += "_360.yuv"
    default:
        return
    }
    guard let inputStream = InputStream(fileAtPath: inputFilePath) else {
        fatalError("Can not find input yuv file")
    }
    guard let outputStream = OutputStream(toFileAtPath: oputFilePath, append: false) else {
        fatalError("Can not find output yuv file")
    }
    var srcCnt    = 0
    var dstCnt    = 0
    let pointer   = UnsafeMutableRawPointer.allocate(byteCount: bufferSize, alignment: 1)
    pointer.initializeMemory(as: UInt8.self, repeating: 0, count: bufferSize)
    inputStream.open()
    outputStream.open()
    let mutex = DispatchSemaphore(value: 1)
    let start = Date()
    while inputStream.streamStatus != .atEnd {
        mutex.wait()
        let rSize = inputStream.read(pointer, maxLength: bufferSize)
        if rSize != NV12_BUFF_SIZE {
            continue
        }
        srcCnt += 1
        let preTime  = Date()
        rotater.rotateYUV(data: pointer,
                          type: type,
                          width: YUV_WIDTH,
                          height: YUV_HEIGHT,
                          direction: direction,
                          angle: angle) { outYuv in
            if let outYuv = outYuv {
                let wSize = outputStream.write(outYuv, maxLength: bufferSize)
                dstCnt   += 1
//                print("旋转并写入: \(wSize)，耗时: \(Date().timeIntervalSince1970 - preTime.timeIntervalSince1970) s")
                if dstCnt == srcCnt, inputStream.streamStatus == .closed {
                    pointer.deallocate()
                    outputStream.close()
                }
            }
            mutex.signal()
        }
    }
    inputStream.close()
    print("总耗时: \(Date().timeIntervalSince1970 - start.timeIntervalSince1970) s")
    // 这里只是为了避免执行完成 crash，因为 readData 比 conver&writeData 块，最后一帧还没写完
    mutex.signal()
    print("旋转 NV12 完成: \(oputFilePath)")
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
//                print("转换并写入: \(wSize)，耗时: \(Date().timeIntervalSince1970 - start.timeIntervalSince1970)s")
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


private func testRotateYUV() {
    var fileName: String = ""
    for t in SYYuv_nv12.rawValue...SYYuv_nv21.rawValue {
        let type = SYYuvType(rawValue: t)
        switch type {
        case SYYuv_i420:
            fileName = "20230410211001_I420"
        case SYYuv_yv12:
            fileName = "20230410211001_YV12"
        case SYYuv_nv12:
            fileName = "20230410211001_NV12"
        case SYYuv_nv21:
            fileName = "20230410211001_NV21"
        default:
            return
        }
        for dir in 0...SYRotate_antiClockwise.rawValue {
            let direction = SYRotateDirection(rawValue: dir)
            for ang in 0...SYRotate_360.rawValue {
                let angle = SYRotateDegree(rawValue: ang)
                testRotateYUV(type: type,
                              fileName: fileName,
                              direction: direction,
                              angle: angle)
            }
        }
    }
}

testRotateYUV()

//testNV12ToRGB24()

