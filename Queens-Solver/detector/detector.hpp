#pragma once

#include <opencv2/opencv.hpp>

#define DO_BLUR   (1 << 0)
#define DO_DILATE (1 << 1) 

namespace detector
{
	extern auto get_image_edges(
		const cv::Mat& image,
		unsigned __int32 mask ) -> cv::Mat;

	extern auto detect_grid( 
		const cv::Mat& image ) -> cv::Rect;

	extern auto get_rows( 
		const cv::Mat& grid_edges ) -> std::vector<cv::Vec4i>;

	extern auto get_columns( 
		const cv::Mat& grid_edges ) -> std::vector<cv::Vec4i>;

	extern auto get_color_matrix( 
		const cv::Mat& grid, 
		std::vector<cv::Vec4i> rows, 
		std::vector<cv::Vec4i> columns ) -> std::vector<std::vector<short>>;
}
