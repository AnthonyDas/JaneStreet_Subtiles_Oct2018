#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <algorithm> // min, max, sort
#include <sstream>

#include "logger.h"

void print_grid(const std::vector<std::vector<int> > &grid) {
	std::ostringstream s;
	s << std::endl;
	for (int i = 0; i < grid.size(); ++i) {
		for (int j = 0; j < grid[i].size(); ++j) {
			// s << std::setw(3) << std::setfill(' ') << grid[i][j] << " ";
			s << std::setw(2) << std::setfill('0') << grid[i][j] << " ";
		}
		s << std::endl;
	}
	s << std::endl;
	LOG_INF(s.str());
}

int count(const std::vector<std::vector<int> > &grid, const int &val) {
	int total_count = 0;
	for (int i = 0; i < grid.size(); ++i) {
		for (int j = 0; j < grid.front().size(); ++j) {
			if (grid[i][j] == val) {
				++total_count;
			}
		}
	}
	return total_count;
}


void min_distance(std::vector<std::vector<int> > &grid, const int &i, const int &j, const int &val, const int &dist) {
	if (grid[i][j] > 0 && grid[i][j] != val) {
		return;
	}
	else if (grid[i][j] == 0 || grid[i][j] == val) {
		grid[i][j] = dist;
	}
	else { // grid[i][j]  < 0
		if (dist > grid[i][j]) {
			grid[i][j] = dist;
		}
		else {
			return;
		}
	}

	if (i > 0) {
		min_distance(grid, (i - 1), j, val, (dist - 1));
	}
	if (i < grid.size() - 1) {
		min_distance(grid, (i + 1), j, val, (dist - 1));
	}
	if (j > 0) {
		min_distance(grid, i, (j - 1), val, (dist - 1));
	}
	if (j < grid.front().size() - 1) {
		min_distance(grid, i, (j + 1), val, (dist - 1));
	}
}

bool connection_possible(const std::vector<std::vector<int> > &grid, const int &val) {
	std::vector<std::pair<int, int> > locs;
	for (int i = 0; i < grid.size(); ++i) {
		for (int j = 0; j < grid.front().size(); ++j) {
			if (grid[i][j] == val) {
				locs.push_back(std::pair<int, int>(i, j));
			}
		}
	}

	for (int n = 0; n < locs.size(); ++n) {
		std::vector<std::vector<int> > copy(grid);

		min_distance(copy, locs[n].first, locs[n].second, val, -1);
		
		for (int m = 0; m < locs.size(); ++m) {
			// Every val position must be within val steps away from each other.
			// Check the distance and check that they are indeed connected.
			if (copy[locs[m].first][locs[m].second] < -val || copy[locs[m].first][locs[m].second] == val) {

				// print_grid(copy);
				return false;
			}
		}
	}

	return true;
}


void span(const std::vector<std::vector<int> > &grid, const int &val, int &x_span, int &y_span,
	int &x_min, int &x_max, int &y_min, int &y_max) {

	x_min = INT_MAX, x_max = 0, y_min = INT_MAX, y_max = 0;

	for (int i = 0; i < grid.size(); ++i) {
		for (int j = 0; j < grid[i].size(); ++j) {
			if (grid[i][j] == val) {
				x_min = std::min(x_min, j);
				x_max = std::max(x_max, j);

				y_min = std::min(y_min, i);
				y_max = std::max(y_max, i);
			}
		}
	}

	x_span = x_max - x_min + 1;
	y_span = y_max - y_min + 1;
}


void create_test_grid(const std::vector<std::vector<int> > &grid, std::vector<std::vector<int> > &test_grid, const int &val) {

	int x_span, y_span, x_min, x_max, y_min, y_max;
	span(grid, val, x_span, y_span, x_min, x_max, y_min, y_max);

	std::vector<int> row(x_span, 0);
	for (int i = 0; i < y_span; ++i) {
		test_grid.push_back(row);
	}

	for (int j = 0; j < x_span; ++j) {
		for (int i = 0; i < y_span; ++i) {
			if (grid[i + y_min][j + x_min] == val) {
				test_grid[i][j] = val;
			}
			else if (grid[i + y_min][j + x_min] != 0) {
				test_grid[i][j] = -1;
			}
		}
	}

	// print_grid(test_grid);
}

