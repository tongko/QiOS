#pragma once

namespace qklib {

template <typename T>
class IIterator {
public:
	virtual IIterator<T> &operator=(T *) = 0;
	virtual IIterator<T> &operator++() = 0;
	virtual IIterator<T> &operator++(int) = 0;
	virtual bool		  operator!=(const IIterator<T> &) = 0;
	virtual int			  operator*() = 0;
};
}	 // namespace qklib