#pragma once

namespace qklib {

template <typename T>
struct LinkedListNode {
	T *				pValue;
	LinkedListNode *pNext;
};

}	 // namespace qklib