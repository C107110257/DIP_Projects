
#include "Course_Lib//myHist.h"
#include <algorithm>
#include <vector>
#include <unordered_map>

// Disjoint Set (Union-Find) implementation for managing label equivalences in connected component labeling
class DisjointSet {
	std::unordered_map<int, int> parent;
	std::unordered_map<int, int> rank;
public:
	void makeSet(std::vector<int> const& wholeset) {
		for (int i : wholeset) {
			parent[i] = i;
			rank[i] = 0;
		}
	}
	int Find(int l) {
		if (parent[l] != l)
			parent[l] = Find(parent[l]);
		return parent[l];
	}
	void Union(int m, int n) {
		int x = Find(m);
		int y = Find(n);
		if (x == y) return;

		if (rank[x] < rank[y])
			parent[x] = y;
		else if (rank[x] > rank[y])
			parent[y] = x;
		else {
			parent[y] = x;
			rank[x]++;
		}
	}
};

// Implementation of Otsu's Algorithm
void Otsu_Algorithm(const float* hist, int& t_star) {
	double p0 = 0.0;
	double p1 = 1.0;
	double Q0 = 0.0;
	double Q1 = 0.0;

	double mu0 = 0.0;
	double mu1 = 0.0;
	double bc_var = 0.0;
	double max_bc_var = 0.0;

	for (int i = 0; i < 256; i++) {
		Q1 += i * hist[i];
	}

	for (int i = 0; i < 255; i++) {
		p0 = p0 + hist[i];
		p1 = 1.0 - p0;
		Q0 = Q0 + i * hist[i];
		Q1 = Q1 - i * hist[i];

		if (p0 <= 0.0 || p1 <= 0.0) {
			continue;
		}
		mu0 = Q0 / p0;
		mu1 = Q1 / p1;
		bc_var = p0 * p1 * pow((mu0 - mu1), 2);
		if (bc_var >= max_bc_var) {
			t_star = i;
			max_bc_var = bc_var;
		}
	}
}

// Implementation of image to histogram
void Image2Histogram(const cv::Mat& m_in, float* hist) {
	int ROW = m_in.rows;
	int COL = m_in.cols;
	for (int r = 0; r < ROW; r++)
		for (int c = 0; c < COL; c++) {
			int gray_value = m_in.at<uchar>(r, c);
			hist[gray_value]++;
		}
	// normalize
	for (int i = 0; i < 256; i++) {
		hist[i] = hist[i] / (ROW * COL);
	}
}

