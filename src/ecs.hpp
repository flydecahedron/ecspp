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
#include <cassert>
#include <algorithm>//find_if for entities
#include <cassert>
#include <exception>
#include <unordered_map> //Comonent Mask map
#include <initializer_list>
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

/*!\class CompareFirst
 * \brief unary predicate returns true if pair.first equals the passed in value.
 * Meant to be used with find_if and a vector of pairs
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
 * \brief unary predicate returns true if pair.second equals the passed in value.
 * Meant to be used with find_if and a vector of pairs
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

/*!\class BaseContainer
 * Base class for component container implementations
 */
class BaseContainer{
public:
	virtual ~BaseContainer(){};
	virtual void add(Entity& entity) = 0;
	virtual void remove(Entity& entity) = 0;
	virtual void init(const int& maxComponents) = 0;
};

/*!\class ComponentVector
 * generic component vector container
 */
template <class Component>
class ComponentVector : BaseContainer {
public:
	ComponentVector(){}
	~ComponentVector(){}

	/*!\fn add
	 * \brief add passed in entity to vector with default component
	 */
	void add(Entity& entity){
		Component component;
		components.emplace_back(entity, component);
	}
	//! add overload that takes a reference of a component
	void add(Entity& entity, Component& component){
		components.emplace_back(std::make_pair(entity,component));
	}
	/*!\fn remove
	 * removes the entity and its component from the vector
	 */
	void remove(Entity& entity){
		if(!components.empty()){
			auto it = std::find_if(components.begin(), components.end(),CompareFirst<Entity,Component>(entity));
			std::swap(components[it - components.begin()], components.back());
			components.pop_back();
		}
	}
	/*!\fn get
	 * returns the component associated with the passed in entity
	 */
	Component get(Entity& entity){
		auto it = std::find_if(components.begin(), components.end(),CompareFirst<Entity,Component>(entity));
		return it->second;
	}
	/*!\fn init
	 * sets the capacity of the vector
	 */
	void init(const int& maxComponents){
		components.reserve(maxComponents);
	}
	std::vector<std::pair<Entity, Component>> components;
//TODO finish interface so logging can be done with components private
};// ComponentVector

/*!\class ComponentContainers
 *
 */
class ComponentContainers{
public:
	/*!\fn add
	 * \brief makes a new container for the passed in type.
	 */
	template <class Component>
	void add(const std::string& name, Component& type){
		types[name] = bitCounter;
		++bitCounter;
		ComponentVector<Component> newCompVec;
		//TODO pointers to component vectors pointers.emplace_back(componentName, ptr);
	}
	/*!\fn get
	 * returns void ptr to component data structure related to the passed in name
	 */
	std::shared_ptr< BaseContainer > get(const std::string& name){
		return pointers[name];
	}
	/*!\fn getBitMask
	 *
	 */
	ComponentMask getBitMask(std::initializer_list<std::string> names){
		ComponentMask CMask;
		for(auto it : names){
			unsigned short int bit = types[it];
			CMask.set(bit , true);
		}
		return CMask;
	}
private:
	std::unordered_map<std::string, std::shared_ptr< BaseContainer> > pointers;
	std::unordered_map<std::string, unsigned short int> types;
	unsigned short int bitCounter = 1; //bit '0' is alive flag for entities

};

/*!\class BaseSystem
 * \brief class that all systems should inherit from
 */
class BaseSystem{
public:
	virtual ~BaseSystem(){};
	virtual void update() = 0;
	virtual void init() = 0;
	virtual void destroy() = 0;
protected:
	std::unordered_map<std::string, std::shared_ptr< BaseContainer> > components;
}; // BaseSystem Interface

/**\class Systems
 *
 */
class Systems{
public:
	/*!
	 *
	 */
	template<class T>
	void add(std::string name, std::shared_ptr<T> systemPtr){
		for(auto const& it : pointers){
			assert(it.first != name);
		}
		pointers[name] = std::static_pointer_cast<BaseSystem>(systemPtr);
	}

	void init(){
		for(auto const& it : pointers){
			it.second->init();
		}
	}

	void update(){
		for(auto const& it : pointers){
			it.second->update();
		}
	}

	void destroy(){
		for(auto const& it : pointers){
			it.second->destroy();
		}
		pointers.clear();
	}
private:
	std::unordered_map<std::string, std::shared_ptr<BaseSystem>> pointers;

}; // Systems class

/*!\class Entities
 * Contains all entities and manages add, remove, create, destroy functionalities.
 * Also, is responsible for adding components to the component mask of entities
 */
class EntityManager{
public:
	EntityManager(ComponentContainers& compContainers){
		compContainers = componentContainers;
		entities.reserve(maxEntities);
	}
	/*!\fn create
	 *
	 */
	Entity create(){
		Entity entity;
		if(deletedEntities.empty()){
			entity = entityCount;
			++entityCount;
		}
		else{
			entity = deletedEntities.front();
			deletedEntities.pop();
		}
		ComponentMask CMask;
		CMask.set(0, true);//!first bit in CMask is "alive" bit
		entities[entity] = CMask;
		return entity;
	}

	Entity create(std::initializer_list<std::string>& components){
		Entity e = create();
		addEntity(e, components);
		return e;
	}
	/*!\fn remove
	 * sets the "alive" flag for the passed in entity to false.
	 */
	void remove(Entity& entity){
		deletedEntities.push(entity);
		entities[entity] = 0;
	}
	//! overload that takes variable amounts of entities
	void remove(std::initializer_list<Entity>& entities){
		for(Entity e : entities){
			deletedEntities.push(e);
			this->entities[e] = 0;
		}
	}
private:
	/*!\fn addEntity
	 *
	 */
	void addEntity(Entity& entity){
		if(deletedEntities.empty()){
			entity = entityCount;
			++entityCount;
		}
		else{
			entity = deletedEntities.front();
			deletedEntities.pop();
		}
		ComponentMask CMask;
		CMask.set(0, true);//!first bit in CMask is "alive" bit
		entities[entity] = CMask;
	}
	//! addEntity overload to take multiple components to mask entity with while adding
	void addEntity(Entity& entity, std::initializer_list<std::string>& components){
		if(deletedEntities.empty()){
			entity = entityCount;
			++entityCount;
		}
		else{
			entity = deletedEntities.front();
			deletedEntities.pop();
		}
		ComponentMask CMask;
		CMask.set(0, true);//!first bit in CMask is "alive" bit
		CMask | componentContainers.getBitMask(components);
		entities[entity] = CMask;
	}
	/*!\fn maskEntity
	 * 'assigns' components to an entity by ORing its component mask with given components
	 */
	void maskEntity(Entity& entity, ComponentMask& CMask){
		entities[entity] | CMask;
	}
	//! maskEntity overload to take names of components
	void maskEntity(Entity& entity, std::initializer_list<std::string>& components){
		entities[entity] |componentContainers.getBitMask(components);
	}

	std::vector<ComponentMask> entities; //! index = entity id. Value = component mask
	std::queue<Entity> deletedEntities; //! available indices to use in entities vector
	Entity entityCount = 0; //! Max number of entity ids used so far
	ComponentContainers componentContainers;
};


}// ecs namespace


#endif /* ECS_HPP_ */
