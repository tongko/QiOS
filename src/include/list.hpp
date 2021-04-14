#pragma once

namespace qklib {

template <typename T>
class List {

public:	   //	Constructors
	List();
	~List();

public:	   //	Attributes
	int Count();
	T * Get(int tIndex) const;

public:	   //	Operations
	void Add(T tItem);
	T	 Remove(T tItem);
	T	 RemoveAt(int tIdx);

private:
};

}	 // namespace qklib