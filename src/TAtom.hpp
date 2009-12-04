/*
 * 基于类型推导实现的类型系统
 *
 * 这只是一个尝试，努力将C++的模板用到类型推导中来。而且据洪峰老师所
 * 做，使用模板可以很大程度上简化整体的设计。
 *
 */

/* 类型模板 */ 
template <enum TypeName> struct Type;

/* 使用偏特化版本来推导类型 */ 
template <>
struct Type <T_CHAR> {
	typedef Char type;
};

template <>
struct Type <T_INTEGER> {
	typedef Integer type;
};

template <>
struct Type <T_BOOLEAN> {
	typedef Boolean type;
};

// how to implenment
template <>
struct Type <T_PAIR> {
	//typedef 
};

template <typename T1, typename T2>
struct Pair {
	typedef T1 first_t;
	typedef T2 second_t;

	first_t first;
	second_t second;
};



/* 原子定义 */ 
template <typename Type>
struct Atom {
	typename Type::type value;
};
