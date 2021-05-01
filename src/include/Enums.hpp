#pragma once

namespace qklib {

template <typename E, E first, E head>
void advanceEnum(E &v) {

	if (v == head) {
		v = first;
	}
}

template <typename E, E first, E head, E next, E... tail>
void advanceEnum(E &v) {

	if (v == head) {
		v = next;
	}
	else {
		advanceEnum<E, first, next, tail...>(v);
	}
}

template <typename E, E first, E... values>
struct EnumValues {
	static void advance(E &v) { advanceEnum<E, first, first, values...>(v); }
};

}	 // namespace qklib