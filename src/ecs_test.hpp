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
	std::string name = "default";
};

class STest : public ecs::BaseSystem{
public:
	STest(ecs::Engine& engine){
		ecs = engine;
		testComponents = ecs::ComponentVectorCast<CTest>(ecs.getComponents("test"));
	}
	~STest(){}
    void update(){
    	ecs::Entity e =  ecs.createEntity("test");
    	ecs.addComponent(e, "test");
    }
	void init(){}
	void destroy(){}
	std::shared_ptr<ecs::ComponentVector<CTest>> testComponents;
	ecs::Engine ecs;
}; //STest

void Engine_test(){
	ecs::Engine engine;
	CTest testComp;
	engine.newComponentType("test", testComp);
	engine.createEntity("test");
	auto testSystem = std::make_shared<STest>(engine);
	engine.addSystem("test", testSystem);

}

void ComponentVector_test(){
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

void ComponentContainers_test(){
	CTest testComp;
	ecs::ComponentContainers compContainers;
	compContainers.add("test", testComp);
	auto testVec = ecs::ComponentVectorCast<CTest>(compContainers.get("test"));
	testVec->init(200);
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

}

#endif /* ECS_TEST_HPP_ */
