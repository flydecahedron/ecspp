/*! \file typemap.hpp
 * got idea from http://stackoverflow.com/questions/24285777/mapping-vector-pointers-of-different-type
 * another idea http://codereview.stackexchange.com/questions/84176/proper-use-of-type-info-in-relation-to-mapping-components-to-a-type
 */

#ifndef TYPEMAP_HPP_
#define TYPEMAP_HPP_

#include <vector>
#include <typeinfo>
#include "flat_map.hpp"


/*!\class typeMap
 * \brief map container that holds different container pointer types
 * object types must have default constructors
 *
 */
class typeMap{
	using flat_map = boost::container::flat_map;
public:
	/*!
	 * \fn addVectPtr
	 * \brief Adds a vector pointer of a generic type to typeMap
	 */
	template<class T>
	void addVectorPtr(std::vector<T>* vecPtr){
		T t;
		pointers[typeid(t).hash_code()] = vecPtr;
	}
	/*!
	 * \fn getVectorPtr
	 * \brief returns a vector ptr of a generic type
	 */
	template<class T>
	std::vector<T>* getVectorPtr(){
		T t;
		return (std::vector<T>*)(pointers[typeid(t).hash_code()]);
	}

	//template<class T>
	//void addFlatMapPtr(flatMap<)
private:
	flat_map<unsigned long, void*> pointers;
};




#endif /* TYPEMAP_HPP_ */
