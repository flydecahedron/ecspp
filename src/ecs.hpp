/*! \mainpage ecspp Index page
 * \section intro_sec Introduction
 * This will be the intro eventually probs
 * \section install_sec Installation
 * \subsection step1 Step 1: I'm a masochist and use Eclipse CDT
 * eff me right?
 * \subsection step2 Step 2: It's really not that bad
 * says no one on the internet
 * \subsectin step3 Step 3: Coming out
 * okay I'm actually enjoying it and hate visual studio D:
 */

/*! \file ecs.hpp
 *
 */
#ifndef ECS_HPP_
#define ECS_HPP_

#include <vector>
#include <queue> // available entity ids and idices
#include <typeindex> // to store component types
#include <bitset> // component bitmasking
#include <functional> //make_shared for creating new CMap
#include <memory> //shared_ptr for CMaps
#include <cassert> // static_assert component types
#include <algorithm>//find_if for entities
#include <cassert>
#include <exception>
/*!\namespace ecs
 * \brief entity component system implementation where entities, components, and systems
 * are held in objects of those names respectively.
 *
 * Entities are a combination of a uuid and their component mask which is std::bitset. A component's
 * "bit" is determined by its index in the component type vector.
 *
 * boost flat_map is used for each different component type for contiguous memory and fast
 * iteration. Components are expected to hold most/all of what a system will need to update.
 * Components have to inherit from IComponent.
 *
 * Systems need to be derived from ISystem and define update, init, and destroy functions (messaging
 * functions may be added later). They will be updated in the order that they are added to the "Systems"
 * object. The "Systems" object will contain all of the systems in a vector.
 *
 */
namespace ecs{

//max expected number of entities and components to pass into container.reserve()
const int maxEntities = 1000; // amount of entities to reserve for vector<Entity>
const int maxComponentTypes = 64;
typedef unsigned int Entity;
typedef std::bitset<maxComponentTypes> ComponentMask;
typedef std::pair<Entity, ComponentMask> EntityData;
/*!
 * \brief vectorOfPairs is used to hold entity ids and their component masks
 * \tparam Key entity id
 * \tparm T component mask
 */
template <typename Key, class T>
using vectorOfPairs = std::vector<std::pair<Key, T>>;
/*!\class CompareFirst
 * \brief unary predicate returns true if pair.first equals the passed in value.
 * Meant to be used with find_if and vectorOfPairs.
 * http://stackoverflow.com/questions/12008059/find-if-and-stdpair-but-just-one-element
 */
template <typename K, typename T>
struct CompareFirst
{
  CompareFirst(K key) : m_key(key) {}
  bool operator()(const std::pair<K,T>& element) const {
    return m_key == element.first;
  }
  private:
    K m_key;
};
/*!\class CompareSecond
 * \brief unary predicate returns true if pair.first equals the passed in value.
 * Meant to be used with find_if and vectorOfPairs.
 * http://stackoverflow.com/questions/12008059/find-if-and-stdpair-but-just-one-element
 */
template <typename K, typename T>
struct CompareSecond
{
  CompareSecond(T t) : m_t(t) {}
  bool operator()(const std::pair<K,T>& element) const {
    return m_t == element.second;
  }
  private:
    T m_t;
};
/*!\class ComponentManager
 *
 */
class ComponentManager{
public:
	template <class T>
	void add(const std::string& componentName, T& dataStruct){
		T * ptr = dataStruct;
		pointers.emplace_back(componentName, ptr);
	}

	void* get(const std::string& componentName){
		auto it = std::find_if(pointers.begin(), pointers.end(), CompareFirst<std::string,void*>(componentName));
		return it->second;
	}
private:
	vectorOfPairs<const std::string, void*> pointers;

};
/*!\class Entities
 * \brief contains all entities and manages add, remove, create, destroy functionalities
 * Also, is responsible for adding components to the component mask of entities
 */
/*!\class Entities
 *
 */
class Entities{
public:
	Entities(ComponentManager cm){
		cm = componentManager;
	}

	Entity create(){
		Entity entity;
		if(deletedEntities.empty()){
			entity = entityCount++;
		}
		else{
			entity = deletedEntities.front();
			deletedEntities.pop();
		}
		ComponentMask CMask;
		CMask.set(0);
		entities[entity] = CMask;
		return entity;
	}
	void remove(Entity entity){
		deletedEntities.push(entity);
		/** TODO
		 * use ComponentMask to remove related components
		 */
	}
private:
	std::vector<ComponentMask> entities; //! index = entity id. Value = component mask
	std::queue<Entity> deletedEntities; //! available indices to use in entities vector
	Entity entityCount = 0; //! Max number of entity ids used so far
	ComponentManager componentManager;
};
/*!\class BaseSystem
 * \brief class that all systems should inherit from
 */
class BaseSystem{
public:
	BaseSystem(){};
	virtual ~BaseSystem(){};
	virtual void update();
	virtual void init();
	virtual void destroy();

}; // BaseSystem Interface

class TestSystem : public BaseSystem{
public:
	TestSystem(){};
	~TestSystem(){};
	void update(){};
	void init(){};
	void destroy(){};
};

/**\class Systems
 *
 */
class SystemManager{
public:
	template <class T>
	void add(T systemObject){
		systems.push_back(std::unique_ptr<BaseSystem>(new T));
	}

	void initAll(){
		for(auto const& system : systems){
			system->init();
		}
	}

	void updateAll(){
		for(auto const& system : systems){
			system->update();
		}
	}

	void destroyAll(){
		for(auto const& system : systems){
			system->destroy();
		}
		systems.clear();
	}
	void test(){
		/*
	}
		ecs::TestSystem testS;
		systems.push_back(testS);
		for(BaseSystem test : systems){
			test.update();
		}*/
	}
private:
	std::vector<std::unique_ptr<BaseSystem>> systems;

}; // Systems class

}// ecs namespace



#endif /* ECS_HPP_ */
