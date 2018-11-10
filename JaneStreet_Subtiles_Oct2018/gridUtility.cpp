#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm> // min, max
#include "gridUtility.h"

// Print the grid to std::cout
void print_grid(const std::vector<std::vector<int> > &grid) {
	std::cout << std::endl;
	for (size_t i = 0; i < grid.size(); ++i) {
		for (size_t j = 0; j < grid[i].size(); ++j) {
			// std::cout << std::setw(3) << std::setfill(' ') << grid[i][j] << " ";
			std::cout << std::setw(2) << std::setfill('0') << grid[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// Count how many tiles are of type val
int count(const std::vector<std::vector<int> > &grid, const int &val) {
	int total_count = 0;
	for (size_t i = 0; i < grid.size(); ++i) {
		for (size_t j = 0; j < grid.front().size(); ++j) {
			if (grid[i][j] == val) {
				++total_count;
			}
		}
	}
	return total_count;
}


void min_distance(std::vector<std::vector<int> > &grid, const size_t &i, const size_t &j, const int &val, const int &dist) {
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

// Determines whether it is possible to connect the existing tiles of type val.
// This function is lenient. It will give false positives but all negatives are correct.
bool connection_possible(const std::vector<std::vector<int> > &grid, const int &val) {
	std::vector<std::pair<int, int> > locs;
	for (size_t i = 0; i < grid.size(); ++i) {
		for (size_t j = 0; j < grid.front().size(); ++j) {
			if (grid[i][j] == val) {
				locs.push_back(std::pair<int, int>(i, j));
			}
		}
	}

	for (size_t n = 0; n < locs.size(); ++n) {
		std::vector<std::vector<int> > copy(grid);

		min_distance(copy, locs[n].first, locs[n].second, val, -1);

		for (size_t m = 0; m < locs.size(); ++m) {
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

// Calcuates the x and y span of tile type val
void span(const std::vector<std::vector<int> > &grid, const int &val, int &x_span, int &y_span,
	int &x_min, int &x_max, int &y_min, int &y_max) {

	x_min = INT_MAX, x_max = 0, y_min = INT_MAX, y_max = 0;

	for (size_t i = 0; i < grid.size(); ++i) {
		for (size_t j = 0; j < grid[i].size(); ++j) {
			if (grid[i][j] == val) {
				x_min = std::min(x_min, (int)j);
				x_max = std::max(x_max, (int)j);

				y_min = std::min(y_min, (int)i);
				y_max = std::max(y_max, (int)i);
			}
		}
	}

	x_span = x_max - x_min + 1;
	y_span = y_max - y_min + 1;
}

// Creates a test grid. This is a smaller grid containing the shape given by tiles of type val.
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

// Performs a rotation of test_grid and saves to rotated.
void rotate(const std::vector<std::vector<int> > &test_grid, std::vector<std::vector<int> > &rotated) {
	rotated.clear();

	std::vector<int> row(test_grid.size(), 0);
	for (size_t i = 0; i < test_grid.front().size(); ++i) {
		rotated.push_back(row);
	}

	for (size_t i = 0; i < test_grid.size(); ++i) {
		for (size_t j = 0; j < test_grid.front().size(); ++j) {
			rotated[j][test_grid.size() - 1 - i] = test_grid[i][j];
		}
	}

	// print_grid(rotated);
}

// Performs a reflection (vertically) of test_grid and saves to reflected.
void reflect_vertical(std::vector<std::vector<int> > &test_grid, std::vector<std::vector<int> > &reflected) {
	reflected.clear();

	for (std::vector<std::vector<int> >::reverse_iterator rit = test_grid.rbegin(); rit != test_grid.rend(); ++rit) {
		reflected.push_back(*rit);
	}

	// print_grid(reflected);
}

// Performs a reflection (horizontally) of test_grid and saves to reflected.
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

// Checks whether the current grid could possibly be valid.
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

// Calculates the minimum number of tiles of type val required to achive a given x and y span.
int min_squares(const int &x_span, const int &y_span) {
	return x_span + y_span - 1;
}