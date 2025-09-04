#include "solver.hpp"

auto solver::solve_game(
	const std::vector<std::vector<short>>& matrix,
	std::vector<cv::Point> solution,
	unsigned __int32 row,
	std::vector<int> used_colors,
	std::vector<int> used_columns ) -> std::vector<cv::Point>
{
	/* Simple backtracking algorithm to solve the N - Queens problem with additional color constraints */

	if ( row == matrix.size() )
		return solution;
	
	for ( int column = 0; column < matrix.size( ); column++ )
	{
		auto color = matrix[row][column];

		/* Check if the column or color is already used */

		if ( std::find( used_columns.begin( ), used_columns.end( ), column ) != used_columns.end( ) )
			continue;

		/* Check if the color is already used */

		if ( std::find( used_colors.begin( ), used_colors.end( ), color ) != used_colors.end( ) )
			continue;

		/* Check if the new queen at( row, column ) is attacking any existing queens in the solution */

		if ( !check_queen_neighbors( solution, row, column, ( unsigned __int32 ) matrix.size( ) ) )
			continue;

		solution.push_back( cv::Point( column, row ) );
		used_columns.push_back( column );
		used_colors.push_back( color );

		auto result = solve_game( matrix, solution, row + 1, used_colors, used_columns );

		if ( !result.empty( ) )
			return result;

		/* Backtrack o_O */ 

		solution.pop_back( );
		used_columns.pop_back( );
		used_colors.pop_back( );
	}

	return {};
}

auto solver::check_queen_neighbors(
	const std::vector<cv::Point>& solution,
	unsigned __int32 row,
	unsigned __int32 column,
	unsigned __int32 matrix_size ) -> bool
{
	/* Check if the new queen at( row, column ) is attacking any existing queens in the solution */

	for ( const auto& x_diagonal : { -1, 0, 1 } )
	{
		for ( const auto& y_diagonal : { -1, 0, 1 } )
		{
			if ( x_diagonal == 0 && y_diagonal == 0 )
				continue;

			auto x = column + x_diagonal;
			auto y = row + y_diagonal;

			if ( 
				x >= 0 && x < matrix_size && 
				y >= 0 && y < matrix_size )
			{
				if ( std::find( solution.begin( ), solution.end( ), cv::Point( x, y ) ) != solution.end( ) )
					return false;

				x += x_diagonal;  y += y_diagonal;
			}
		}
	}

	return true;
}

auto solver::solve_on_screen(
	const cv::Rect& grid_bbox,
	const std::vector<cv::Vec4i>& rows,
	const std::vector<cv::Vec4i>& columns,
	const std::vector<cv::Point>& cells,
	unsigned __int32 clicks,
	unsigned __int32 delay) -> void
{
	/* Simulate mouse clicks on the screen to place queens in the solution cells */

	auto get_random_value = [] ( float min, float max ) -> float
	{
		std::random_device random_device;
		std::mt19937 mt_19937( random_device( ) );
		return ( float ) std::uniform_real_distribution<>( ( float ) min, ( float ) max )( mt_19937 );
	};

	for ( const auto& cell : cells )
	{
		/* Calculate the center of the cell in screen coordinates */

		auto center_x = ( 2 * grid_bbox.x + ( columns[cell.x][0] + columns[cell.x + 1][0] ) ) / 2;
		auto center_y = ( 2 * grid_bbox.y + ( rows[cell.y][1] + rows[cell.y + 1][1] ) ) / 2;

		SetCursorPos( center_x, center_y );

		for ( unsigned __int32 i = 0; i < clicks; ++i )
		{
			auto rand_offset = get_random_value( 0.1f, 0.7f );

			mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
			if ( delay > 0 ) Sleep( ( unsigned long ) ( delay * rand_offset ) );
			mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
			if ( delay > 0 ) Sleep( ( unsigned long ) ( delay * ( 1.f - rand_offset ) ) );
		}
	}
}