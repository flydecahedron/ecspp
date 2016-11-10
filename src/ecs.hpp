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
#include "flat_map.hpp" // CMaps and systems
#include <vector>
#include <queue> // available entity ids and idices
#include <typeindex> // to store component types
#include <bitset> // component bitmasking
#include <functional> //make_shared for creating new CMap
#include <memory> //shared_ptr for CMaps
#include <cassert> // static_assert component types
#include <algorithm>//find_if for entities
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
	virtual ~IComponent();
};

//! Component Defines
typedef std::pair<std::string, std::type_index> typeName;


/*!\class Components
 * \brief contains all component containers and functionality to add and remove them
 */
class Components{
public:
	/*!\fn add
	 * \brief creates a new CMap for the passed in type
	 */
	template<class Component>
	void add(std::string& CName, Component& CStruct){
		for(auto type : types){
			static_assert(type.first != CName, "Component names must be unique");
			static_assert(type.second != std::type_index(typeid(CStruct)), "Component types must be unique");
		}
		types.emplace_back(CName, std::type_index(typeid(CStruct)));
		flatMap< Entity, Component > CMap;
		CMaps.emplace(CName, std::make_shared<CMap>());
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

private:
	vectorOfPairs<std::string, std::type_index> types; //!index of component type is its 'bit' in bitmask
	flatMap< std::string, std::shared_ptr< flatMap< Entity, IComponent> > > CMaps; //! CMap shared_ptrs
}; //Components

/*!\class Entities
 *
 */
class Entities{
public:
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

}
#endif /* ECS_HPP_ */
