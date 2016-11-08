//============================================================================
// Name        : ecs.cpp
// Author      : Eric Schmiedeberg
// Version     :
// Copyright   : Copyright 2016
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "ecs.hpp"
#include "flat_map.hpp"
using namespace std;

int main() {

	boost::container::flat_map<int, int> map;
	map.reserve(20);
	cout << "FUCK YOU" << endl; // prints FUCK YOU
	return 0;
}
