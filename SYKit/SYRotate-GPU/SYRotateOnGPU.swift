//
//  SYRotateOnGPU.swift
//  SYKitExample-GPU
//
//  Created by ShenYuanLuo on 2023/4/10.
//  Copyright © 2023 shenyuanluo. All rights reserved.
//

import Foundation
import Metal

/// 旋转结果回调
typealias SYRotateYUVHandler = (_ outYuv: UnsafeMutableRawPointer?) -> Void

class SYRotateOnGPU {
    static var device: MTLDevice!               // 用来计算的设备（即，GPU；一般一个 APP 只用一个对象，所以设置为“类对象”）
    static var commandQueue: MTLCommandQueue!   // 命令队列（控制渲染命令按部就班执行；一般一个 APP 只用一个对象，所以设置为“类对象”）
    static var library: MTLLibrary!             // 着色器源代码程序（从 Bundle 加载所有着色器文件（.metal），一般一个 APP 只用一个对象，所以设置为“类对象”）
    
    private var rotateNV12Pipeline: MTLComputePipelineState!    // 旋转NV12-内核计算管线
    
    init() {
        guard let device = MTLCreateSystemDefaultDevice(),
            let commandQueue = device.makeCommandQueue(),
            let library = device.makeDefaultLibrary() else {
            fatalError("GPU not available")
        }
        SYRotateOnGPU.device       = device
        SYRotateOnGPU.commandQueue = commandQueue
        SYRotateOnGPU.library      = library
        
        // 创建内核计算函数
        guard let rotateNV12Fun = library.makeFunction(name: "NV12Rotate") else {
            fatalError("Can't crate kernel compute funtions")
        }
        do {    // 创建内核计算管线（耗性能，不宜频繁操作）
            self.rotateNV12Pipeline  = try device.makeComputePipelineState(function: rotateNV12Fun)
        } catch let error {
            print("Create rotate-array pipeline error: \(error.localizedDescription)")
        }
    }
    
    
    /// 旋转 YUV-nv12 图像
    /// - Parameters:
    ///   - data: yuv 数据
    ///   - width: 图像-宽度
    ///   - height: 图像-高度
    ///   - isRight: 是否向右旋转（0：向左，1：向右；旋转 90度）
    ///   - completion: 结果回调
    public func nv12Rotate(inYuv: UnsafeRawPointer, width: Int, height: Int, direction: SYRotateDirection, completion: @escaping SYRotateYUVHandler) {
        var width     = width
        var height    = height
        let dataSize  = Int(Float(width * height) * 1.5)
        var direction = direction
        let inYUVBuff = SYRotateOnGPU.device.makeBuffer(bytes: inYuv,
                                                        length: MemoryLayout<UInt8>.size * dataSize,
                                                        options: .storageModeShared)    // 使用共享内存（CPU 和 GPU都可以访问）
        inYUVBuff?.label = "rotate_nv12_inYUV"
        let outYUVBuff = SYRotateOnGPU.device.makeBuffer(length: MemoryLayout<UInt8>.size * dataSize,
                                                         options: .storageModeShared)   // 使用共享内存（CPU 和 GPU都可以访问）
        outYUVBuff?.label = "rotate_nv12_outYUV"
        guard let commandBuffer = SYRotateOnGPU.commandQueue.makeCommandBuffer(),   // 创建命令缓存
            let computeEncoder = commandBuffer.makeComputeCommandEncoder() else {    // 创建命令编码器
            print("Can not execute compute.")
            return
        }
        computeEncoder.setComputePipelineState(self.rotateNV12Pipeline) // 设置内核计算管线
        computeEncoder.setBytes(&width, length: MemoryLayout<Int>.stride, index: 11)    // 图像-宽度
        computeEncoder.setBytes(&height, length: MemoryLayout<Int>.stride, index: 12)   // 图像-高度
        computeEncoder.setBytes(&direction, length: MemoryLayout<Int>.stride, index: 13)  // 图像-旋转方向
        computeEncoder.setBuffer(inYUVBuff, offset: 0, index: 0)    // 原始图像数据缓存
        computeEncoder.setBuffer(outYUVBuff, offset: 0, index: 1)   // 结果图像数据缓存
        
#if false   // 为 true 时，模拟器不支持，会 crash
        let gridSize   = MTLSize(width: dataSize, height: 1, depth: 1)  // GPU 内核计算网格大小
        var maxThreads = self.rotateNV12Pipeline.maxTotalThreadsPerThreadgroup  // 每组最大线程数
        if maxThreads > dataSize {
            maxThreads = dataSize
        }
        let threadSize = MTLSize(width: maxThreads, height: 1, depth: 1)    // 线程大小
        // 使用非均匀线程组的网格编码计算命令,该方法能够更有效的利用GPU资源， 但是该方法最低支持到A11处理器(芯片)
        computeEncoder.dispatchThreads(gridSize, threadsPerThreadgroup: threadSize) // 注意：模拟器不支持
#else
        // 使用均匀线程组边界的网格编码计算命令（shader 内部需要判断 thread pos 越界问题）
        let execWidth  = self.rotateNV12Pipeline.threadExecutionWidth
        let threadSize = MTLSize(width: execWidth, height: 1, depth: 1)   // 一维数据 height、depth 都是 1
        let groupSize  = MTLSize(width: (dataSize + execWidth - 1) / execWidth, height: 1, depth: 1)   // 一维数据 height、depth 都是 1
        computeEncoder.dispatchThreadgroups(groupSize, threadsPerThreadgroup: threadSize)
#endif

        // 设置处理完成回调
        commandBuffer.addCompletedHandler { buff in
            completion(outYUVBuff?.contents())
        }

        computeEncoder.endEncoding()
        commandBuffer.commit()
    }
}
