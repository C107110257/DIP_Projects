#include "Course_Lib//myHist.h"
cv::Mat m_in = cv::imread("images//lena.bmp", 0);

int ROW = m_in.rows;
int COL = m_in.cols;

int main()
{
    //cv::Mat m_tar = cv::imread("lena.bmp", 0);
    cv::Mat m_out;
    m_out.create(ROW, COL, CV_8UC1);

    myHist* pHist = new myHist(myHist::Type::Image);

    //################################################
    float m_in_hist[256] = { 0.0 }, m_tar_hist[256] = { 0.0 }, acc_pr;
    float T[256] = { 0 }, G[256] = { 0 }, G_inv[256] = { 0 }, Z[256] = { 0 };
    //################################################  in_hist and T function
    for (int r = 0; r < ROW; r++)
        for (int c = 0; c < COL; c++) {
            int gray_value = m_in.at<uchar>(r, c);
            m_in_hist[gray_value]++;
        }
    //----------------------------------------  T 
    acc_pr = 0.0;
    for (int i = 0; i < 256; i++) {
        m_in_hist[i] = m_in_hist[i] / (ROW * COL);
        acc_pr = acc_pr + m_in_hist[i];
        T[i] = int(255.0 * acc_pr);
    }

    //################################################ m_out 
    for (int r = 0; r < ROW; r++)
        for (int c = 0; c < COL; c++) {
            int gray_value = m_in.at<uchar>(r, c);
            int out_v = T[gray_value];//Z[gray_value]
            m_out.at<uchar>(r, c) = out_v;
        }
    if (m_in.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }
    // ################################################m_in picture and hist show
    cv::namedWindow("Lena_old", cv::WINDOW_AUTOSIZE);
    cv::imshow("Lena_old", m_in);
    pHist->CalcHist(m_in);
    m_in = cv::Scalar::all(0);
    pHist->Show(m_in);
    cv::namedWindow("intput hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("intput hist", m_in);
    pHist->Clear();
    // ################################################m_out picture and hist show
    cv::namedWindow("Lena_new", cv::WINDOW_AUTOSIZE);
    cv::imshow("Lena_new", m_out);
    pHist->CalcHist(m_out);
    m_out = cv::Scalar::all(0);
    pHist->Show(m_out);
    cv::namedWindow("output hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("output hist", m_out);
    pHist->Clear();
    cv::waitKey(0);
    return 0;
}