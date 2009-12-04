// -*-c++-*-

#ifndef Exception_HPP
#define Exception_HPP



#include "common.hpp"



//
// 异常类的定义
//
// 实现了系统和Scheme中需要的异常。
//




// namespace
namespace tos {



	//
	// 异常基类
	//
	
	class Exception {

	public:

		// 引起异常的对象及对错误的描述
		Exception (const string pCause, const string pMessage)
			: mCause(pCause)
			, mMessage(pMessage) {}

		// 使类成为抽象类
		virtual ~Exception () {}

		// 获取引起异常的对象的外部表示
		string getCause () const {
			return mCause;
		}

		// 获取异常的消息
		string getMessage () const {
			return mMessage;
		}

		// 获取异常的字符串表示
		virtual string toString () const {
			return string ("\'")
				+ getCause()
				+ string ("\' caused a ")
				+ getMessage();
		}

	private:
		// 引起异常的对象
		string mCause;

		// 相关的异常描述消息
		string mMessage;
	};

	class RuntimeException : public Exception {

	public:

		RuntimeException (const string pCause, const string pMessage)
			: Exception (pCause, pMessage) {}

		~RuntimeException () {}
	};

	class DivisionByZeroException : public RuntimeException {

	public:
		DivisionByZeroException (const string pCause="0")
			: RuntimeException (pCause
					    ,"division by zero exception") {}
	};

	class ArityException : public RuntimeException {
	public:
		ArityException (const string pCause,
				const string pDetail)
			: RuntimeException (pCause
					    ,"expected "+pDetail) {}
	};

	// 
	// 类型转换异常类
	// 

	// 复合对象异常
	class CompoundExpected : public Exception {
		
	public:
		
		CompoundExpected (const string pCause)
			: Exception (pCause
				     ,"compound object expected") {}
	
		~CompoundExpected () {}
	};

	// 符号对象异常
	class SymbolExpected : public Exception {
		
	public:
		
		SymbolExpected (const string pCause)
			: Exception (pCause
				     ,"symbol expected") {}
	
		~SymbolExpected () {}
	};

	// 点对异常
	class PairExpected : public Exception {
		
	public:
		
		PairExpected (const string pCause)
			: Exception (pCause
				     ,"pair expected") {}
	
		~PairExpected () {}
	};

	// 整数异常
	class IntegerExpected : public Exception {

	public:
		
		IntegerExpected (const string pCause)
			: Exception (pCause
				     ,"integer object expected") {
		}

		~IntegerExpected () {}
	};

	// 字符对象异常
	class CharExpected : public Exception {

	public:

		CharExpected (const string pCause)
			: Exception(pCause
				    ,"char object expected") {}

		~CharExpected () {}
	};

	// 布尔对象异常
	class BooleanExpected : public Exception {
		
	public:
		
		BooleanExpected (const string pCause)
			: Exception (pCause
				     ,"boolean object expected") {}
	
		~BooleanExpected () {}
	};

	class PrimOpExpected : public Exception {

	public:

		PrimOpExpected (const string pCause)
			: Exception (pCause
				     ,"primitive procedure expected") {}

		~PrimOpExpected () {}
	};

	// 未知表达式类型异常
	class UnknownExpression : public Exception {

	public:
		UnknownExpression (const string pCause)
			: Exception (pCause
				     ,"unknown expression type") {}

		~UnknownExpression () {}
	};

	// 未绑定的变量异常
	class UnboundVariable : public Exception {

	public:

		UnboundVariable (const string pCause)
			: Exception (pCause
				     ,"unbound variable") {}

		~UnboundVariable () {}
	};


	//
	// 系统内部异常
	// 

	// 内部异常基类
	class InnerException : public Exception {

	public:

		InnerException (const string pMessage)
			: Exception ("", pMessage) {}
		
		virtual ~InnerException () {}

		// 内部异常只有异常描述
		virtual string toString () const {
			return getMessage ();
		}
	};
	
	// 内存耗尽异常类
	class MemoryExhaustedException : public InnerException {
		
	public:
		
		MemoryExhaustedException()
			: InnerException("memory exhausted") {}

		~MemoryExhaustedException () {}
	};

	// 数组越界异常
	class OutOfRangeException : public InnerException {

	public:

		OutOfRangeException ()
			: InnerException ("array's index out of range") {}

		~OutOfRangeException () {}
	};

	// 对空栈执行出栈时的异常
	class EmptyStackException : public InnerException {
		
	public:
		
		EmptyStackException ()
			: InnerException ("stack is empty, cannot restore") {}

		~EmptyStackException () {}
	};

	// 词法分析器异常
	class ParserException : public InnerException {
		
	public:

		ParserException (const string pMessage)
			: InnerException(pMessage) {}

		~ParserException () {}
	};

	// 未知的基本操作异常
	class UnknownPrimOperation : public InnerException {

	public:

		UnknownPrimOperation ()
			: InnerException ("unknown primitive operation") {}

		~UnknownPrimOperation () {}
	};

	class ArgException : public InnerException {

	public:

		ArgException ()
			: InnerException ("unknown arguments") {}

		~ArgException () {}
	};



} // namespace


#endif	// Exception_HPP
