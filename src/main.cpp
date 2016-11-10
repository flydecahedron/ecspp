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
	ecs::flatMap< ecs::Entity, CTest > CTestMap;
	CTestMap = components.newType("test", test);
	std::shared_ptr< ecs::flatMap< ecs::Entity, ecs::IComponent > > CTestMapPtr = components.get("test");
	ecs::Entity e;
	CTestMapPtr->emplace(e, test);
	ecs::IComponent test2 = CTestMapPtr->at(e);
	ecs::IComponent* test2Ptr = &test2;
	dynamic_cast<CTest*>(test2Ptr);
}
int main() {
	addCTest();
	return 0;
}
