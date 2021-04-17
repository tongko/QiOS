#pragma once

#include <IIterator.hpp>

namespace qklib {

template <typename T>
class IIteratable {
public:
	IIterator<T> begin();
	IIterator<T> end();
};

}	 // namespace qklib