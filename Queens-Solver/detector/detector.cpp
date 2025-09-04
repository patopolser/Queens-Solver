#include "detector.hpp"

auto detector::detect_grid( const cv::Mat& image ) -> cv::Rect
{
	/* Detect the bounding box of the grid in the image */

	auto edges = get_image_edges( image, DO_BLUR );
	
	/* Find contours in the edge-detected image */

	auto contours = std::vector< std::vector<cv::Point>>( ); cv::findContours( edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE );

	auto max_score = 0.0;
	auto grid = cv::Rect { };

	for ( const auto& contour : contours )
	{
		auto bb = cv::boundingRect( contour );
		auto area = bb.width * bb.height;

		/* Filter non square-like bounding boxes */

		auto ratio = ( double ) bb.width / ( double ) bb.height;

		if ( ratio < 0.7 || ratio > 1.3 )
			continue;

		/* Score the bounding box based on its area and how close it is to a square */

		auto score = area * ( 1 - std::abs( 1 - ratio ) );

		if ( score > max_score )
		{
			auto region = cv::Mat( ); cv::cvtColor( image( bb ), region, cv::COLOR_BGR2GRAY );

			auto mean_gray = cv::Scalar( ), stddev_gray = cv::Scalar( );
			cv::meanStdDev( region, mean_gray, stddev_gray );

			/* Filter regions that not have enough variance in color */

			if ( std::pow( stddev_gray[0], 2 ) < 50.0 )
				continue;

			max_score = score;
			grid = bb;
		}
	}

	return grid;
}

auto detector::get_image_edges( const cv::Mat& image, unsigned __int32 mask ) -> cv::Mat
{
	/* Preprocess the image to extract edges using Canny edge detection */

	auto img = image.clone( );

	/* Convert to grayscale */

	cv::cvtColor( img, img, cv::COLOR_BGR2GRAY );

	/* Apply Gaussian blur to reduce noise and improve edge detection */

	if ( mask & DO_BLUR )
		cv::GaussianBlur( img, img, cv::Size( 3, 3 ), 0 );

	/* Detect edges o_O */
	 
	cv::Canny( img, img, 50, 150 );

	/* Dilate the edges to strengthen them */

	if ( mask & DO_DILATE )
	{
		auto kernel = cv::getStructuringElement( cv::MORPH_RECT, cv::Size( 3, 3 ) );
		cv::dilate( img, img, kernel );
	}

	return img;
}

auto detector::get_rows( const cv::Mat& grid_edges ) -> std::vector< cv::Vec4i >
{
	/* Detect horizontal lines using Hough Transform */

	auto lines = std::vector< cv::Vec4i >( ); cv::HoughLinesP( grid_edges, lines, 1, CV_PI / 180, 50, 50.0, 50.0 );
	auto horizontal_lines = std::vector< cv::Vec4i >( );

	for ( const auto& line : lines )
	{
		auto y1 = line[1], y2 = line[3];

		if ( std::abs( y2 - y1 ) < 5 )
			horizontal_lines.push_back( line );
	}

	std::sort( 
		horizontal_lines.begin( ), horizontal_lines.end( ),
		[] ( const cv::Vec4i& a, const cv::Vec4i& b ) { return a[1] < b[1]; } );

	auto rows = std::vector<cv::Vec4i>( );

	/* Here, we merge lines that are actually the same but were detected as different. */
	/* e.g. if two lines are within 10 pixels vertically, we consider them the same line */

	for ( const auto& line : horizontal_lines )
	{
		if ( rows.empty( ) || std::abs( line[1] - rows.back( )[1] ) > 10 )
			rows.push_back( line );
		else
		{
			auto prev = rows.back( );
			rows[rows.size( ) - 1][0] = std::min( prev[0], line[0] );
			rows[rows.size( ) - 1][2] = std::max( prev[2], line[2] );
		}
	}

	return rows;
}

auto detector::get_columns( const cv::Mat& grid_edges ) -> std::vector< cv::Vec4i >
{
	/* Detect vertical lines using Hough Transform */

	auto lines = std::vector< cv::Vec4i >( ); cv::HoughLinesP( grid_edges, lines, 1, CV_PI / 180, 50, 50.0, 50.0 );
	auto vertical_lines = std::vector< cv::Vec4i >( );

	for ( const auto& line : lines )
	{
		auto x1 = line[0], x2 = line[2];

		if ( std::abs( x2 - x1 ) < 5 )
			vertical_lines.push_back( line );
	}

	std::sort( 
		vertical_lines.begin( ), vertical_lines.end( ),
		[] ( const cv::Vec4i& a, const cv::Vec4i& b ) { return a[0] < b[0]; } );

	auto columns = std::vector<cv::Vec4i>( );

	/* Here, we merge lines that are actually the same but were detected as different. */
	/* e.g. if two lines are within 10 pixels horizontally, we consider them the same line */

	for ( const auto& line : vertical_lines )
	{
		if ( columns.empty( ) || std::abs( line[0] - columns.back( )[0] ) > 10 )
			columns.push_back( line );
		else
		{
			auto prev = columns.back( );

			columns[columns.size( ) - 1][1] = std::min( prev[1], line[1] );
			columns[columns.size( ) - 1][3] = std::max( prev[3], line[3] );
		}
	}

	return columns;
}

auto detector::get_color_matrix(
	const cv::Mat& grid, 
	std::vector<cv::Vec4i> rows, 
	std::vector<cv::Vec4i> columns ) -> std::vector<std::vector<short>>
{
	/* Create a matrix representing the colors in each cell of the grid */

	auto color_map = std::unordered_map<int, short>( );
	auto matrix = std::vector<std::vector<short>>( );

	for ( int r = 0; r < rows.size( ) - 1; r++ )
	{
		auto matrix_row = std::vector<short>( );

		for ( int c = 0; c < columns.size( ) - 1; c++ )
		{
			/* Get the cell between the current row and column lines */

			auto y1 = rows[r][1], y2 = rows[r + 1][1];
			auto x1 = columns[c][0], x2 = columns[c + 1][0];

			auto cell = grid( cv::Rect( x1, y1, x2 - x1, y2 - y1 ) );
			auto cell_color = cell.at<cv::Vec3b>( ( x2 - x1 ) / 2, ( y2 - y1 ) / 2 );

			/* Flexing bitwise skills o_O */

			auto color_key = ( cell_color[0] << 16 ) | ( cell_color[1] << 8 ) | ( cell_color[2] );

			if ( color_map.find( color_key ) == color_map.end( ) )
				color_map[color_key] = ( short ) color_map.size( ) + 1;

			matrix_row.push_back( color_map[color_key] );
		}

		matrix.push_back( matrix_row );
	}

	return matrix;
}