void rotate(const std::vector<std::vector<int> > &test_grid, std::vector<std::vector<int> > &rotated) {
	rotated.clear();

	std::vector<int> row(test_grid.size(), 0);
	for (int i = 0; i < test_grid.front().size(); ++i) {
		rotated.push_back(row);
	}

	for (int i = 0; i < test_grid.size(); ++i) {
		for (int j = 0; j < test_grid.front().size(); ++j) {
			rotated[j][test_grid.size() - 1 - i] = test_grid[i][j];
		}
	}

	// print_grid(rotated);
}

void reflect_vertical(std::vector<std::vector<int> > &test_grid, std::vector<std::vector<int> > &reflected) {
	reflected.clear();

	for (std::vector<std::vector<int> >::reverse_iterator rit = test_grid.rbegin(); rit != test_grid.rend(); ++rit) {
		reflected.push_back(*rit);
	}

	// print_grid(reflected);
}

void reflect_horizontal(std::vector<std::vector<int> > &test_grid, std::vector<std::vector<int> > &reflected) {
	reflected.clear();

	for (std::vector<std::vector<int> >::iterator it = test_grid.begin(); it != test_grid.end(); ++it) {
		std::vector<int> row;
		for (std::vector<int>::reverse_iterator rit = it->rbegin(); rit != it->rend(); ++rit) {
			row.push_back(*rit);
		}
		reflected.push_back(row);
	}

	// print_grid(reflected);
}


bool is_valid(std::vector<std::vector<int> > &grid, const int &val) {

	int x_span, y_span, x_min, x_max, y_min, y_max;
	span(grid, val, x_span, y_span, x_min, x_max, y_min, y_max);

	if ((x_span + y_span - 1) > val) {
		return false;
	}

	if (count(grid, val) > val) {
		return false;
	}

	if (!connection_possible(grid, val)) {
		return false;
	}

	return true;
}

int min_squares(int x_span, int y_span) {
	return x_span + y_span - 1;
}

int overlay(std::vector<std::vector<int> > grid, const int &val, const int &val_test, std::set<std::vector<std::vector<int> > > &sols, const bool &log = false) {
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
			LOG_INF("Rotation/Reflection ", rot, "\n");
			print_grid(test_temp);
		}

		// Check that this test grid isn't a duplicate of an earlier test.
		// Possible due to rotational/reflectional symmetry.
		int test_size = tests.size();
		tests.insert(test_temp);
		if (test_size == tests.size()) {
			continue;
		}

		// Span of test and existing grid must be feasible
		if (min_squares(std::max(x_span, (int)test_temp.front().size()), std::max(y_span, (int)test_temp.size())) > val) {
			continue;
		}

		// Grid offsets
		for (int gi = 0; gi < grid.size() - test_temp.size() + 1; ++gi) {
			for (int gj = 0; gj < grid.front().size() - test_temp.front().size() + 1; ++gj) {

				bool valid = true;
				int count = 0;
				std::vector<std::vector<int> > grid_temp(grid);

				// Test grid
				for (int ti = 0; ti < test_temp.size(); ++ti) {
					for (int tj = 0; tj < test_temp.front().size(); ++tj) {

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
					valid = overlay(grid_temp, val_test, val, sols) > 0;
				}
				/*if (valid && (val < val_test) && (val_test > 14)) {
					valid = overlay(grid_temp, val - 1, val_test - 1) > 0;
				}*/

				if (valid && (val > val_test) && (val < 17)) {
					valid = overlay(grid_temp, val + 1, val_test + 1, sols) > 0;
				}
				if (valid) {
					int loc_size = locs.size();
					locs.insert(grid_temp);

					if (log && loc_size != locs.size()) {
						LOG_INF("Possible location found: ", locs.size(), "\n");
						print_grid(grid_temp);
						LOG_INF("\n");
					}

					if (val == 17) {
						int sol_size = sols.size();
						sols.insert(grid_temp);

						if (sol_size != sols.size()) {
							LOG_INF("Possible solution found: ", sols.size(), "\n");
							print_grid(grid_temp);
							LOG_INF("\n");
						}
					}
				}
			}
		}

	}

	if (locs.size() == 1) {
		grid = *(locs.begin());
		if (log) {
			LOG_INF("Only single solution: ", locs.size(), "\n");
			print_grid(grid);
		}
	}

	return locs.size();
}

int main() {

	standard_out_and_file_log_policy policy("JaneStreet_Subtiles.log");
	logger log(policy);

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

	// overlay(grid, 10, 9, sols, true);
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

	overlay(grid, 10, 9, sols, true);

	LOG_INF("Finished - type something to quit\n");
	int dummy; std::cin >> dummy;
	return 0;
}
