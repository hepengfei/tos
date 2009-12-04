// -*-c++-*-

#ifndef Trace_HPP
#define Trace_HPP



#include <string>
#include <iostream>


// namespace
namespace tos {


#if DEBUG

#define TRACE(fun) Trace __TRACE__(fun)
#define TRACE_ENABLE(b) (b) ? Trace::enable () : Trace::disable ()
#define TRACE_OUT(msg) __TRACE__.print(msg)

// 此宏可在当前作用域环境中关闭跟踪输出
#define TRACE_LOCK TraceLock __TRACE_LOCKER__; __TRACE_LOCKER__.dummy()

#else  // DEBUG
	
#define TRACE(fun)      while(0)
#define TRACE_ENABLE(b) while(0)
#define TRACE_OUT(msg)  while(0)
#define TRACE_LOCK      while(0)

#endif	// DEBUG
	

// 
// 用于跟踪执行的类
// 
class Trace {
public:
	Trace (std::string pFunction) : mFunction(pFunction) {
		++ level;

		print (std::string("enter ") + mFunction);
	}

	~Trace () {
		print (std::string("exit ") + mFunction);
		
		-- level;
	}

	void print (std::string msg) {
		if (isEnable ()) {
			std::cout << std::string(level,' ')
				  << msg << std::endl;
		}
	}

	//
	// 暂时开启或关闭调试信息输出
	// 

	static void enable () {
		enabled = true;
	}

	static void disable () {
		enabled = false;
	}

	static bool isEnable () {
		return enabled;
	}
	
private:
	std::string mFunction;

	static int level;
	static bool enabled;
};


// 可以暂时关闭TRACE输出的类
class TraceLock {
public:
	TraceLock () {
		//std::cout << "lock Trace." << std::endl;
		Trace::disable ();
	}

	~TraceLock () {
		//std::cout << "unlock Trace." << std::endl;		
		Trace::enable ();
	}

	void dummy () {}
};

} // namespace


#endif	// Trace_HPP

