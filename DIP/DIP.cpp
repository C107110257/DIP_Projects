
#include "Course_Lib//myHist.h"
#include <algorithm>
#include <vector>
#include <unordered_map>
using namespace std;
class DisjointSet { //to represent disjoint set
    unordered_map<int, int> parent;
public:
    void makeSet(vector<int> const& wholeset) {
        //perform makeset operation
        for (int i : wholeset) // create n disjoint sets
            parent[i] = i;
    }
    int Find(int l) { // Find the root of the set in which element l belongs
        if (parent[l] == l) // if l is root
            return l;
        return Find(parent[l]); // recurs for parent till we find root
    }
    void Union(int m, int n) { // perform Union of two subsets m and n  
        int x = Find(m);
        int y = Find(n);
        parent[x] = y;
    }
};
int findIndex(const vector<int>& arr, int item) {

    for (auto i = 0; i < arr.size(); ++i) {
        if (arr[i] == item)
            return i;
    }
}
int my_random(int min, int max) {
    return rand() % (max - min + 1) + min;
}
int main()
{
    cv::Mat m_in = cv::imread("images//license_plate.bmp", 0);
    int ROW = m_in.rows;
    int COL = m_in.cols;
    cv::Mat m_out, show_t, binary_mat, display_plate;
    binary_mat.create(ROW, COL, CV_8UC1);
    m_out.create(ROW, COL, CV_8UC1);
    show_t.create(ROW, COL, CV_8UC1);
    display_plate.create(ROW, COL, CV_8UC3);
    myHist* pHist = new myHist(myHist::Type::Image);
    float m_in_hist[256] = { 0.0 };
    //################################################  in_hist 
    for (int r = 0; r < ROW; r++)
        for (int c = 0; c < COL; c++) {
            int gray_value = m_in.at<uchar>(r, c);
            m_in_hist[gray_value]++;
        }
    //----------------------------------------   
    for (int i = 0; i < 256; i++) {
        m_in_hist[i] = m_in_hist[i] / (ROW * COL);
    }
    //################################################  Otsu----
    double p0 = 0.0, p1 = 1.0,
        Q0 = 0.0, Q1 = 0.0;
    for (int i = 0; i < 256; i++) {
        Q1 += i * m_in_hist[i];
    }    int t_star = 0;
    double max_bc_var = 0.0;
    for (int i = 0; i < 255; i++) {
        p0 = p0 + m_in_hist[i];
        p1 = 1.0 - p0;
        Q0 = Q0 + i * m_in_hist[i];
        Q1 = Q1 - i * m_in_hist[i];

        if (p0 <= 0.0 || p1 <= 0.0) {
            continue;
        }
        double mu0 = Q0 / p0,
            mu1 = Q1 / p1;
        double bc_var = p0 * p1 * pow((mu0 - mu1), 2);
        if (bc_var >= max_bc_var) {
            t_star = i;
            max_bc_var = bc_var;
        }
    }
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            if (m_in.at<uchar>(r, c) <= t_star) {
                binary_mat.at<uchar>(r, c) = 0;
            }
            else {
                binary_mat.at<uchar>(r, c) = 255;
            }
        }
    }
    // // ################################################show_t
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            show_t.at<uchar>(r, c) = 255;
        }
    }
    // set string and variables
    std::string text = "optimal threshold = " + to_string(t_star);
    int font_face = cv::FONT_HERSHEY_COMPLEX;
    double font_scale = 2;
    int thickness = 2;
    int baseline;
    // get text size
    cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);

    // decide text position
    cv::Point origin;
    origin.x = COL >> 1 - text_size.width >> 1;
    origin.y = ROW >> 1 + text_size.height >> 1;
    cv::putText(show_t, text, origin, font_face, font_scale, cv::Scalar(0, 255, 255), thickness, 8, 0);
    cv::namedWindow("threshold", cv::WINDOW_AUTOSIZE);
    cv::imshow("threshold", show_t);
    // ###############################################intput
    cv::namedWindow("intput", cv::WINDOW_AUTOSIZE);
    cv::imshow("intput", m_in);
    //################################################binary
    cv::namedWindow("binary", cv::WINDOW_AUTOSIZE);
    cv::imshow("binary", binary_mat);
    // ###############################################connected component labeling
    //---------------------------------------- Pass 1:
    int** label_mat, nl = 1;
    label_mat = new int* [ROW];
    for (int i = 0; i < ROW; i++) {
        label_mat[i] = new int[COL];
    }
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            label_mat[r][c] = 0;
        }
    }
    vector<int> equivalences{}, new_label{};
    for (int r = 1; r < ROW; r++) {
        for (int c = 1; c < COL; c++) {
            if (binary_mat.at<uchar>(r, c) != 255) {
                vector<int> adjacent_labels{};
                int upper_label = label_mat[r - 1][c];
                if (upper_label != 0) {
                    adjacent_labels.insert(adjacent_labels.end(), upper_label);
                }
                int left_label = label_mat[r][c - 1];
                if (left_label != 0 && left_label != upper_label) {
                    adjacent_labels.insert(adjacent_labels.end(), left_label);
                }
                if (adjacent_labels.size() == 0) {
                    label_mat[r][c] = nl;
                    new_label.insert(new_label.end(), nl);
                    nl += 1;
                }
                else {
                    label_mat[r][c] = int(*min_element(adjacent_labels.begin(), adjacent_labels.end()));
                }
                if (adjacent_labels.size() == 2) {
                    equivalences.insert(equivalences.end(), adjacent_labels[0]);
                    equivalences.insert(equivalences.end(), adjacent_labels[1]);
                }
            }
        }
    }
    //---------------------------------------- Pass 2:
    DisjointSet dis;
    dis.makeSet(new_label);
    for (int i = 0; i < equivalences.size(); i += 2) {
        dis.Union(equivalences[i], equivalences[i + 1]);
    }
    for (int r = 1; r < ROW; r++) {
        for (int c = 1; c < COL; c++) {
            if (label_mat[r][c] != 0) {
                label_mat[r][c] = dis.Find(label_mat[r][c]);
            }
        }
    }
    vector<int>labels = { 0 };
    for (int r = 1; r < ROW; r++) {
        for (int c = 1; c < COL; c++) {
            if (label_mat[r][c] != 0) {
                if (!(find(labels.begin(), labels.end(), label_mat[r][c]) != labels.end())) {
                    labels.insert(labels.end(), label_mat[r][c]);
                }
                label_mat[r][c] = findIndex(labels, label_mat[r][c]);
            }
        }
    }
    // ###############################################label mat display
    vector<int> color_plate{ 255,255,255 };
    for (int i = 0; i < labels.size(); i++) {//239*3
        color_plate.insert(color_plate.end(), my_random(0, 255));
        color_plate.insert(color_plate.end(), my_random(0, 255));
        color_plate.insert(color_plate.end(), my_random(0, 255));
    }
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            if (label_mat[r][c] > -1) {
                display_plate.at<cv::Vec3b>(r, c)[0] = color_plate[label_mat[r][c] + 2];
                display_plate.at<cv::Vec3b>(r, c)[1] = color_plate[label_mat[r][c] + 1];
                display_plate.at<cv::Vec3b>(r, c)[2] = color_plate[label_mat[r][c]];
            }
        }
    }
    for (int i = 0; i < ROW; i++) {
        delete[]label_mat[i];
    }
    delete[]label_mat;
    // ###############################################display
    cv::namedWindow("display", cv::WINDOW_AUTOSIZE);
    cv::imshow("display", display_plate);
    // ###############################################
    cv::waitKey(0);
    return 0;
}

