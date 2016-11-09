/*! \file main.cpp
 * \brief entry point for the program
 *
 */

#include <iostream>

#include "ecs.hpp"
#include "flat_map.hpp"
#include <iostream>
using namespace std;

int main() {
	struct TestComponent{
		std::string name = "fuck";
	};
	TestComponent test;
	ecs::EntityId entity = 1;
	ecs::Components components;
	ecs::flatMap<ecs::EntityId, TestComponent> testMap = components.createComponentMap("test",test);
	testMap.emplace(entity, test);
	TestComponent test2 = testMap.at(entity); //testMap[entity]
	std::string testName = test2.name;
	cout << testName << endl;
	return 0;
}
