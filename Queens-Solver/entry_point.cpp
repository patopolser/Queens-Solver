#include "screen/screen.hpp"
#include "detector/detector.hpp"
#include "solver/solver.hpp"

#define DELAY 0 /* Delay between clicks */
#define CLICKS 1 /* Number of clicks per cell */

auto __stdcall main( ) -> __int32
{
	for ( ; "github.com/patopolser" ; )
	{
		std::cout << "Queens-Solver - github.com/patopolser" << std::endl;
		std::cout << "Press END to exit or ENTER to solve..." << std::endl;

		for ( ; !GetAsyncKeyState( VK_RETURN ); Sleep( 100 ) )
			if ( GetAsyncKeyState( VK_END ) )
				return 0;

		system( "cls" );

		auto screen = screen::capture_screen_mat( );
		auto grid_bb = detector::detect_grid( screen );

		if ( grid_bb.empty( ) )
			continue;

		auto grid = screen( grid_bb );
		auto grid_edges = detector::get_image_edges( grid, DO_BLUR | DO_DILATE );

		auto rows = detector::get_rows( grid_edges );
		auto columns = detector::get_columns( grid_edges );

		if ( rows.size( ) != columns.size( ) || rows.size( ) < 4 )
			continue;

		auto color_matrix = detector::get_color_matrix( grid, rows, columns );

		std::cout << "Detected grid size: " << rows.size( ) << "x" << columns.size( ) << std::endl;

		for ( const auto& row : color_matrix )
		{
			for ( const auto& cell : row )
				std::cout << cell << " ";
			std::cout << std::endl;
		}

		auto start = std::chrono::steady_clock::now( );

		solver::solve_on_screen( grid_bb, rows, columns, solver::solve_game( color_matrix ), CLICKS, DELAY );

		auto end = std::chrono::steady_clock::now( );

		std::cout << "Solved in " << ( std::chrono::duration<double> ) ( end - start ) << std::endl;
	}

	return 0;
}