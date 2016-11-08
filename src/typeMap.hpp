/*
 * typemap.hpp
 *
 *  Created on: Nov 8, 2016
 *      Author: eroc
 */

#ifndef TYPEMAP_HPP_
#define TYPEMAP_HPP_

#include <vector>
#include <typeinfo>
//#include <map>
#include "flat_map.hpp"

namespace ecs{
	/**\class typeMap
	 * \brief map container that holds different container ptr types
	 *
	 */
	class typeMap{
	public:
		/*
		 *
		 */
		template<typename Key, class T>
		void addFlatMapPtr(flatMap<Key, T>* flatMap){

		}
	private:
		flatMap<unsigned long, void*> m_ptrs;
	};
}



#endif /* TYPEMAP_HPP_ */
