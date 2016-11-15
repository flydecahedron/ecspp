/*! \file main.cpp
 * \brief entry point for the program
 *
 */

#include <iostream>

#include "flat_map.hpp"
#include <iostream>

#include "ecs.hpp"
using namespace std;
struct CTest{
	std::string name = "it worked bitch";
};

/*! TESTS
 *
 */
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
};
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
