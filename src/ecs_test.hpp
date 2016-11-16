/*
 * ecs_test.hpp
 *
 *  Created on: Nov 15, 2016
 *      Author: eroc
 */

#ifndef ECS_TEST_HPP_
#define ECS_TEST_HPP_

#include "ecs.hpp"

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


void test_ComponentVector(){
	ecs::ComponentVector<CTest>* testVec = new ecs::ComponentVector<CTest>;
	testVec->init(200); //reserve 200

	//add enough to double capacity
	//uses default component
	for(int i = 0; i < 201; i++){
		ecs::Entity e = i;
		testVec->add(e);
	}
	//remove all entities just added
	for(int i = 0; i < 201; i++){
		ecs::Entity e = i;
		testVec->remove(e);
	}
	//test overload of add
	for(int i = 0; i < 21; i++){
		ecs::Entity e = i;
		CTest test;
		test.name = std::to_string(i);
		testVec->add(e,test);
	}
	//print 'name' variable of all components
	for(std::pair<ecs::Entity, CTest> comp : testVec->components){
		std::cout << comp.second.name << std::endl;
	}
	std::cout << "Capacity" << std::endl;
	std::cout << testVec->components.capacity() << std::endl;
	std::cout << "Size" << std::endl;
	std::cout << testVec->components.size() << std::endl;

}

void test(){
	//auto stest = new STest();
	auto stest = std::make_shared<STest>();
	ecs::ComponentContainers cm;
	ecs::SystemManager sm;
	ecs::EntityManager em(cm, sm);
	//TODO make a custom component container wrapper

	cm.add("test", stest->testComponents);
	// maybe this functionality is redundant???
	//sm.add(stest);
}

#endif /* ECS_TEST_HPP_ */
