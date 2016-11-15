/*! \file main.cpp
 * \brief entry point for the program
 *
 */

#include <iostream>

#include "flat_map.hpp"
#include <iostream>

#include "ecs.hpp"
#include <algorithm>
#include <utility>
/*! TESTS
 *
 */
struct CTest{
	std::string name = "it worked bitch";
};

class STest : public ecs::BaseSystem{
public:
	STest(){}
	~STest(){}
    void update(){}
	void init(){}
	void destroy(){}
	void removeEntity(ecs::Entity& entity){}
	void addEntity(){}
	std::vector<CTest> testComponents;
}; //STest
/*!\TestContainer
 * generic component vector container
 */
template <class C>
class TestContainer : ecs::BaseContainer {
public:
	TestContainer(){
		C first;
	}
	~TestContainer(){}
	void add(ecs::Entity& entity){
		C component;
		components.emplace_back(entity, component);
	}
	void remove(ecs::Entity& entity){
		if(!components.empty()){
			auto it = std::find_if(components.begin(), components.end(),ecs::CompareFirst<ecs::Entity,C>(entity));
			std::swap(components[it - components.begin()], components.back());
			components.pop_back();
		}
	}
	void init(const int& maxComponents){
		components.reserve(maxComponents);
	}
	typedef C value_type;
	std::vector<std::pair<ecs::Entity, C>> components;

private:

};// TestContainer

void test_TestContainer(){
	TestContainer<CTest>* testVec = new TestContainer<CTest>;
	testVec->init(200);
	//testVec.components;
	//auto testVec = std::make_shared<TestContainer<CTest>>();
}
void test(){
	//auto stest = new STest();
	auto stest = std::make_shared<STest>();
	ecs::ComponentManager cm;
	ecs::SystemManager sm;
	ecs::EntityManager em(cm, sm);
	//TODO make a custom component container wrapper

	cm.add("test", stest->testComponents);
	// maybe this functionality is redundant???
	//sm.add(stest);
}

int main() {
	test();
	return 0;
}
