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

#ifndef ECS_OLD_HPP_
#define ECS_OLD_HPP_

//#include "typeMap.hpp"
#include "flat_map.hpp"
#include "any.hpp"
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
namespace ecs_old{
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
	typedef std::pair<std::string, std::type_index> typeName;
	// System Definitions


	/*!\class Components
	 * \brief contains and provides functionality for adding component types and maps
	 */
	class Components{
	public:
		//Components();
		//~Components();

		/*!\fn createComponentMap
		 * \brief inits and returns a componentMap for the type passed in
		 */
		template <class Component>
		flatMap<EntityId, Component>* createComponentMap(Component& componentStruct){
			Component C;
			addType(typeid(C).name(), componentStruct);
			flatMap<EntityId, Component> newComponentMap;
			flatMap<EntityId, Component>* ptr = &newComponentMap;
			addComponentMapPtr(ptr);
			return newComponentMap;
		}

		template <class Component>
		flatMap<EntityId, Component> createComponentMap(std::string componentName, Component& componentStruct){
			addType(componentName, componentStruct);
			flatMap<EntityId, Component> newComponentMap;
			flatMap<EntityId, Component>* ptr = &newComponentMap;
			addComponentMapPtr(ptr);
			return newComponentMap;
		}
		/*!\fn getComponentMap
		 * \brief set the desired component map type equal to this function
		 * example: textureMap* tm = getComponentMap()
		 */
		template <class Component>
		flatMap<EntityId, Component>* getComponentMap(){
			Component C;
			auto ptr = componentMapPtrs.find(std::type_index(typeid(C)))->second;
			return static_cast<flatMap<EntityId, Component>*>(ptr);
		}
		/*!\fn deleteComponentMap
		 * \brief erases entry from componentMapPtrs and clears the passed in map
		 */
		template <class Component>
		void deleteComponentMap(flatMap<EntityId, Component>* map){
			removeComponentMapPtr(map);
			map->clear();
		}
	private:
		// index of component = position of bit for that component (Entity.componentMask)
		vectorOfPairs<std::string, std::type_index> typeNames;
		vectorOfPairs<std::type_index, boost::any> types;
		flatMap<std::type_index, void*> componentMapPtrs; //

		template <class Component>
		void createComponent(EntityId entityId, std::string componentName){
			std::type_index cType;
			boost::any c;
			for(auto typeName : typeNames){
				if(typeName.first == componentName){
					cType = typeName.second;
				}
			}
			for(auto type : types){
				if(type.first == cType ){
					c = type.second;
				}
			}
			//flatMap<EntityId, c>* cMap = getComponentMap();
		}
		/*!\fn addType
		 * adds type_index and the name of a component to typeNames
		 * overloads handle only passing in a struct and .name() is used to infer the name
		 */
		template <class T>
		void addType(const std::string& componentName, T& componentStruct){
			T t;
			for(typeName tn : typeNames){
				assert(tn.second.hash_code() != std::type_index(typeid(t)).hash_code());
			}
			typeNames.emplace_back(std::make_pair(componentName, std::type_index(typeid(t))));
		}

		template <class T>
		void addType(T& componentStruct){
			T t;
			for(typeName tn : typeNames){
				assert(tn.first() != std::type_index(typeid(t)));
			}
			typeNames.emplace_back(std::make_pair(std::type_index(typeid(t).name()), std::type_index(typeid(t))));
		}

		template <typename T>
		std::type_index resolveType(std::string name){
			for(typeName tn : typeNames){
				if(tn.second() == name){
					return tn.second();
				}
			}
			return nullptr;
		}
		/*!\fn addComponentMap
		 * \brief use if a componentMap is already created, adds a ptr and typeinfo
		 */
		template <class Component>
		void addComponentMapPtr(flatMap<EntityId, Component>* map){
			Component C;
			componentMapPtrs.emplace(std::type_index(typeid(C)), map);
		}

		/*!\fn removeComponentMapPtr
		 * \brief erases passed in ptr from componentMapPtrs
		 */
		template <class Component>
		void removeComponentMapPtr(flatMap<EntityId, Component>* map){
			Component C;
			auto it = componentMapPtrs.find(std::type_index(typeid(C)));
			componentMapPtrs.erase(it);
		}
	}; // Components Class

	/*!
	 * \class Entities
	 * \brief contains all entities and manages add, remove, create, destroy functionalities
	 * Also, is responsible for adding components to the component mask of entities
	 */
	class Entities{
	public:
		Entities(Components c){
			c = components;
		};
		~Entities();
	private:
		Components components;
		vectorOfPairs<const EntityId, ComponentMask> entities; /*! holds all entities (id and mask) */
		std::queue<int> availableEntityIndices; /*! "destroyed" entity indices to be reused */
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
	void componentMapTest(){
		struct TestComponent{
			std::string name = "test name";
		};
		TestComponent test;
		ecs::EntityId entity = 1;
		ecs::Components components;
		ecs::flatMap<ecs::EntityId, TestComponent> testMap = components.createComponentMap("test",test);
		testMap.emplace(entity, test);
		TestComponent test2 = testMap.at(entity); //testMap[entity]
		std::string testName = test2.name;
		cout << testName << endl;
		testMap* pTestMap2;
		pTestMap2 = components.getComponentMap();
	}


}// ecs namespace

#endif /* ECS_OLD_HPP_ */
