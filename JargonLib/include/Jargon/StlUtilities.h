#pragma once

#include <algorithm>


namespace Jargon{
namespace StlUtilities{

	template<typename SetType>
	const bool SetContains(const SetType& set, const typename SetType::key_type& value) {
		return set.find(value) != set.end();
	}

	template<typename VectorType>
	const bool VectorContains(const VectorType& vector, const typename VectorType::value_type& value) {
		return std::find(vector.begin(), vector.end(), value) != vector.end();
	}

	template<typename MapType>
	const typename MapType::mapped_type & GetOrDefault(const MapType & map, const typename MapType::key_type & key, const typename MapType::mapped_type & defaultValue){
		typename MapType::const_iterator it = map.find(key);
		if( it == map.end() ){
			return defaultValue;
		}
		return it->second;
	}
}
}
