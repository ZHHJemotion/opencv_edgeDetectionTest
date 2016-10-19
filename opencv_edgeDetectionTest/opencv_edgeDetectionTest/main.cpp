//
//  main.cpp
//  opencv_edgeDetectionTest
//
//  Created by ZHHJemotion on 2016/10/19.
//  Copyright © 2016年 Lukas_Zhang. All rights reserved.
//

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv; // 命名空间
using namespace std;

#define PATH string("/Users/zhangxingjian/Desktop/Programming/C++/OpenCV/opencv_edgeDetectionTest/opencv_edgeDetectionTest/opencv_edgeDetectionTest/")

// ---------------------------------- 全局变量声明 -------------------------------------
//    Describe: 全局变量声明
// -----------------------------------------------------------------------------------
// 原图，原图的灰度图，效果图
Mat srcImage, srcGrayImage, dstImage;

// Canny 边缘检测相关变量
Mat cannyDetectedEdges; // Canny检测出的边缘
int cannyLowThreshold = 1; // TrackBar位置参数

// Laplace（算子）边缘检测相关变量
//Mat laplaceDetectedEdges; // Laplace检测出的边缘
Mat dstLaplaceImage, dstAbsLaplaceImage; // Laplace 算子输出图和8-bit 输出图
int laplaceKernelSize = 1; // Laplace 内核尺寸 = TrackBar 位置参数

// Sobel 边缘检测相关参数
Mat sobelGradientX, sobelGradientY; // Sobel算子 X 与 Y 方向梯度
Mat sobelAbsGradientX, sobelAbsGradientY; // 8-bit 方向梯度
int sobelKernelSize = 1; // sobel 内核尺寸 = TrackBar位置参数

// scharr 滤波器相关参数
Mat scharrGradientX, scharrGradientY; // Scharr 滤波器 X 与 Y 方向梯度
Mat scharrAbsGradientX, scharrAbsGradientY; // 8-bit 方向梯度
// scharr无内核尺寸 ！！！



// --------------------------------- 全局函数申明部分 ------------------------------------
//    Describe: 全局函数申明
// ------------------------------------------------------------------------------------
static void showHelpText();
static void onCanny(int, void *); // Canny边缘检测窗口滚动条的回调函数
static void onLaplace(int, void *); // Laplace算子边缘检测滚动条的回调函数
static void onSobel(int, void *); // Sobel边缘检测窗口滚动条的回调函数
void onScharr(); // 封装了 Scharr 边缘检测相关代码的函数



// --------------------------------- main() Function ----------------------------------

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    // show some helpful information
    showHelpText();
    
    // 载入原图
    srcImage = imread(PATH+string("1.jpg"),1);
    // 异常处理
    if (!srcImage.data)
    {
        printf("error!!! We can't read the original image");
        return false;
    }
    
    // 显示原图
    namedWindow("the original image");
    imshow("the original image", srcImage);
    
    // 创建与 srcImage 相同大小和类型的矩阵（dstImage）
    dstImage.create(srcImage.size(), srcImage.type()); // 必须Size和Type都要（）
    
    // 将原图转化为灰度图
    cvtColor(srcImage, srcGrayImage, CV_BGR2GRAY);
    
    // 创建显示窗口
    namedWindow("the destination image of the Canny edge detection", WINDOW_AUTOSIZE);
    namedWindow("the destination image of the Laplace edge detection", WINDOW_AUTOSIZE);
    namedWindow("the destination image of the Sobel edge detection", WINDOW_AUTOSIZE);
    namedWindow("the destination image of the Scharr filter", WINDOW_AUTOSIZE);
    
    // 创建 TrackBar
    createTrackbar("Parameter Values", "the destination image of the Canny edge detection", &cannyLowThreshold, 120, onCanny);
    createTrackbar("Parameter Values", "the destination image of the Laplace edge detection", &laplaceKernelSize, 3, onLaplace);
    createTrackbar("Parameter Values", "the destination image of the Sobel edge detection", &sobelKernelSize, 3, onSobel);
    
    // 调用回调函数
    onCanny(cannyLowThreshold, 0);
    onLaplace(laplaceKernelSize, 0);
    onSobel(sobelKernelSize, 0);
    
    // 调用封装了 Scharr 边缘检测代码的函数onScharr
    onScharr();
    
    // 按下‘q’，程序退出
    while(char(waitKey(1)) != 'q') {}
    
    
    return 0;
}



