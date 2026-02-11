
#include "Course_Lib//myHist.h"
cv::Mat m_original = cv::imread("images//dpi_hw04.bmp", 0);
int ROW = m_original.rows;
int COL = m_original.cols;
int QuickSort(int* A, int left, int right)
{
    int i, j, s, Temp;
    if (left < right) {
        s = A[(left + right) >> 1];
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
double my_generate_gausson(double x) {
    return (1.0 / (sqrt(6.28318530718) * 3)) * exp(-x * x / 18.0);   //p(x)=1/sqrt(2*pi)*sigma*exp(-pow(x-mean)/2*pow(sigma))
}
//------------------------------------------------------------------------------------------
int main()
{
    srand(time(NULL));

    cv::Mat m_gn, m_after;
    m_gn.create(ROW, COL, CV_8UC1);
    m_after.create(ROW, COL, CV_8UC1);
    //-----------------------------------------------------------Add [Gaussian Noise], u(mean)=0, sigma=3.0

    int p[19] = { 0 };
    p[0] = int(my_generate_gausson(0) * 100);
    for (int x = 2; x <= 18; x += 2) {   //對稱映射機率一樣
        int pt = int(my_generate_gausson(x / 2.0) * 100);
        p[x - 1] = p[x - 2] + pt;
        p[x] = p[x - 1] + pt;
    }
    for (int r = 0; r < ROW; r++)
        for (int c = 0; c < COL; c++) {
            int tp = NULL, dis = 0, inv = 1, rd = my_random(1, 100);//[min , max]
            for (int i = 0; i < 19; i++) {
                inv *= -1;
                dis += (i * inv);
                if (rd <= p[i]) {
                    tp = m_original.at<uchar>(r, c) + dis;
                    break;
                }
            }
            if (tp == NULL)
                tp = m_original.at<uchar>(r, c) + my_random(-10, 10);
            if (tp < 0)
                tp = 0;
            else if (tp > 255)
                tp = 255;
            m_gn.at<uchar>(r, c) = tp;
        }
    //------------------------------------------------------------------------------------------
    int Smax = 7;
    for (int r = 1; r < ROW - 1; r++) {
        for (int c = 1; c < COL - 1; c++) {
            bool b = false, Zxy = m_gn.at<uchar>(r, c);
            //------------------------------------------------------------------------------------------
            for (int s = 3; s <= Smax; s += 2) {//----------------------
                int s2 = s * s, viot = 0;
                int* arr = new int[s2];
                for (int i = -(s >> 1); i <= s >> 1; i++) {
                    for (int j = -(s >> 1); j <= s >> 1; j++) {
                        if (r + i < 0 || r + i == ROW || c + j < 0 || c + j == COL) {
                            b = true;
                            break;
                        }
                        arr[viot] = m_gn.at<uchar>(r + i, c + j);
                        viot++;
                    }
                    if (b) { break; }
                }
                if (b || ((s2) != viot)) { break; }
                QuickSort(arr, 0, viot - 1);
                int A1 = arr[viot >> 1] - arr[0],
                    A2 = arr[viot >> 1] - arr[viot - 1];
                //------------------------------------------------------------------------------------------
                if (A1 > 0 && A2 < 0) {
                    int B1 = Zxy - arr[0],
                        B2 = Zxy - arr[viot - 1];
                    if (B1 > 0 && B2 < 0) {
                        m_after.at<uchar>(r, c) = Zxy;
                    }
                    else {
                        m_after.at<uchar>(r, c) = arr[viot >> 1];
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
    cv::namedWindow("Add Gaussian Noise", cv::WINDOW_AUTOSIZE);
    cv::imshow("Add Gaussian Noise", m_gn);

    pHist->CalcHist(m_gn);
    m_gn = cv::Scalar::all(0);
    pHist->Show(m_gn);
    cv::namedWindow("Add Gaussian Noise hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("Add Gaussian Noise hist", m_gn);
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

