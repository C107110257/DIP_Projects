
#include "Course_Lib//myHist.h"
#include <time.h>
cv::Mat m_original = cv::imread("images//dpi_hw04.bmp", 0);
int ROW = m_original.rows;
int COL = m_original.cols;
int QuickSort(int* A, int left, int right)
{
    int i, j, s, Temp;
    if (left < right) {
        s = A[(left + right) / 2];
        i = left - 1;
        j = right + 1;
        while (1) {
            while (A[++i] < s);  // 向右找
            while (A[--j] > s);  // 向左找
            if (i >= j) break;
            Temp = A[i];
            A[i] = A[j];
            A[j] = Temp;
        }
        QuickSort(A, left, i - 1);   // 對左邊進行遞迴
        QuickSort(A, j + 1, right);  // 對右邊進行遞迴
    }
    return A[right + 1];
}
//-------------------------------------------------------/* 產生 [min , max] 的整數亂數 */
int my_random(int min, int max) {
    return rand() % (max - min + 1) + min;
}
int main()
{
    srand(time(NULL));

    cv::Mat m_spn, m_after;
    m_spn.create(ROW, COL, CV_8UC1);
    m_after.create(ROW, COL, CV_8UC1);
    //-----------------------------------------------------------Add [Salt-and-Pepper Noise],p0=0.1,p255=0.1
    for (int r = 0; r < ROW; r++)
        for (int c = 0; c < COL; c++) {
            int rd = my_random(1, 100);//[min , max]
            if (rd <= 10)
                m_spn.at<uchar>(r, c) = 0;
            else if (rd <= 20)
                m_spn.at<uchar>(r, c) = 255;
            else
                m_spn.at<uchar>(r, c) = m_original.at<uchar>(r, c);
        }


    //------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------
    int Smax = 7;
    for (int r = 1; r < ROW - 1; r++) {
        for (int c = 1; c < COL - 1; c++) {
            bool b = false, Zxy = m_spn.at<uchar>(r, c);
            //------------------------------------------------------------------------------------------
            for (int s = 3; s <= Smax; s += 2) {//----------------------
                int s2 = s * s, viot = 0;
                int* arr = new int[s2];
                for (int i = -s / 2; i <= s / 2; i++) {
                    for (int j = -s / 2; j <= s / 2; j++) {
                        if (r + i < 0 || r + i == ROW || c + j < 0 || c + j == COL) {
                            b = true;
                            break;
                        }
                        arr[viot] = m_spn.at<uchar>(r + i, c + j);
                        viot++;
                    }
                    if (b) { break; }
                }
                if (b || ((s2) != viot)) { break; }
                QuickSort(arr, 0, viot - 1);
                int A1 = arr[viot / 2] - arr[0],
                    A2 = arr[viot / 2] - arr[viot - 1];
                //------------------------------------------------------------------------------------------
                if (A1 > 0 && A2 < 0) {
                    int B1 = Zxy - arr[0],
                        B2 = Zxy - arr[viot - 1];
                    if (B1 > 0 && B2 < 0) {
                        m_after.at<uchar>(r, c) = Zxy;
                    }
                    else {
                        m_after.at<uchar>(r, c) = arr[viot / 2];
                    }
                }
                else if (s == Smax) {
                    m_after.at<uchar>(r, c) = Zxy;
                }
                //------------------------------------------------------------------------------------------
                delete[] arr;
            }
        }
    }
    //------------------------------------------------------------------------------------------
    myHist* pHist = new myHist(myHist::Type::Image);
    //------------------------------------------------------------------------------------------
    cv::namedWindow("original", cv::WINDOW_AUTOSIZE);
    cv::imshow("original", m_original);

    pHist->CalcHist(m_original);
    m_original = cv::Scalar::all(0);
    pHist->Show(m_original);
    cv::namedWindow("original hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("original hist", m_original);
    pHist->Clear();

    //------------------------------------------------------------------------------------------
    cv::namedWindow("Add Salt and Pepper Noise", cv::WINDOW_AUTOSIZE);
    cv::imshow("Add Salt and Pepper Noise", m_spn);

    pHist->CalcHist(m_spn);
    m_spn = cv::Scalar::all(0);
    pHist->Show(m_spn);
    cv::namedWindow("Add Salt and Pepper Noise hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("Add Salt and Pepper Noise hist", m_spn);
    pHist->Clear();

    //------------------------------------------------------------------------------------------
    cv::namedWindow("After", cv::WINDOW_AUTOSIZE);
    cv::imshow("After", m_after);

    pHist->CalcHist(m_after);
    m_after = cv::Scalar::all(0);
    pHist->Show(m_after);
    cv::namedWindow("After hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("After hist", m_after);
    pHist->Clear();


    cv::waitKey(0);
    return 0;
}

