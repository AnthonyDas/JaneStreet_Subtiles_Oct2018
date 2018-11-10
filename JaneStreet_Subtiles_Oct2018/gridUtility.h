#pragma once

#include <vector>

// Print the grid to std::cout
void print_grid(const std::vector<std::vector<int> > &);

// Count how many tiles are of type val
int count(const std::vector<std::vector<int> > &, const int &);


void min_distance(std::vector<std::vector<int> > &, const size_t &, const size_t &, const int &, const int &);

// Determines whether it is possible to connect the existing tiles of type val.
// This function is lenient. It will give false positives but all negatives are correct.
bool connection_possible(const std::vector<std::vector<int> > &, const int &);

// Calcuates the x and y span of tile type val
void span(const std::vector<std::vector<int> > &, const int &, int &, int &,
	int &, int &, int &, int &);

// Creates a test grid. This is a smaller grid containing the shape given by tiles of type val.
void create_test_grid(const std::vector<std::vector<int> > &, std::vector<std::vector<int> > &, const int &);

// Performs a rotation of test_grid and saves to rotated.
void rotate(const std::vector<std::vector<int> > &, std::vector<std::vector<int> > &);

// Performs a reflection (vertically) of test_grid and saves to reflected.
void reflect_vertical(std::vector<std::vector<int> > &, std::vector<std::vector<int> > &);

// Performs a reflection (horizontally) of test_grid and saves to reflected.
void reflect_horizontal(std::vector<std::vector<int> > &, std::vector<std::vector<int> > &);

// Checks whether the current grid could possibly be valid.
bool is_valid(std::vector<std::vector<int> > &, const int &);

// Calculates the minimum number of tiles of type val required to achive a given x and y span.
int min_squares(const int &, const int &);