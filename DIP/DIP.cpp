
#include "Course_Lib//myHist.h"

int main()
{
    myHist* pHist = new myHist(myHist::Type::Image);

    int  neighbor[9][2] = { {-1,-1}, { 0,-1}, {+1,-1},
                            {-1, 0}, { 0, 0}, {+1, 0},
                            {-1,+1}, { 0,+1}, {+1,+1} };
    float w_avg = 16.0, weight[9] = { 1.0, 2.0, 1.0,
                                      2.0, 4.0, 2.0,
                                      1.0, 2.0, 1.0 };


    cv::Mat m_in = cv::imread("images//lena.bmp", 0);

    int R = m_in.rows,
        C = m_in.cols,
        matrix_size = 9;


    cv::Mat m_out;
    m_out.create(R, C, CV_8UC1);

    if(m_in.empty()){
        std::cout << "Error: Image not found!" << std::endl;
        return -1;
	}

    for (int r = 1; r < R - 1; r++)
        for (int c = 1; c < C - 1; c++) {
            float sum = 0.0;
            for (int i = 0; i < matrix_size; i++) {
                int mr = r + neighbor[i][1];
                int mc = c + neighbor[i][0];
                sum += m_in.at<uchar>(mr, mc) * weight[i];
            }// End of for(m)
            sum /= w_avg;
            if (sum > 255)
                sum = 255;
            else if (sum < 0)
                sum = 0;
            m_out.at<uchar>(r, c) = sum;
        }
    // ################################################m_in picture and hist show
    cv::namedWindow("Lena_before", cv::WINDOW_AUTOSIZE);
    cv::imshow("Lena_before", m_in);
    //---------------------------------------
    pHist->CalcHist(m_in);
    m_in = cv::Scalar::all(0);
    pHist->Show(m_in);
    cv::namedWindow("intput hist", cv::WINDOW_AUTOSIZE);
    cv::imshow("intput hist", m_in);
    pHist->Clear();
    // ################################################m_out picture and hist show
    cv::namedWindow("Lena_after", cv::WINDOW_AUTOSIZE);
    cv::imshow("Lena_after", m_out);
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

