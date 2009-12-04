// -*-c++-*-


#ifndef Evaluator_HPP
#define Evaluator_HPP



#include "common.hpp"
#include "Atom.hpp"
#include "Machine.hpp"


// namespace
namespace tos {


	class Evaluator {

	public:

		Evaluator (Machine &pMachine)
			: mMachine (pMachine) {
			setupEnv ();
		}


		// 调用接口
		void operator() () {
			eval ();
		}

	protected:

		// 
		// 对R_EXP中的表达式求值
		//
		
		void eval ();

		void apply ();

		//
		// 检测R_EXP寄存器中的表达式的类型
		//

		// 自求值，有标志（破碎的心和EOF）、空表、字符、整数、
		// 布尔、基本过程、端口、向量、字符串，即除点对、符号和
		// 过程外都是。
		bool isSelfEvaluating ();

		// 变量，即符号
		bool isVariable ();

		bool isPrimOp ();
		/*
		// 引号表达式，(quote ...)
		bool isQuoted ();

		// 赋值set!
		bool isAssignment ();

		// define
		bool isDefinition ();

		// if
		bool isIf ();

		// lambda
		bool isLambda ();
		*/
		// application
		bool isApplication ();

		//
		// 对各种类型的表达式进行求值
		//

		void evSelfEval ();
			
		void evVariable ();

		void evPrimOp ();
		/*
		void evQuoted ();

		void evAssignment ();
		void evAssignmentEval ();

		void evDefinition ();
		void evDefinitionEval ();

		void evIf ();

		void evLambda ();
		*/
		void evApplication ();

		OperationType getOpType ();

		bool isType (OperationType);
		bool isOpType (OperationType);
		

		void setupEnv ();
		
	private:

		Machine &mMachine;

	};


} // namespace



#endif	// Evaluator_HPP
