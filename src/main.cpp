/*! \file main.cpp
 * \brief entry point for the program
 *
 */

#include <iostream>

#include "flat_map.hpp"
#include <iostream>
#include "ecs.hpp"
using namespace std;

void addCTest(){
	ecs::Components components;
	struct CTest : ecs::IComponent{
		~CTest(){};
		std::string name = "test";
	};
	CTest test;
	ecs::flatMap<ecs::Entity, CTest> testMap;
	//components.add("test",test);
}
int main() {
	return 0;
}
