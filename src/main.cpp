/*! \file main.cpp
 * \brief entry point for the program
 *
 */

#include <iostream>

#include "ecs.hpp"
#include "flat_map.hpp"
using namespace std;

int main() {

	boost::container::flat_map<int, int> map;
	map.reserve(20);
	return 0;
}
