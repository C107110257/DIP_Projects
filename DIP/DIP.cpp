#include "Course_Lib//myHist.h"
cv::Mat m_in = cv::imread("images//lena.bmp", 0);
cv::Mat m_tar = cv::imread("images//license_plate.bmp", 0);
int ROW = m_in.rows;
int COL = m_in.cols;

int main()
{
    
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
    //################################################  tar_hist and G function
    for (int r = 0; r < m_tar.rows; r++)
        for (int c = 0; c < m_tar.cols; c++) {
            int gray_value = m_tar.at<uchar>(r, c);
            m_tar_hist[gray_value]++;
        }
    //----------------------------------------  G
    acc_pr = 0.0;
    for (int i = 0; i < 256; i++) {
        m_tar_hist[i] = m_tar_hist[i] / (m_tar.rows * m_tar.cols);
        acc_pr = acc_pr + m_tar_hist[i];
        G[i] = int(255.0 * acc_pr);
    }
    //################################################  G_invert function
    for (int y = 0; y < 256; y++) {
        G_inv[y] = -1;
        for (int x = 0; x < 256; x++) {
            if (G[x] == y) {
                G_inv[y] = x;
                break;
            }
        }
    }
    //--------------------------------------------
    for (int y = 0; y < 256; y++) {
        if (G_inv[y] == -1) {
            for (int offset = 0; offset < 256; offset++) {
                if ((y - offset >= 0) && (G_inv[y - offset] != -1)) {
                    G_inv[y] = G_inv[y - offset];
                    break;
                }
                if ((y + offset < 255) && (G_inv[y + offset] != -1)) {
                    G_inv[y] = G_inv[y + offset];
                    break;
                }
            }
        }
    }
    //################################################ Z function
    for (int x = 0; x < 256; x++) {
        int y = T[x];
        Z[x] = G_inv[y];
    }
    //################################################ m_out 
    for (int r = 0; r < ROW; r++)
        for (int c = 0; c < COL; c++) {
            int gray_value = m_in.at<uchar>(r, c);
            int out_v = Z[gray_value];
            m_out.at<uchar>(r, c) = out_v;
        }
    // ################################################m_in picture and hist show
    cv::namedWindow("Lena_old", cv::WINDOW_AUTOSIZE);
    cv::imshow("Lena_old", m_in);
    //---------------------------------------
    pHist->CalcHist(m_in);
    m_in = cv::Scalar::all(0);
    pHist->Show(m_in);
    cv::namedWindow("intput hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("intput hist", m_in);
    pHist->Clear();
    // ################################################target picture and hist show
    cv::namedWindow("target picture", cv::WINDOW_AUTOSIZE);
    cv::imshow("target picture", m_tar);
    //---------------------------------------
    pHist->CalcHist(m_tar);
    m_tar = cv::Scalar::all(0);
    pHist->Show(m_tar);
    cv::namedWindow("target picture hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("target picture hist", m_tar);
    pHist->Clear();
    // ################################################m_out picture and hist show
    cv::namedWindow("Lena_new", cv::WINDOW_AUTOSIZE);
    cv::imshow("Lena_new", m_out);
    //---------------------------------------
    pHist->CalcHist(m_out);
    m_out = cv::Scalar::all(0);
    pHist->Show(m_out);
    cv::namedWindow("output hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("output hist", m_out);
    pHist->Clear();
    cv::waitKey(0);
    return 0;
}