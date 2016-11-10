/**\mainpage ecspp
 * ecs.hpp
 *
 *  Created on: Nov 9, 2016
 *      Author: eroc
 */

#ifndef ECS_HPP_
#define ECS_HPP_
#include "flat_map.hpp"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <bitset>
#include <functional>
#include <memory>
#include <cassert>
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

//! Entity Defines
typedef std::uint32_t Entity;
typedef std::bitset<maxComponentTypes> ComponentMask;
typedef std::pair<Entity, ComponentMask> EntityData;

//! System Defines


/*!\class Entities
 *
 */
class Entities{
public:
	Entity create(){
		Entity e;
		return e;
	}
private:
	vectorOfPairs<Entity, ComponentMask> entities;
};

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
	template<class Component>
	void add(std::string& CName, Component& CStruct){
		for(auto type : types){
			static_assert(type.first != CName, "Component names must be unique");
			static_assert(type.second != std::type_index(typeid(CStruct)), "Component types must be unique");
		}
		types.emplace_back(CName, std::type_index(typeid(CStruct)));
		flatMap< Entity, Component > CMap;
		CMaps.emplace(CName, CMap);
	}

private:
	vectorOfPairs<std::string, std::type_index> types; //!index of component type is its 'bit' in bitmask
	flatMap< std::string, std::shared_ptr< flatMap< Entity, IComponent> > > CMaps; //! CMap shared_ptrs
}; //Components

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
