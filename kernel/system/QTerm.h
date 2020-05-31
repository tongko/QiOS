#pragma once

#include <system/system.h>

namespace System {

class QTerm : public ITerm {
	COORD m_pos;
	COLOR m_back;
	COLOR m_fore;

public:
	//	Attributes
	COLOR GetForeColor() const { return m_fore; };
	void  SetForeColor(COLOR fore) { m_fore = fore; };
	COLOR GetBackColor() const { return m_back; };
	void  SetBackColor(COLOR back) { m_back = back; };

public:
	//	Operations
	void Print(const int8_t *);
	void Clear();
	void MoveTo(const PCOORD);

private:
	// Operations
	void PutC(int8_t, COLOR, COLOR);
};

}	 // namespace System