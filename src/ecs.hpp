/*! \mainpage ecspp Index page
 * \section intro_sec Introduction
 * This will be the intro eventually probs
 * \section install_sec Installation
 * \subsection step1 Step 1: I'm a masochist and use Eclipse CDT
 * eff me right?
 * \subsection step2 Step 2: It's really not that bad
 * says no one on the internet
 * \subsection step3 Step 3: Coming out
 * okay I'm actually enjoying it and am not to thrilled about visual studio D:
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
#include <functional> //make_shared
#include <memory> //shared_ptr for CMaps
#include <cassert>
#include <algorithm>//find_if for entities
#include <cassert>
#include <unordered_map> //Comonent Mask map
#include <initializer_list>
/*!\namespace ecs
 * \brief header only entity component system implementation
 *
 * Entities are a combination of a uuid and their component mask which is std::bitset. A component's
 * "bit" is determined by its index in the component type vector. Bit '0' is the alive flag in the
 * ComponentMask.
 *
 * Custom container objects derived from BaseContainer are used to hold components.
 * At the moment there is only a vector implementation.
 * Components are expected to hold most/all of what a system will need to update.
 *
 * The 'ComponentContainers' object holds a map of BaseContainer shared_ptrs. At the moment, systems need
 * to cast the ptr to the correct type and container using the 'ComponentContainerTypeCast' function where
 * 'ContainerType' is the implemented container such as vector. For now, the only type of container is
 * ComponentContainer so there is only one cast function (more may be added later):
 * ComponentVectorCast.
 *
 * Systems need to be derived from BaseSystem and define update, init, and destroy functions (messaging
 * functions may be added later). The 'Systems' object will contain all of the systems in a map where
 * the key is the system's given name and the value is the object itself.
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
 *
 * Meant to be used with find_if and a vector of pairs, for more info see:
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
 *
 * Meant to be used with find_if and a vector of pairs, for more info see:
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

/****************************** COMPONENT ******************************************************/

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
class ComponentVector : public BaseContainer {
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

/*!\fn ComponentVectorCast
 * \brief casts a BaseContainer shared_ptr to a ComponentVector of the template type
 */
template<class Component>
std::shared_ptr<ComponentVector<Component>> ComponentVectorCast(std::shared_ptr<BaseContainer> basePtr){
	return std::static_pointer_cast<ecs::ComponentVector<Component>>(basePtr);
}

/*!\class ComponentContainers
 * \brief holds pointers to all component containers by name
 *
 * A map of BaseContainer shared_ptrs is utilized so that there can be various implementations of component
 * containers. Containers are accessed by name and need to casted after calling the get(). See
 * ComponentVectorCast for an example of pointer cast.
 */
class ComponentContainers{
public:
	/*!\fn add
	 * \brief makes a new container for the passed in type.
	 */
	template <class Component>
	void add(const std::string& name, Component& type){
		for(auto const& it : types){
			assert(it.first != name);
		}
		types[name] = bitCounter;
		++bitCounter;
		auto cVecPtr = std::make_shared<ComponentVector<Component>>();
		pointers[name] = cVecPtr;
	}
	/*!\fn get
	 * returns void ptr to component data structure related to the passed in name
	 */
	std::shared_ptr< BaseContainer > get(const std::string& name){
		return pointers[name];
	}
	//! returns a ComponentMask representing the passed in names of types
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

/************************* SYSTEM *********************************************************/

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
 * \brief registers systems by name and can call inherited functions from BaseSystem
 */
class Systems{
public:
	//! registers a system by name
	template<class T>
	void add(std::string const& name, std::shared_ptr<T> systemPtr){
		for(auto const& it : pointers){
			assert(it.first != name);
		}
		pointers[name] = std::static_pointer_cast<BaseSystem>(systemPtr);
	}
	//! calls init on all registered systmes
	void init(){
		for(auto const& it : pointers){
			it.second->init();
		}
	}
	//! calls update on all registered systems
	void update(){
		for(auto const& it : pointers){
			it.second->update();
		}
	}
	//! calls destroy on all registered systems
	void destroy(){
		for(auto const& it : pointers){
			it.second->destroy();
		}
		pointers.clear();
	}
private:
	std::unordered_map<std::string, std::shared_ptr<BaseSystem>> pointers;
}; // Systems class

/******************************** ENTITY *****************************************************/

/*!\class Entities
 * \brief Contains all entities and has add, remove, create, and bitMasking functions
 */
class Entities{
public:
	Entities(ComponentContainers& compContainers){
		compContainers = componentContainers;
		componentMasks.reserve(maxEntities);
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
		componentMasks[entity] = CMask;
		return entity;
	}

	Entity create(std::initializer_list<std::string>& components){
		Entity e = create();
		addEntity(e, components);
		return e;
	}
	/*!\fn remove
	 * \brief sets the "alive" flag for the passed in entity to false.
	 */
	void remove(Entity& entity){
		deletedEntities.push(entity);
		componentMasks[entity] = 0;
	}
	//! overload that takes variable amounts of entities
	void remove(std::initializer_list<Entity>& entities){
		for(Entity e : entities){
			deletedEntities.push(e);
			this->componentMasks[e] = 0;
		}
	}
private:
	/*!\fn addEntity
	 * \brief adds entity with a default ComponentMask
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
		componentMasks[entity] = CMask;
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
		componentMasks[entity] = CMask;
	}
	/*!\fn maskEntity
	 * \brief 'assigns' components to an entity by ORing its component mask with given components
	 */
	void maskEntity(Entity& entity, ComponentMask& CMask){
		componentMasks[entity] | CMask;
	}
	//! maskEntity overload to take names of components
	void maskEntity(Entity& entity, std::initializer_list<std::string>& components){
		componentMasks[entity] |componentContainers.getBitMask(components);
	}

	std::vector<ComponentMask> componentMasks; //! index = entity id. bit '0' = alive flag
	std::queue<Entity> deletedEntities; //! available indices to use in componentMasks vector
	Entity entityCount = 0; //! Max number of entity ids used so far
	ComponentContainers componentContainers;
};


}// ecs namespace


#endif /* ECS_HPP_ */
