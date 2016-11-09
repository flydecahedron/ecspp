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

//#include "typeMap.hpp"
#include "flat_map.hpp"
#include <vector>
#include <bitset> // component mask
#include <queue> // available entity ids
#include <memory> //unique_ptr for vector<systems>
#include <typeinfo> //for component types vector and map of component ptrs
#include <typeindex>

/*!\namespace ecs
 * \brief entity component system implementation where entities, components, and systems
 * are held in objects of those names respectively.
 *
 * Entities are a combination of a uuid and their component mask which is std::bitset. A component's
 * "bit" is determined by its index in the component type vector.
 *
 * boost flat_map is used for each different component type for contiguous memory and fast
 * iteration. Components are expected to hold most/all of what a system will need to update.
 *
 * Systems need to be derived from BaseSystem and define update, init, and destroy functions (messaging
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

	// Entity Definitions
	typedef std::uint32_t EntityId;
	typedef std::bitset<maxComponentTypes> ComponentMask;
	typedef std::pair<EntityId, ComponentMask> entity;


	// Component Definitions

	// System Definitions

	/*!
	 * \class Entities
	 * \brief contains all entities and manages add, remove, create, destroy functionalities
	 * Also, is responsible for adding components to the component mask of entities
	 */
	class Entities{
	public:
		Entities();
		~Entities();
	private:
		vectorOfPairs<const EntityId, ComponentMask> entities; /*! holds all entities (id and mask) */
		std::queue<int> availableEntityIndices; /*! "destroyed" entity indices to be reused */
	};

	class Components{
	public:
		Components();
		~Components();

		template <class T>
		void addType(const std::string& componentName, T& componentStruct){
			T t;
			typeNames.emplace_back(std::make_pair(componentName, std::type_index(typeid(t))));
		}

		template <class Component>
		void addComponentMap(flatMap<EntityId, Component>* map){
			Component C;
			componentMapPtrs.insert(typeid(C).hash_code(), map);
		}

		template <class Component>
		flatMap<EntityId, Component>* getComponentMap(){
			Component C;
			auto it = componentMapPtrs.find(std::type_index(typeid(C)));

			return static_cast<flatMap<EntityId, Component>*>(it);
		}
		template <class T>
		void remove(T component){

		}
	private:
		// index of component = position of bit for that component (Entity.componentMask)
		vectorOfPairs<std::string, std::type_index> typeNames;
		flatMap<std::type_index, void*> componentMapPtrs; //
	}; // Components Class

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
	class Systems{
	public:
		Systems();
		~Systems();

		template <class T>
		void add(T systemObject){
			systems.push_back(std::unique_ptr<BaseSystem>(new T));
		}

		void initAll(){
			for(auto const& system : systems){
				system->init();
			}
		}

		void update(){
			for(auto const& system : systems){
				system->update();
			}
		}

		void removeAll(){
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
