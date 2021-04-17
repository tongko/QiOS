#pragma once

#include <common.hpp>

namespace qklib {

//	Prototypes begin
template <typename T>
class IIteratable;

template <typename T>
struct LinkedListNode;

class Mutex;
//	Prototypes end

template <typename T>
class List : public IList<T> {

public:
	List();
	List(IIteratable<T> &collection);

public:	   //	IIterator implementations
	IIterator<T> &GetIterator() override;

public:	   // Attributes
	PROP_DECL_IMPL(size_t, Capacity);
	PROPERTY_DECL(size_t, Count);

public:	   // ICollection implementations
	size_t Count_get() override;
	bool   IsReadOnly_get() override;
	void   Add(T &) override;
	void   Clear() override;
	bool   Contains(T &) override;
	void   CopyTo(T *, size_t) override;
	bool   Remove(T &) override;

public:	   // IList implementations
	T &	   Get(size_t) override;
	void   Set(size_t, T &) override;
	size_t IndexOf(T &) override;
	void   Insert(size_t, T &) override;
	void   RemoveAt(size_t) override;

public:	   // Operations
	void AddRange(IIteratable<T> collection);

private:
	LinkedListNode<T> *m_pHead;
	LinkedListNode<T> *m_pTail;
	Mutex *			   m_pMutex;
	size_t			   m_nCount;
	size_t			   m_nCapacity;
	int				   m_nVersion;
};

}	 // namespace qklib