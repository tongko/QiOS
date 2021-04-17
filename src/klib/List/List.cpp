#include <IIteratable.hpp>
#include <IIterator.hpp>
#include <LinkedListNode.hpp>
#include <List.hpp>
#include <mutex.hpp>

namespace qklib {

static const int s_DefCapacity = 4;

/**
 * List()
 */
template <typename T>
List<T>::List()
	: m_pHead(0)
	, m_pTail(0)
	, m_pMutex(0)
	, m_nCount(0)
	, m_nCapacity(0)
	, m_nVersion(0) {}

/**
 * List(IIteratable)
 */
template <typename T>
List<T>::List(IIteratable<T> &collection)
	: this() {

	AddRange(collection);
}

}	 // namespace qklib