#pragma once

namespace qklib {

template <typename T>
struct ListNode {

	T *			 Data;
	ListNode<T> *pNext;
};

}	 // namespace qklib