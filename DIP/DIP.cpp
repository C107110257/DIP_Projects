
#include <iostream>
#include <opencv2\opencv.hpp>
int main()
{
    cv::Mat mat0 = cv::imread("images/lena.bmp", 0);
    cv::Mat mat1, mat2;
    int ROW = mat0.rows;
    int COL = mat0.cols;
    mat1.create(ROW, COL, CV_8UC1);
    mat2.create(ROW, COL, CV_8UC1);
    //------------------------------------------------------------------------------------------
    for (int r = 0; r < ROW; r++)
        for (int c = 0; c < COL; c++) {
            uchar v = mat0.at<uchar>(r, c);
            mat1.at<uchar>(ROW - r - 1, c) = v;
            mat2.at<uchar>(r, COL - c - 1) = v;
        }
    //------------------------------------------------------------------------------------------
    cv::namedWindow("Lena_gray_level", cv::WINDOW_AUTOSIZE);
    cv::imshow("Lena_gray_level", mat0);
    //------------------------------------------------------------------------------------------
    cv::namedWindow("Lena_upside_down", cv::WINDOW_AUTOSIZE);
    cv::imshow("Lena_upside_down", mat1);
    //------------------------------------------------------------------------------------------
    cv::namedWindow("Lena_right_side_left", cv::WINDOW_AUTOSIZE);
    cv::imshow("Lena_right_side_left", mat2);
    //------------------------------------------------------------------------------------------
    cv::waitKey(0);
    return 0;
}

