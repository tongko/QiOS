#pragma once

#include <ListNode.hpp>

namespace qklib {

template <typename T>
class LinkedList {

public:	   // ctor
	LinkedList(bool tbDelElem = false);
	~LinkedList();

public:	   // Attributes
	bool IsEmpty();
	int	 Count();

public:	   // Operations
	void AddNode(T *);
	T *	 Remove(int);
	T *	 operator[](int index);

	template <typename Predicate>
	T *Find(Predicate tPred);

private:
	ListNode<T> *m_pHead;
	ListNode<T> *m_pTail;
	bool		 m_bDelElem;
	int			 m_nCount;
};

template <typename T>
LinkedList<T>::LinkedList(bool tbDelElem)
	: m_pHead(0)
	, m_pTail(0)
	, m_bDelElem(tbDelElem)
	, m_nCount(0) {}

template <typename T>
LinkedList<T>::~LinkedList() {

	if (!m_bDelElem)
		return;

	ListNode<T> *p = m_pHead;
	ListNode<T> *next = nullptr;
	m_pHead = nullptr;
	while (p) {
		next = p->pNext;
		delete p;
		p = next;
	}
}

template <typename T>
bool LinkedList<T>::IsEmpty() {

	return m_nCount == 0;
}

template <typename T>
int LinkedList<T>::Count() {

	return m_nCount;
}

template <typename T>
void LinkedList<T>::AddNode(T *tpData) {

	if (m_pHead == nullptr) {
		m_pHead = new ListNode<T>();
		m_pHead->Data = tpData;
		m_pHead->pNext = nullptr;
		m_pTail = m_pHead;
	}
	else {
		ListNode<T> *newNode = new ListNode<T>();
		newNode->Data = tpData;
		newNode->pNext = nullptr;
		ListNode<T> *temp;
		temp = m_pTail;
		m_pTail = newNode;
		temp->pNext = newNode;
	}

	m_nCount++;
}

template <typename T>
T *LinkedList<T>::Remove(int tnIdx) {

	if (tnIdx >= m_nCount) {
		return nullptr;
	}

	ListNode<T> *theNode = m_pHead;
	ListNode<T> *pPrev = nullptr;
	while (tnIdx && theNode != nullptr) {
		tnIdx--;
		pPrev = theNode;
		theNode = theNode->pNext;
	}

	//	found the node, let delete it
	if (pPrev != nullptr) {
		pPrev->pNext = theNode->pNext;
		if (pPrev->pNext->pNext == nullptr) {
			m_pTail = pPrev->pNext;
		}
	}
	else {
		m_pHead = theNode->pNext;
	}

	T *data = theNode->Data;
	delete theNode;
	m_nCount--;

	return data;
}

template <typename T>
T *LinkedList<T>::operator[](int tnIdx) {

	if (tnIdx >= m_nCount) {
		return nullptr;
	}

	ListNode<T> *theNode = m_pHead;
	while (tnIdx && theNode != nullptr) {
		tnIdx--;
		theNode = theNode->pNext;
	}

	if (theNode != nullptr)
		return theNode->Data;

	return nullptr;
}

template <typename T>
template <class Predicate>
T *LinkedList<T>::Find(Predicate tPred) {

	if (m_pHead != nullptr) {
		ListNode<T> *pNode = m_pHead;
		while (pNode) {
			if (tPred(pNode->Data))
				return pNode->Data;
			pNode = pNode->pNext;
		}
	}

	return nullptr;
}

}	 // namespace qklib