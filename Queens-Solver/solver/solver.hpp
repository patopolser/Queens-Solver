#pragma once

#include <opencv2/opencv.hpp>

#include <windows.h>
#include <random>

namespace solver
{
	extern auto solve_game( 
		const std::vector<std::vector<short>>& matrix,
		std::vector<cv::Point> solution = {},
		unsigned __int32 row = 0,
		std::vector<int> used_colors = {},
		std::vector<int> used_columns = {} ) -> std::vector<cv::Point>;

	extern auto check_queen_neighbors(
		const std::vector<cv::Point>& solution,
		unsigned __int32 row,
		unsigned __int32 column, 
		unsigned __int32 matrix_size ) -> bool;

	extern auto solve_on_screen( 
		const cv::Rect& grid_bbox, 
		const std::vector<cv::Vec4i>& rows, 
		const std::vector<cv::Vec4i>& columns, 
		const std::vector<cv::Point>& cells,
		unsigned __int32 clicks = 1,
		unsigned __int32 delay = 0) -> void;
}