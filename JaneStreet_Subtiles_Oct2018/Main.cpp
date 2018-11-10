#include <iostream>
#include <vector>
#include <set>
#include <algorithm> // min, max
#include "gridUtility.h"

// Main processing function
size_t recursive_overlay(std::vector<std::vector<int> > grid, const int &val, const int &val_test, std::set<std::vector<std::vector<int> > > &sols, const bool &log = false) {
	if (val == val_test) {
		throw std::logic_error("val and val_test the same");
	}

	std::vector<std::vector<int> > test_grid;
	create_test_grid(grid, test_grid, val_test);

	int x_span, y_span, x_min, x_max, y_min, y_max;
	span(grid, val, x_span, y_span, x_min, x_max, y_min, y_max);

	// print_grid(test_grid);

	std::set<std::vector<std::vector<int> > > locs, tests;

	std::vector<std::vector<int> > test_temp(test_grid);

	// Rotations and reflections
	for (int rot = 0; rot < 8; ++rot) {

		if (rot != 0 && rot != 4) {
			std::vector<std::vector<int> > rotated;
			rotate(test_temp, rotated);
			test_temp = rotated;
		}
		else if (rot == 4) {
			std::vector<std::vector<int> > reflected;
			reflect_vertical(test_temp, reflected);
			test_temp = reflected;
		}

		if (log) {
			std::cout << "Rotation/Reflection " << rot << std::endl;
			print_grid(test_temp);
		}

		// Check that this test grid isn't a duplicate of an earlier test.
		// Possible due to rotational/reflectional symmetry.
		size_t test_size = tests.size();
		tests.insert(test_temp);
		if (test_size == tests.size()) {
			continue;
		}

		// Span of test and existing grid must be feasible
		if (min_squares(std::max(x_span, (int)test_temp.front().size()), std::max(y_span, (int)test_temp.size())) > val) {
			continue;
		}

		// Grid offsets
		for (size_t gi = 0; gi < grid.size() - test_temp.size() + 1; ++gi) {
			for (size_t gj = 0; gj < grid.front().size() - test_temp.front().size() + 1; ++gj) {

				bool valid = true;
				int count = 0;
				std::vector<std::vector<int> > grid_temp(grid);

				// Test grid
				for (size_t ti = 0; ti < test_temp.size(); ++ti) {
					for (size_t tj = 0; tj < test_temp.front().size(); ++tj) {

						if (val > val_test) {
							// Every tile in the test grid must be in the main grid
							if (test_temp[ti][tj] == val_test) {
								if (grid_temp[gi + ti][gj + tj] == 0) {
									grid_temp[gi + ti][gj + tj] = val;
									continue;
								}
								else if (grid_temp[gi + ti][gj + tj] != val) {
									valid = false;
									break;
								}
							}

							if (test_temp[ti][tj] == -1 && grid_temp[gi + ti][gj + tj] == val) {
								++count;

								if (count > (val - val_test)) {
									valid = false;
									break;
								}
							}
						}
						else {
							// At most (val_test - val) tiles in the test grid cannot be in the grid
							if (test_temp[ti][tj] == val_test) {

								if (grid_temp[gi + ti][gj + tj] == val) {
									continue;
								}
								else if (grid_temp[gi + ti][gj + tj] != 0) {

									++count;

									if (count > (val_test - val)) {
										valid = false;
										break;
									}
								}
							}

							if (test_temp[ti][tj] != -1 && grid_temp[gi + ti][gj + tj] == val) {
								valid = false;
								break;
							}
						}

					}
				}

				if (valid) {
					valid = is_valid(grid_temp, val);
				}
				if (valid && (val > val_test)) {
					// Can we overlay back the other way
					valid = recursive_overlay(grid_temp, val_test, val, sols) > 0;
				}
				/*if (valid && (val < val_test) && (val_test > 14)) {
					valid = recursive_overlay(grid_temp, val - 1, val_test - 1) > 0;
				}*/

				if (valid && (val > val_test) && (val < 17)) {
					valid = recursive_overlay(grid_temp, val + 1, val_test + 1, sols) > 0;
				}
				if (valid) {
					size_t loc_size = locs.size();
					locs.insert(grid_temp);

					if (log && loc_size != locs.size()) {
						std::cout << "Possible location found: " << locs.size() << std::endl;
						print_grid(grid_temp);
						std::cout << std::endl;
					}

					if (val == 17) {
						size_t sol_size = sols.size();
						sols.insert(grid_temp);

						if (sol_size != sols.size()) {
							std::cout << "Possible solution found: " << sols.size() << std::endl;
							print_grid(grid_temp);
							std::cout << std::endl;
						}
					}
				}
			}
		}

	}

	if (locs.size() == 1) {
		grid = *(locs.begin());
		if (log) {
			std::cout << "Only single solution: " << locs.size() << std::endl;
			print_grid(grid);
		}
	}

	return locs.size();
}

int main() {

	std::vector<int> row(13, 0);
	std::vector<std::vector<int> > grid(13, row);

	// Mark the cells already known
	grid[11][9] = 1;
	grid[8][7] = 2;
	grid[2][8] = 3;
	grid[5][3] = 5;
	grid[4][0] = grid[4][1] = 6;
	grid[8][12] = 8;
	grid[12][0] = grid[8][2] = 9;
	grid[10][5] = grid[9][6] = grid[7][8] = 10;
	grid[11][7] = grid[8][10] = grid[8][11] = 11;
	grid[4][10] = grid[1][11] = 12;
	grid[1][3] = grid[1][4] = grid[1][5] = 13;
	grid[7][9] = grid[3][11] = grid[0][12] = 14;
	grid[9][1] = grid[11][1] = grid[12][5] = grid[11][8] = 15;
	grid[10][3] = grid[5][4] = grid[10][4] = grid[4][5] = grid[3][6] = 16;
	grid[4][2] = grid[2][7] = grid[0][7] = grid[2][9] = 17;

	// Logical reasoning:
	// Must connect existing two 9s this way otherwise would maroon the nearby 15s
	grid[8][0] = grid[8][1] = grid[9][0] = grid[10][0] = grid[11][0] = 9;

	// recursive_overlay(grid, 10, 9, sols, true);
	// Two solutions both of which imply that:
	grid[1][12] = grid[2][12] = 14;
	
	grid[12][6] = grid[12][7] = 15;

	// 14 ==>
	grid[0][5] = grid[0][6] = 13;

	// 13 ==>
	grid[0][10] = grid[0][11] = 12;

	// 15 ==>
	grid[7][10] = 14;

	grid[1][0] = 13;

	grid[5][10] = 12;

	grid[10][11] = 11;

	grid[9][9] = 10;

	grid[7][0] = 9;

	grid[2][0] = 13;

	grid[5][9] = 12;

	grid[9][11] = 11;

	grid[8][9] = 10;

	grid[7][1] = 9;

	grid[8][5] = 16;

	grid[3][8] = grid[3][9] = 3;

	print_grid(grid);

	std::set<std::vector<std::vector<int> > > sols;

	recursive_overlay(grid, 10, 9, sols, true);

	std::cout << "Finished - type something to quit" << std::endl;
	int dummy; std::cin >> dummy;
	return 0;
}
