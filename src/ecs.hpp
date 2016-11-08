/*
 * Component.hpp
 *
 *  Created on: Nov 7, 2016
 *      Author: eroc
 */

#ifndef ECS_HPP_
#define ECS_HPP_

#include "flat_map.hpp"
#include <vector>
#include <bitset> // component mask
#include <queue> // available entity ids
#include <memory> //unique_ptr for vector<systems>

/*	TODO
 * Component-> add, remove, addType
 * System-> add, remove
 */

/**\namespace ecs
 * \brief entity component system implementation where entities, components, and systems
 * are held in objects of those names respectively.
 *
 */
namespace ecs{
	/**
	 * \brief max expected number of entities and components to pass into container.reserve()
	 */
	const int maxEntities = 1000; // amount of entities to reserve for vector<Entity>
	const int maxComponentTypes = 64;

	/**
	 * \brief flatMap is used for each component type and acquired from boost::container
	 * It is a contiguous ordered map
	 * \tparam Key entity id
	 * \tparam T component associated with the key (entity id)
	 */
	template <typename Key, class T>
	using flatMap = boost::container::flat_map<Key, T>;

	/**
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

	/**
	 * \class Entities
	 * \brief contains all entities and manages add, remove, create, destroy functionalities
	 * Also, is responsible for adding components to the component mask of entities
	 */
	class Entities{
	public:
		Entities();
		~Entities();
	private:
		vectorOfPairs<const EntityId, ComponentMask> entities; /** holds all entities (id and mask) */
		std::queue<int> availableEntityIndices; /** "destroyed" entity indices to be reused */
	};

	class Components{
	public:
		Components();
		~Components();



		template <class T>
		void add(const std::string& componentName, T& componentStruct){

		}

		template <class T>
		void remove(T component){

		}
	private:
		// index of component = position of bit for that component (Entity.componentMask)

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

	/* Systems
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
