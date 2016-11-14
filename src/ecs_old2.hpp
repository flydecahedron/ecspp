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

#ifndef ECS_OLD2_HPP_
#define ECS_OLD2_HPP_
#include "flat_map.hpp" // CMaps and systems
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
namespace ecs_old2{

/*!
 * \brief max expected number of entities and components to pass into container.reserve()
 */
const int maxEntities = 1000; // amount of entities to reserve for vector<Entity>
const int maxComponentTypes = 64;

/*!
 * \brief flatMap is used for each component type and acquired from boost::container
 * It is a contiguous ordered map
 * \tparam Key entity id
 * \tparam T component associated with the key (entity id)
 */
template <typename Key, class T>
using flatMap = boost::container::flat_map<Key, T>;

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
  CompareFirst(K key) : key_(key) {}
  bool operator()(const std::pair<K,T>& element) const {
    return key_ == element.first;
  }
  private:
    K key_;
};

//! Entity Defines
typedef std::uint32_t Entity;
typedef std::bitset<maxComponentTypes> ComponentMask;
typedef std::pair<Entity, ComponentMask> EntityData;

//! System Defines

/*!\class IComponent
 * \brief base POD struct for all component types
 */
struct IComponent{
	virtual ~IComponent(){};
};

//! Component Defines
typedef std::pair<std::string, std::type_index> typeName;
//typedef flatMap< Entity, IComponent > ICMap;

/*!\class Components
 * \brief contains all component containers and functionality to add and remove them
 */
class Components{
public:
	/*!\fn add
	 * \brief creates a new CMap for the passed in type
	 */
	template<class Component>
	flatMap< Entity, Component> newType(const std::string& CName, Component& CStruct){
		for(auto type : types){
			assert(type.first != CName);
			assert(type.second != std::type_index(typeid(CStruct)));
		}
		types.emplace_back(CName, std::type_index(typeid(CStruct)));
		flatMap< Entity, Component > CMap = flatMap< Entity, Component >();
		std::shared_ptr< flatMap<Entity, IComponent> > CMapPtr;
		CMaps.emplace(CName, CMapPtr);
		return CMap;
	}

	/*!/fn get
	 *\brief NOTE: MUST CAST TO APPROPRIATE TYPE! returns a shared_ptr of the passed in type or name
	 */
	std::shared_ptr<flatMap< Entity, IComponent>> get(std::string CName){
		return CMaps.at(CName);
	}
	//get overload
	std::shared_ptr<flatMap< Entity, IComponent>> get(std::type_index CType){
		std::string CName;
		for(auto type: types){
			if(type.second == CType){
				CName = type.first;
			}
		}
		return CMaps.at(CName);
	}

	vectorOfPairs<std::string, std::type_index> types; //!index of component type is its 'bit' in bitmask
	flatMap< std::string, std::shared_ptr< flatMap< Entity, IComponent> > > CMaps; //! CMap shared_ptrs
}; //Components

/*!\class Entities
 *
 */
class Entities{
public:
	Entities(Components& c){
		components = c;
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
		entities.emplace_back(entity, CMask);
		return entity;
	}
	void remove(Entity entity){
		auto it = std::find_if(entities.begin(), entities.end(), CompareFirst<Entity,ComponentMask>(entity));
		int index = it - entities.begin();
		availableIndices.push(index);
		deletedEntities.push(entity);
		/** TODO
		 * use ComponentMask to remove related components
		 */
	}
private:
	vectorOfPairs<Entity, ComponentMask> entities;
	std::queue<Entity> deletedEntities;
	std::queue<int> availableIndices;
	Entity entityCount = 0;
	Components components;
};

/*!\class ISystem
 * \brief base class for all system types
 *
 */
class ISystem{
public:
	virtual ~ISystem();
	virtual void update();
	virtual void init();
	virtual void destroy();

};

/*!\class Systems
 * \brief class to hold all systems and functionality to call all derived methods from ISystem.
 */
class Systems{
public:

private:

	flatMap<std::string, std::unique_ptr<ISystem>> systems;

};
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
}
#endif /* ECS_OLD2_HPP_ */

/* just in case....
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
typedef std::hash<std::string> stringHash;
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
	/*!\fn add
	 * places a new void ptr to types map for the passed in component data structure. Name is the key
	 */
	template <class T>
	void add(const std::string& componentName, T& dataStruct){
		T * ptr = dataStruct;
		types[componentName] = bitCounter;
		++bitCounter;
		pointers.emplace_back(componentName, ptr);
	}
	/*!\fn get
	 * returns void ptr to component data structure related to the passed in name
	 */
	void* get(const std::string& componentName){
		auto it = std::find_if(pointers.begin(), pointers.end(), CompareFirst<std::string,void*>(componentName));
		return it->second;
	}
	/*!\fn getBitMask
	 *
	 */
	ComponentMask getBitMask(std::initializer_list<std::string> componentNames){
		ComponentMask CMask;
		for(auto it : componentNames){
			unsigned short int bit = types[it];
			CMask.set(bit , true);
		}
		return CMask;
	}
private:
	vectorOfPairs<std::string, void*> pointers;
	std::unordered_map<std::string, unsigned short int> types;
	unsigned short int bitCounter = 1;

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
	virtual void removeEntity(Entity& entity);
	virtual void addEntity();
	//virtual void getComponents();
private:


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
	void add(T& systemObject){
		systems.push_back(std::shared_ptr<BaseSystem>(systemObject));
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
private:
	std::vector<std::shared_ptr<BaseSystem>> systems;

}; // Systems class

/*!\class Entities
 * Contains all entities and manages add, remove, create, destroy functionalities.
 * Also, is responsible for adding components to the component mask of entities
 */
class EntityManager{
public:
	EntityManager(ComponentManager& cm, SystemManager& sm){
		cm = componentManager;
		sm = systemManager;
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
		CMask | componentManager.getBitMask(components);
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
		entities[entity] |componentManager.getBitMask(components);
	}

	std::vector<ComponentMask> entities; //! index = entity id. Value = component mask
	std::queue<Entity> deletedEntities; //! available indices to use in entities vector
	std::unordered_map<std::string, std::vector<std::unique_ptr<BaseSystem>>> componentRegistry;
	Entity entityCount = 0; //! Max number of entity ids used so far
	ComponentManager componentManager;
	SystemManager systemManager;
};

}// ecs namespace



#endif /* ECS_HPP_ */
*/
