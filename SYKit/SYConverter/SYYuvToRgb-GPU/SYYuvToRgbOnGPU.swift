//
//  SYYuvToRgbOnGPU.swift
//  SYKitExample-GPU
//
//  Created by ShenYuanLuo on 2023/4/10.
//  Copyright © 2023年 http://blog.shenyuanluo.com/ All rights reserved.
//

import Foundation
import Metal

/// 旋转结果回调
typealias SYYUVToRGBHandler = (_ outRGB: UnsafeMutableRawPointer?) -> Void

class SYYuvToRgbOnGPU {
    static var device: MTLDevice!               // 用来计算的设备（即，GPU；一般一个 APP 只用一个对象，所以设置为“类对象”）
    static var commandQueue: MTLCommandQueue!   // 命令队列（控制渲染命令按部就班执行；一般一个 APP 只用一个对象，所以设置为“类对象”）
    static var library: MTLLibrary!             // 着色器源代码程序（从 Bundle 加载所有着色器文件（.metal），一般一个 APP 只用一个对象，所以设置为“类对象”）
    
    private var nv12ToRGB24Pipeline: MTLComputePipelineState!    // 旋转NV12-内核计算管线
    
    init() {
        guard let device = MTLCreateSystemDefaultDevice(),
            let commandQueue = device.makeCommandQueue(),
            let library = device.makeDefaultLibrary() else {
            fatalError("GPU not available")
        }
        SYYuvToRgbOnGPU.device       = device
        SYYuvToRgbOnGPU.commandQueue = commandQueue
        SYYuvToRgbOnGPU.library      = library
        
        // 创建内核计算函数
        guard let nv12ToRGB24Fun = library.makeFunction(name: "NV12ToRGB24") else {
            fatalError("Can't crate kernel compute funtions")
        }
        do {    // 创建内核计算管线（耗性能，不宜频繁操作）
            self.nv12ToRGB24Pipeline = try device.makeComputePipelineState(function: nv12ToRGB24Fun)
        } catch let error {
            print("Create NV12->RGB24 pipeline error: \(error.localizedDescription)")
        }
    }
    
    
    /// NV12 转 RGB24
    /// - Parameters:
    ///   - data: yuv 数据
    ///   - width: 图像-宽度
    ///   - height: 图像-高度
    ///   - completion: 结果回调
    public func nv12ToRGB24(inYuv: UnsafeRawPointer, width: Int, height: Int, completion: @escaping SYYUVToRGBHandler) {
        var width     = width
        var height    = height
        let yuvSize   = Int(Float(width * height) * 1.5)
        let rgbSize   = width * height * 3
        let inYUVBuff = SYYuvToRgbOnGPU.device.makeBuffer(bytes: inYuv,
                                                          length: MemoryLayout<UInt8>.size * yuvSize,
                                                          options: .storageModeShared)    // 使用共享内存（CPU 和 GPU都可以访问）
        inYUVBuff?.label = "nv12ToRGB24_inYUV"
        let outRGBBuff = SYYuvToRgbOnGPU.device.makeBuffer(length: MemoryLayout<UInt8>.size * rgbSize,
                                                           options: .storageModeShared)   // 使用共享内存（CPU 和 GPU都可以访问）
        outRGBBuff?.label = "nv12ToRGB24_outRGB"
        guard let commandBuffer = SYYuvToRgbOnGPU.commandQueue.makeCommandBuffer(),   // 创建命令缓存
              let commandEncoder = commandBuffer.makeComputeCommandEncoder() else {    // 创建命令编码器
            print("Can not execute compute.")
            return
        }
        commandEncoder.setComputePipelineState(self.nv12ToRGB24Pipeline) // 设置内核计算管线
        commandEncoder.setBytes(&width, length: MemoryLayout<Int>.stride, index: 11)    // 图像-宽度
        commandEncoder.setBytes(&height, length: MemoryLayout<Int>.stride, index: 12)   // 图像-高度
        commandEncoder.setBuffer(inYUVBuff, offset: 0, index: 0)    // 原始图像数据缓存
        commandEncoder.setBuffer(outRGBBuff, offset: 0, index: 1)   // 结果图像数据缓存
        
#if false   // 为 true 时，模拟器不支持，会 crash
        let gridSize   = MTLSize(width: yuvSize, height: 1, depth: 1)  // GPU 内核计算网格大小
        var maxThreads = self.nv12ToRGB24Pipeline.maxTotalThreadsPerThreadgroup  // 每组最大线程数
        if maxThreads > yuvSize {
            maxThreads = yuvSize
        }
        let threadSize = MTLSize(width: maxThreads, height: 1, depth: 1)    // 线程大小
        // 使用非均匀线程组的网格编码计算命令,该方法能够更有效的利用GPU资源， 但是该方法最低支持到A11处理器(芯片)
        commandEncoder.dispatchThreads(gridSize, threadsPerThreadgroup: threadSize) // 注意：模拟器不支持
#else
        // 使用均匀线程组边界的网格编码计算命令（shader 内部需要判断 thread pos 越界问题）
        let execWidth  = self.nv12ToRGB24Pipeline.threadExecutionWidth
        let threadSize = MTLSize(width: execWidth, height: 1, depth: 1)   // 一维数据 height、depth 都是 1
        let groupSize  = MTLSize(width: (yuvSize + execWidth - 1) / execWidth, height: 1, depth: 1)   // 一维数据 height、depth 都是 1
        commandEncoder.dispatchThreadgroups(groupSize, threadsPerThreadgroup: threadSize)
#endif

        // 设置处理完成回调
        commandBuffer.addCompletedHandler { buff in
            completion(outRGBBuff?.contents())
        }

        commandEncoder.endEncoding()
        commandBuffer.commit()
    }
}
