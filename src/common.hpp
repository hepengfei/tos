// -*-c++-*-

#ifndef common_HPP
#define common_HPP


// 
// headers
//

#include <cstring>
#include <string>
#include <stack>
#include <vector>
#include <istream>
#include <ostream>
#include <iostream>
#include <sstream>
#include <fstream>


//
// 堆相关常量
// 

// 单位设置
#define UNIT_KB (1024)
#define UNIT_MB (1024 * UNIT_KB)


// 堆的大小
#define HEAP_SIZE (512 * UNIT_KB)


// namespace
namespace tos {

	// 引入标准名字空间
	using namespace std;

} // namespace

#endif	// common_HPP
