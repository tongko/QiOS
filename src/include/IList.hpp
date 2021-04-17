#pragma once

namespace qklib {

//	Prototypes
template <typename T>
class ICollection;

template <typename T>
class IList : public ICollection<T> {

public:
	virtual T &	   Get(size_t) = 0;
	virtual void   Set(size_t, T &) = 0;
	virtual size_t IndexOf(T &) = 0;
	virtual void   Insert(size_t, T &) = 0;
	virtual void   RemoveAt(size_t) = 0;
};

}	 // namespace qklib