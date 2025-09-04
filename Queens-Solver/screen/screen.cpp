#include "screen.hpp"

auto screen::capture_screen_mat( ) -> cv::Mat
{
	/* Capture the entire screen and return it as a cv::Mat */

	auto context = GetDC( nullptr );

	auto compatible_context = CreateCompatibleDC( context );

	auto width = GetDeviceCaps( context, HORZRES );
	auto height = GetDeviceCaps( context, VERTRES );

	auto bit_map = CreateCompatibleBitmap( context, width, height );
	auto old_bit_map = ( HBITMAP__* ) SelectObject( compatible_context, bit_map );

	BitBlt( compatible_context, 0, 0, width, height, context, 0, 0, SRCCOPY );

	auto bit_map_info = tagBITMAPINFOHEADER { };

	bit_map_info.biSize = sizeof( tagBITMAPINFOHEADER );
	bit_map_info.biWidth = width;
	bit_map_info.biHeight = -height;
	bit_map_info.biPlanes = 1;
	bit_map_info.biBitCount = 24;
	bit_map_info.biCompression = BI_RGB;
	bit_map_info.biSizeImage = 0;
	bit_map_info.biClrUsed = 0;
	bit_map_info.biClrImportant = 0;

	auto screen_mat = cv::Mat( height, width, CV_8UC3 );
	GetDIBits( compatible_context, bit_map, 0, height, screen_mat.ptr(), ( tagBITMAPINFO* ) &bit_map_info, DIB_RGB_COLORS );

	SelectObject( compatible_context, old_bit_map );
	DeleteDC( compatible_context );
	ReleaseDC( nullptr, context );
	DeleteObject( bit_map );

	return screen_mat;
}