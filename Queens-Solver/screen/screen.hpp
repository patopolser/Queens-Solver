#pragma once

#include <opencv2/opencv.hpp>
#include <windows.h>


namespace screen
{
	extern auto capture_screen_mat( ) -> cv::Mat;
}