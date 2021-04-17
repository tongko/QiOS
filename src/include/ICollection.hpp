#pragma once

namespace qklib {

//	Prototypes
template <typename T>
class IIteratable;

template <typename T>
class ICollection : public IIteratable<T> {

public:
	virtual size_t Count_get() = 0;
	virtual bool   IsReadOnly_get() = 0;
	virtual void   Add(T &) = 0;
	virtual void   Clear() = 0;
	virtual bool   Contains(T &) = 0;
	virtual void   CopyTo(T *, size_t) = 0;
	virtual bool   Remove(T &) = 0;
};
}	 // namespace qklib