// Implementation of connected component labeling
void Connected_Component_Labeling(const cv::Mat& binary_mat, cv::Mat& label_mat, int& label_count) {

	int ROW = binary_mat.rows;
	int COL = binary_mat.cols;

	//---------------------------------------- Pass 1:
	// Start from the second row and second column to avoid out-of-bounds access when checking upper and left neighbors.
	// For each foreground pixel, check its upper and left neighbors to determine the label.
	// If both neighbors are background, assign a new label.
	int nl = 1;
	// Use vectors to store new labels and equivalences
	std::vector<int> new_label{};
	// equivalences will store pairs of equivalent labels 
	// (e.g., [label1, label2, label3, label4] 
	// means label1 is equivalent to label2 and label3 is equivalent to label4)
	std::vector<int> equivalences{};
	for (int r = 1; r < ROW; r++) {
		for (int c = 1; c < COL; c++) {
			// Only process foreground pixels
			if (binary_mat.at<uchar>(r, c) != 255) {
				std::vector<int> adjacent_labels{};
				int upper_label = label_mat.at<int>(r - 1, c);
				if (upper_label != 0) {
					adjacent_labels.insert(adjacent_labels.end(), upper_label);
				}
				int left_label = label_mat.at<int>(r, c - 1);
				if (left_label != 0 && left_label != upper_label) {
					adjacent_labels.insert(adjacent_labels.end(), left_label);
				}
				if (adjacent_labels.size() == 0) {
					label_mat.at<int>(r, c) = nl;
					new_label.insert(new_label.end(), nl);
					nl += 1;
				}
				else {
					label_mat.at<int>(r, c) = int(*min_element(adjacent_labels.begin(), adjacent_labels.end()));
				}
				if (adjacent_labels.size() == 2) {
					equivalences.insert(equivalences.end(), adjacent_labels[0]);
					equivalences.insert(equivalences.end(), adjacent_labels[1]);
				}
			}
		}
	}
	//---------------------------------------- Pass 2:
	// Use the equivalences to merge labels
	// Create a disjoint set to manage label equivalences
	DisjointSet dis;
	dis.makeSet(new_label);
	for (int i = 0; i < equivalences.size(); i += 2) {
		dis.Union(equivalences[i], equivalences[i + 1]);
	}
	for (int r = 1; r < ROW; r++) {
		for (int c = 1; c < COL; c++) {
			if (label_mat.at<int>(r, c) != 0) {
				label_mat.at<int>(r, c) = dis.Find(label_mat.at<int>(r, c));
			}
		}
	}
	// --------------------------------------- 
	// Relabeling
	// Create a mapping from old labels to new labels
	std::unordered_map<int, int> label_map;
	for (int r = 0; r < ROW; r++) {
		for (int c = 0; c < COL; c++) {

			int label = label_mat.at<int>(r, c);

			if (label == 0) continue;

			if (label_map.count(label) == 0)
			{
				label_map[label] = label_count++;
			}

			label_mat.at<int>(r, c) = label_map[label];
		}
	}
}
int main()
{
	cv::Mat m_in = cv::imread("images//license_plate.bmp", 0);

	if (m_in.empty()) {
		std::cout << "Image load failed\n";
		return -1;
	}

	int ROW = m_in.rows;
	int COL = m_in.cols;

	cv::Mat m_out;
	cv::Mat show_t;
	cv::Mat binary_mat;
	cv::Mat display_plate;

	m_out.create(ROW, COL, CV_8UC1);
	show_t.create(ROW, COL, CV_8UC1);
	binary_mat.create(ROW, COL, CV_8UC1);
	display_plate.create(ROW, COL, CV_8UC3);

	// ################################################  
	// Step 1: Image to histogram
	// ################################################   
	float m_in_hist[256] = { 0.0 };
	Image2Histogram(m_in, m_in_hist);


	// ################################################ 
	// Step 2: Otsu's Algorithm to get t_star
	// ################################################
	int t_star = 0;
	Otsu_Algorithm(m_in_hist, t_star);


	// ################################################ 
	// Step 3: Use t_star to get binary mat
	// ################################################
	cv::threshold(m_in, binary_mat, t_star, 255, cv::THRESH_BINARY);

	// ################################################
	// Step 4: Connected component labeling
	// ################################################
	int label_count = 1;
	cv::Mat label_mat = cv::Mat::zeros(ROW, COL, CV_32S);
	Connected_Component_Labeling(binary_mat, label_mat, label_count);

	// ################################################
	// Step 5: Label mat display
	// ################################################
	// Use a random number generator(RNG) to create a color palette for the labels
	cv::RNG rng(cv::getTickCount());
	// The first color is reserved for the background (label 0), which is set to white
	std::vector<cv::Vec3b> color_plate(label_count + 1);
	color_plate[0] = cv::Vec3b(255, 255, 255);
	// Generate random colors for each label
	std::generate(color_plate.begin() + 1, color_plate.end(), [&rng]() {
		return cv::Vec3b(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
	});
	// Map each label in the label_mat to its corresponding color in the color_plate and create the display_plate
	for (int r = 0; r < ROW; r++) {
		for (int c = 0; c < COL; c++) {
			int label = label_mat.at<int>(r, c);
			display_plate.at<cv::Vec3b>(r, c) =(label? color_plate[label] : cv::Vec3b(255, 255, 255));
		}
	}
	// ################################################
	// Step 6: Show the results
	// ################################################
	show_t.setTo(255);
	// set string and variables
	std::string text = "optimal threshold = " + std::to_string(t_star);
	int font_face = cv::FONT_HERSHEY_COMPLEX;
	double font_scale = 2;
	int thickness = 2;
	int baseline;
	// get text size
	cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);

	// decide text position
	cv::Point origin;
	origin.x = (COL - text_size.width) >> 1;
	origin.y = (ROW + text_size.height) >> 1;
	cv::putText(show_t, text, origin, font_face, font_scale, cv::Scalar(0, 255, 255), thickness, 8, 0);
	// ###############################################threshold
	cv::namedWindow("threshold", cv::WINDOW_AUTOSIZE);
	cv::imshow("threshold", show_t);
	// ###############################################intput_image
	cv::namedWindow("intput_image", cv::WINDOW_AUTOSIZE);
	cv::imshow("intput_image", m_in);
	//################################################binary_image
	cv::namedWindow("binary_image", cv::WINDOW_AUTOSIZE);
	cv::imshow("binary_image", binary_mat);
	// ################################################Result of connected component labeling
	cv::namedWindow("Result of connected component labeling", cv::WINDOW_AUTOSIZE);
	cv::imshow("Result of connected component labeling", display_plate);
	// ###############################################
	cv::waitKey(0);
	return 0;
}