// --------------------------------- showHelpText() function ------------------------------
//    Describe: output some helpful information
// ----------------------------------------------------------------------------------------
static void showHelpText()
{
    //输出一些帮助信息
    printf( "\n\n\t嗯。运行成功，请调整滚动条观察图像效果~\n\n"
           "\t按下“q”键时，程序退出~!\n"
           "\n\n\t\t\t\t by ZHHJemotion" );

}


// --------------------------------- onCanny() function -----------------------------------
//    Describe: Canny边缘检测回调函数
// ----------------------------------------------------------------------------------------
 static void onCanny(int,void *)
{
    // 先使用3X3内核来降噪
    Mat srcGrayImageBlur; // 降噪后的灰度图
    blur(srcGrayImage, srcGrayImageBlur, Size(3,3));
    
    // 运行 Canny 算子
    Canny(srcGrayImageBlur, cannyDetectedEdges, cannyLowThreshold, cannyLowThreshold*3, 3);
    
    // 先将dstImage 内的所有元素设为0，保证每次调用的时候为初始化0
    dstImage = Scalar::all(0);
    
    // 使用Canny算子输出的边缘图cannyDetectedEdges作为掩码，来将原图srcImage拷到目标图dstImage中
    // dstImage = srcImage * cannyDetectedEdges, cannyDetectedEdges是一个单通道的矩阵(即显示边缘值为1，有点类似稀疏矩阵)，拷贝值为1的位置
    srcImage.copyTo(dstImage, cannyDetectedEdges);
    
    // 显示效果图
    imshow("the destination image of the Canny edge detection", dstImage);
}


// --------------------------------- onLaplace() function -----------------------------------
//    Describe: Laplace边缘检测回调函数
// ----------------------------------------------------------------------------------------
static void onLaplace(int, void *)
{
    // 使用3x3内核的高斯滤波降噪
    Mat srcGrayImageBlur; // 降噪后的灰度图
    GaussianBlur( srcGrayImage, srcGrayImageBlur, Size(3,3), 0,0);
    
    // 使用 Laplace 算子
    Laplacian(srcGrayImageBlur, dstLaplaceImage, CV_16S, laplaceKernelSize*2+1, 1, 0);
    
    // 计算绝对值，并将结果转化为8-bit
    convertScaleAbs(dstLaplaceImage, dstAbsLaplaceImage);
    
    // 显示效果图
    imshow("the destination image of the Laplace edge detection", dstAbsLaplaceImage);
}



// --------------------------------- onSobel() function -----------------------------------
//    Describe: Sobel边缘检测回调函数
// ----------------------------------------------------------------------------------------
static void onSobel(int, void *)
{
    // 求 X 方向梯度
    Sobel(srcImage, sobelGradientX, CV_16S, 1, 0, sobelKernelSize*2+1, 1, 1);
    convertScaleAbs(sobelGradientX, sobelAbsGradientX); // 计算绝对值，并将结果转化为8-bit
    
    // 求 Y 方向梯度
    Sobel(srcImage, sobelGradientY, CV_16S, 0, 1, sobelKernelSize*2+1, 1, 1);
    convertScaleAbs(sobelGradientY, sobelAbsGradientY); // 计算绝对值，并将结果转化为8-bit
    
    // 合并梯度
    addWeighted(sobelAbsGradientX, 0.5, sobelAbsGradientY, 0.5, 0, dstImage);
    
    // 显示效果图
    imshow("the destination image of the Sobel edge detection", dstImage);
}



// --------------------------------- onScharr() function ----------------------------------
//    Describe: Scharr滤波器回调函数
// ----------------------------------------------------------------------------------------
void onScharr()
{
    // 求 X 方向梯度
    Scharr(srcImage, scharrGradientX, CV_16S, 1, 0, 1, 0); // Scharr无内核尺寸
    convertScaleAbs(scharrGradientX, scharrAbsGradientX); // 计算绝对值，并将结果转化为8-bit
    
    // 求 Y 方向梯度
    Scharr(srcImage, scharrGradientY, CV_16S, 0, 1, 1, 0); // Scharr无内核尺寸
    convertScaleAbs(scharrGradientY, scharrAbsGradientY); // 计算绝对值，并将结果转化为8-bit
    
    // 合并梯度
    addWeighted(scharrAbsGradientX, 0.5, scharrAbsGradientY, 0.5, 0, dstImage);
    
    // 显示效果图
    imshow("the destination image of the Scharr filter", dstImage);
}
