/*! \file main.cpp
 * \brief entry point for the program
 *
 */

#include <iostream>

#include "ecs.hpp"
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
class ComponentVector : ecs::BaseContainer {
public:
	ComponentVector(){}
	~ComponentVector(){}
	/*!\fn add
	 * add passed in entity to vector with default component
	 */
	void add(ecs::Entity& entity){
		C component;
		components.emplace_back(entity, component);
	}
	//! add overload that takes a reference of a component
	void add(ecs::Entity& entity, C& component){
		components.emplace_back(std::make_pair(entity,component));
	}
	/*!\fn remove
	 * removes the entity and its component from the vector
	 */
	void remove(ecs::Entity& entity){
		if(!components.empty()){
			auto it = std::find_if(components.begin(), components.end(),ecs::CompareFirst<ecs::Entity,C>(entity));
			std::swap(components[it - components.begin()], components.back());
			components.pop_back();
		}
	}
	/*!\fn get
	 * returns the component associated with the passed in entity
	 */
	C get(ecs::Entity& entity){
		auto it = std::find_if(components.begin(), components.end(),ecs::CompareFirst<ecs::Entity,C>(entity));
		return it->second;
	}
	/*!\fn init
	 * sets the capacity of the vector
	 */
	void init(const int& maxComponents){
		components.reserve(maxComponents);
	}
private:
	std::vector<std::pair<ecs::Entity, C>> components;
};// TestContainer

void test_TestContainer(){
	ComponentVector<CTest>* testVec = new ComponentVector<CTest>;
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
	ecs::ComponentManager cm;
	ecs::SystemManager sm;
	ecs::EntityManager em(cm, sm);
	//TODO make a custom component container wrapper

	cm.add("test", stest->testComponents);
	// maybe this functionality is redundant???
	//sm.add(stest);
}

int main() {
	test_TestContainer();
	return 0;
}
