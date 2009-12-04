// -*-c++-*-


#include "Evaluator.hpp"
#include "PrimitiveProcedure.hpp"
#include "Exception.hpp"


// namespace
namespace tos {



	//
	// 根据R_EXP中的表达式类型，来调用相应的求值函数
	//
	
	void Evaluator::eval () {


		// 约定值 - 参见Main.cpp::run
		// 运行的结束标志为EOF
		while (mMachine.reg (R_EXP)
		       .isEof ().isFalse ()) {
			if (isSelfEvaluating ()) // 自求值表达式
				evSelfEval ();
			else if (isVariable ()) // 变量
				evVariable ();
			else if (isPrimOp ())
				evPrimOp ();
			else if (isApplication ()) // 过程调用/组合式
				evApplication ();
			else
				throw UnknownExpression (mMachine.getString (R_EXP));
		}
	}


	// 自求值 #\a ...
	bool Evaluator::isSelfEvaluating () {
		Atom exp = mMachine.reg (R_EXP);
		return (exp.isPrimOp ().isFalse ()
			&& exp.isPair ().isFalse ()
			&& exp.isSymbol ().isFalse ()
			&& exp.isProcedure ().isFalse ());
	}

	// 变量，即符号：symbol
	bool Evaluator::isVariable () {
		return mMachine.reg (R_EXP)
			.isSymbol ().isTrue ();
	}

	// 内置基本操作
	bool Evaluator::isPrimOp () {
		return mMachine.reg (R_EXP)
			.isPrimOp ().isTrue ();
	}

	// (operator operands)
	bool Evaluator::isApplication () {
		Atom exp = mMachine.reg (R_EXP);
		bool result = exp.isPair ().isTrue ();

		return result;
	}


	void Evaluator::evSelfEval () {
		mMachine.assign (R_VAL, R_EXP);

		// 返回到合适的位置
		mMachine.assign (R_EXP, R_CONTINUE);
	}
			
	void Evaluator::evVariable () {
		mMachine.lookup (R_EXP);
		
		// 返回到合适的位置
		mMachine.assign (R_EXP, R_CONTINUE);
	}

	
	void Evaluator::evPrimOp () {

#define HANDLE_OT(ot) case ot: op<ot> (mMachine);break

		switch (mMachine.reg (R_EXP).getOpType()) {
		case OT_QUOTE:
			op<OT_QUOTE> (mMachine);
			break;
		case OT_LAMBDA:
			op<OT_LAMBDA> (mMachine);
			break;
		case OT_IF:
			op<OT_IF> (mMachine);
			break;
		case OT_SET:
			op<OT_SET> (mMachine);
			break;
		case OT_DEFINE:
			op<OT_DEFINE> (mMachine);
			break;
		case OT_BEGIN:
			op<OT_BEGIN> (mMachine);
			break;
		case OT_AND:
			op<OT_AND> (mMachine);
			break;
		case OT_AND_CONT:
			op<OT_AND_CONT> (mMachine);
			break;
		case OT_OR:
			op<OT_OR> (mMachine);
			break;
		case OT_OR_CONT:
			op<OT_OR_CONT> (mMachine);
			break;
			// 
		case OT_IF_DECIDE:
			op<OT_IF_DECIDE> (mMachine);
			break;
		case OT_SET_EVAL:
			op<OT_SET_EVAL> (mMachine);
			break;
		case OT_DEFINE_EVAL:
			op<OT_DEFINE_EVAL> (mMachine);
			break;
		case OT_DID_EVAL_OPERATOR:
			op<OT_DID_EVAL_OPERATOR> (mMachine);
			break;
		case OT_EVAL_OPERAND_LOOP:
			op<OT_EVAL_OPERAND_LOOP> (mMachine);
			break;
		case OT_EVAL_LAST_OPERAND:
			op<OT_EVAL_LAST_OPERAND> (mMachine);
			break;
		case OT_ACCUMULATE_OPERAND:
			op<OT_ACCUMULATE_OPERAND> (mMachine);
			break;
		case OT_ACCUMULATE_LAST:
			op<OT_ACCUMULATE_LAST> (mMachine);
			break;
		case OT_APPLY:
			op<OT_APPLY> (mMachine);
			break;
		case OT_EVAL_SEQUENCE:
			op<OT_EVAL_SEQUENCE> (mMachine);
			break;
		case OT_EVAL_SEQUENCE_LOOP:
			op<OT_EVAL_SEQUENCE_LOOP> (mMachine);
			break;
		case OT_EVAL_SEQUENCE_END:
			op<OT_EVAL_SEQUENCE_END> (mMachine);
			break;
		case OT_EVAL_DONE:
			op<OT_EVAL_DONE> (mMachine);
			break;
			// 
		case OT_CONS:
			op<OT_CONS> (mMachine);
			break;
		case OT_CAR:
			op<OT_CAR> (mMachine);
			break;
		case OT_CDR:
			op<OT_CDR> (mMachine);
			break;
		case OT_SETCAR:
			op<OT_SETCAR> (mMachine);
			break;
		case OT_SETCDR:
			op<OT_SETCDR> (mMachine);
			break;
		case OT_LENGTH:
			op<OT_LENGTH> (mMachine);
			break;
			// 
		case OT_PLUS:
			op<OT_PLUS> (mMachine);
			break;
		case OT_MINUS:
			op<OT_MINUS> (mMachine);
			break;
		case OT_MULTI:
			op<OT_MULTI> (mMachine);
			break;
		case OT_DIV:
			op<OT_DIV> (mMachine);
			break;
		case OT_EQUAL_TO:
			op<OT_EQUAL_TO> (mMachine);
			break;
		case OT_LESS_THEN:
			op<OT_LESS_THEN> (mMachine);
			break;

		case OT_GREATER_THEN: // >
			op<OT_GREATER_THEN> (mMachine);
			break;
		case OT_LESS_THEN_OR_EQUAL_TO: // <=
			op<OT_LESS_THEN_OR_EQUAL_TO> (mMachine);
			break;
		case OT_GREATER_THEN_OR_EQUAL_TO: // >=
			op<OT_GREATER_THEN_OR_EQUAL_TO> (mMachine);
			break;
		case OT_IS_NUMBER:	// number?
			op<OT_IS_NUMBER> (mMachine);
			break;
		case OT_IS_INTEGER:	// integer?
			op<OT_IS_INTEGER> (mMachine);
			break;
//case OT_MATH_SQRT: // sqrt
			// break;
//case OT_MATH_SIN:
			// ...

			//break;
		case OT_NUMBER_TO_STRING: // number->string
			op<OT_NUMBER_TO_STRING> (mMachine);
			break;
		case OT_STRING_TO_NUMBER: // string->number
			op<OT_STRING_TO_NUMBER> (mMachine);
			break;
		case OT_IS_EQV:	// eqv?
			op<OT_IS_EQV> (mMachine);
			break;
		case OT_IS_EQ:	// eq?
			op<OT_IS_EQ> (mMachine);
			break;
		case OT_IS_EQUAL:	// equal?
			op<OT_IS_EQUAL> (mMachine);
			break;
		case OT_NOT:	// not
			op<OT_NOT> (mMachine);
			break;
		case OT_IS_BOOLEAN:	// boolean?
			op<OT_IS_BOOLEAN> (mMachine);
			break;
		case OT_IS_PAIR:	// pair?
			op<OT_IS_PAIR> (mMachine);
			break;
		case OT_IS_NULL:	// null?
			op<OT_IS_NULL> (mMachine);
			break;
		case OT_IS_LIST:	// list?
			op<OT_IS_LIST> (mMachine);
			break;
		case OT_IS_SYMBOL:	// symbol?
			op<OT_IS_SYMBOL> (mMachine);
			break;
		case OT_SYMBOL_TO_STRING: // symbol->string
			op<OT_SYMBOL_TO_STRING> (mMachine);
			break;
		case OT_STRING_TO_SYMBOL: // string->symbol
			op<OT_STRING_TO_SYMBOL> (mMachine);
			break;
		case OT_IS_CHAR:	// char?
			op<OT_IS_CHAR> (mMachine);
			break;
		case OT_CHAR_EQUAL_TO: // char=?
			op<OT_CHAR_EQUAL_TO> (mMachine);
			break;
		case OT_CHAR_LESS_THEN: // char<?
			op<OT_CHAR_LESS_THEN> (mMachine);
			break;
		case OT_CHAR_GREATER_THEN: // char>?
			op<OT_CHAR_GREATER_THEN> (mMachine);
			break;
		case OT_CHAR_LESS_THEN_OR_EQUAL_TO: // char<=?
			op<OT_CHAR_LESS_THEN_OR_EQUAL_TO> (mMachine);
			break;
		case OT_CHAR_GREATER_THEN_OR_EQUAL_TO: // char>=?
			op<OT_CHAR_GREATER_THEN_OR_EQUAL_TO> (mMachine);
			break;
		case OT_CHAR_TO_INTEGER:		   // char->integer
			op<OT_CHAR_TO_INTEGER> (mMachine);
			break;
		case OT_INTEGER_TO_CHAR:		   // integer->char
			op<OT_INTEGER_TO_CHAR> (mMachine);

			// 字符串操作

			break;
		case OT_IS_STRING:	// string?
			op<OT_IS_STRING> (mMachine);
			break;
		case OT_MAKE_STRING: // make-string
			op<OT_MAKE_STRING> (mMachine);
			break;
		case OT_STRING_LENGTH:
			op<OT_STRING_LENGTH> (mMachine);
			break;
		case OT_STRING_REF:
			op<OT_STRING_REF> (mMachine);
			break;
		case OT_STRING_SET:
			op<OT_STRING_SET> (mMachine);

			// 向量操作

			break;
		case OT_IS_VECTOR:	//
			op<OT_IS_VECTOR> (mMachine);
			break;
		case OT_MAKE_VECTOR:
			op<OT_MAKE_VECTOR> (mMachine);
			break;
		case OT_VECTOR_LENGTH:
			op<OT_VECTOR_LENGTH> (mMachine);
			break;
		case OT_VECTOR_REF:
			op<OT_VECTOR_REF> (mMachine);
			break;
		case OT_VECTOR_SET:
			op<OT_VECTOR_SET> (mMachine);

			// 控制特征

			break;
		case OT_IS_PROCEDURE:
			op<OT_IS_PROCEDURE> (mMachine);
			break;
		case OT_APPLY_PROC:
			op<OT_APPLY_PROC> (mMachine);
			break;
		case OT_EVAL:
			op<OT_EVAL> (mMachine);
			break;

		case OT_TOS_REG:
			op<OT_TOS_REG> (mMachine);
			break;
		case OT_TOS_MEMORY:
			op<OT_TOS_MEMORY> (mMachine);
			break;

		case OT_DISPLAY:
			op<OT_DISPLAY> (mMachine);
			break;
		case OT_NEWLINE:
			op<OT_NEWLINE> (mMachine);
			break;
		case OT_WRITE:
			op<OT_WRITE> (mMachine);
			break;
		case OT_READ:
			op<OT_READ> (mMachine);
			break;

			// 采用宏来简化处理
			HANDLE_OT(OT_IS_EOF_OBJECT);

			
			/*case OT_:
			  op<OT_> (mMachine);
			  break;*/

			// 
		case OT_SYNTAX_BEGIN:
		case OT_SYNTAX_END:
		default:
			throw RuntimeException (mMachine.getString (R_EXP)
						, "unknow primitive operator");
		}
	}


	/*
	//
	// TODO:
	// 
	// 后面这些应该都归为一条，evApplication，而这些关键字的功能只
	// 不过是内置基本操作的一个绑定罢了。这样用户才能重新定义这些关
	// 键字的意义。但这之前必须先实现环境模型。
	//

	
	// (set! var val)
	void Evaluator::evAssignment () {
	// 保存变量
	mMachine.cdr (R_EXP, R_EXP); // (var val)
	mMachine.car (R_UNEV,R_EXP); // var
	mMachine.save (R_UNEV);

	// 准备求值
	mMachine.cdr (R_EXP,R_EXP); // (val)
	mMachine.car (R_EXP,R_EXP); // val

	// 保存环境和继续
	mMachine.save (R_ENV);
	mMachine.save (R_CONTINUE);

	// 重新设置继续
	Atom set_eval = Atom::makePrimOp (OT_SET_EVAL);
	mMachine.assign (R_CONTINUE, set_eval);
	}

	// OT_SET_EVAL
	// 求值第二个操作数后返回到这里
	void Evaluator::evAssignmentEval () {
	// 恢复继续和环境
	mMachine.restore (R_CONTINUE);
	mMachine.restore (R_ENV);
	// 取出变量
	mMachine.restore (R_UNEV);
		
	// 修改约束
	mMachine.set (R_UNEV, R_VAL);

	// 返回
	mMachine.assign (R_EXP, R_CONTINUE);
	}

	// (define var val)
	void Evaluator::evDefinition () {
	// 保存变量
	mMachine.cdr (R_EXP,R_EXP); // (var val)
	mMachine.car (R_UNEV,R_EXP); // var
	mMachine.save (R_UNEV);

	// 准备求值
	mMachine.cdr (R_EXP,R_EXP); // (val)
	mMachine.car (R_EXP,R_EXP); // val

	// 保存环境和继续
	mMachine.save (R_ENV);
	mMachine.save (R_CONTINUE);
		
	// 设置继续
	Atom define_val = Atom::makePrimOp (OT_DEFINE_EVAL);
	mMachine.assign (R_CONTINUE, define_val);
	}

	// define-eval
	void Evaluator::evDefinitionEval () {
	// 恢复继续和环境
	mMachine.restore (R_CONTINUE);
	mMachine.restore (R_ENV);
	// 获取变量
	mMachine.restore (R_UNEV);
	// 定义
	mMachine.define (R_UNEV,R_VAL);
	// 设置返回值为变量名
	mMachine.assign (R_VAL,R_UNEV);
	// 返回
	mMachine.assign (R_EXP, R_CONTINUE);
	}

	void Evaluator::evIf () {
		
	op<OT_IF> (mMachine);

	}

	void Evaluator::evLambda () {
	op<OT_LAMBDA> (mMachine);
	}
	*/

	void Evaluator::evApplication () {
		mMachine.car (R_VAL, R_EXP);

		if (mMachine.reg (R_VAL).isSymbol().isTrue()) {
			// 如果是内置基本操作的话，求出其值
			mMachine.lookup (R_VAL);

			Atom op=mMachine.reg (R_VAL);
			if (op.isPrimOp().isTrue () && op.isSyntax()) {
				// 如果是语法性操作的话，不求值参数
				
				// 准备参数
				mMachine.cdr (R_UNEV, R_EXP);

				mMachine.assign (R_EXP, R_VAL);
			}else {
				// 如果是自定义的复合函数或非语法性内置
				// 操作的话，因为已求出操作符，所以直接
				// 到DID_EVAL_OPERATOR来求参数的值

				mMachine.save (R_CONTINUE);
				mMachine.save (R_ENV);
				// 保存参数到R_UNEV中
				mMachine.cdr (R_UNEV, R_EXP);
				mMachine.save (R_UNEV);

				// TODO:这时最好能直接跳到求值参数的循
				// 环中，就可以避免保存R_UNEV了
				Atom did = Atom::makePrimOp (OT_DID_EVAL_OPERATOR);
				mMachine.assign (R_EXP, did);
			}
		} else {
			// 若操作符不是符号，则为其它表达式，现在对它求
			// 值，然后再到DID_EVAL_OPERATOR来求参数值

			// 保存寄存器
			mMachine.save (R_CONTINUE);
			mMachine.save (R_ENV);

			// 保存操作数
			mMachine.cdr (R_UNEV, R_EXP);
			mMachine.save (R_UNEV);
		
			// 准备求值操作符
			mMachine.car (R_EXP, R_EXP);

			// 设置继续
			Atom didEvalOperator = Atom::makePrimOp (OT_DID_EVAL_OPERATOR);
			mMachine.assign (R_CONTINUE, didEvalOperator);
		}
	}

	
	OperationType Evaluator::getOpType () {
		mMachine.save (R_VAL);
		mMachine.car (R_EXP);
		Atom op = mMachine.reg (R_VAL);
		mMachine.restore (R_VAL);
		
		return op.getOpType ();
	}

	// 判断R_EXP中的对象类型是不是某种基本操作
	bool Evaluator::isType (OperationType ot) {
		Atom exp = mMachine.reg (R_EXP);
		bool result = (exp.isPrimOp().isTrue ()
			       &&(exp.getOpType () == ot));

		return result;
	}

	// 判断R_EX:(op ...)中的第一个元素:op的类型
	bool Evaluator::isOpType (OperationType ot) {
		Atom exp = mMachine.reg (R_EXP);
		bool result = false;
		if (exp.isPair ().isTrue ()) {
			mMachine.save (R_VAL);

			mMachine.car (R_EXP);
			exp = mMachine.reg (R_VAL);

			if (exp.isSymbol ().isTrue ()) {
				mMachine.lookup (R_VAL);
				exp = mMachine.reg (R_VAL);
				result = (exp.isPrimOp().isTrue ()
					  &&(exp.getOpType () == ot));
			}
			
			

			mMachine.restore (R_VAL);
		}
		return result;
	}


	void Evaluator::setupEnv () {
		// 建立基本操作名及其操作的映射关系
		//map<string,OperatorType> bindings;
		//bindings.bind ("lambda",OT_LAMBDA);
			

		// 扩展基本语法环境
		mMachine.assign (R_VAL, Atom::makeNull());
		mMachine.extend (R_VAL);

		
#define DEFINE_SYMBOL(str,op) do {		\
			mMachine.makeSymbol (str);			\
			mMachine.assign (R_ARGL, Atom::makePrimOp (op)); \
			mMachine.define (R_VAL, R_ARGL);		\
		} while (0)
			
		mMachine.makeSymbol ("lambda");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_LAMBDA));
		mMachine.define (R_VAL, R_ARGL);
			
		mMachine.makeSymbol ("if");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IF));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("quote");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_QUOTE));
		mMachine.define (R_VAL, R_ARGL);
			
		mMachine.makeSymbol ("define");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_DEFINE));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("begin");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_BEGIN));
		mMachine.define (R_VAL, R_ARGL);
			
		mMachine.makeSymbol ("set!");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_SET));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("and");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_AND));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("or");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_OR));
		mMachine.define (R_VAL, R_ARGL);

		
		// 扩展标准过程环境
		mMachine.assign (R_VAL, Atom::makeNull());
		mMachine.extend (R_VAL);

						
		mMachine.makeSymbol ("cons");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_CONS));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("car");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_CAR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("cdr");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_CDR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("set-car!");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_SETCAR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("set-cdr!");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_SETCDR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("pair?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_PAIR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("+");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_PLUS));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("-");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_MINUS));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("*");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_MULTI));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("/");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_DIV));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("=");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_EQUAL_TO));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("<");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_LESS_THEN));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol (">");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_GREATER_THEN));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("<=");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_LESS_THEN_OR_EQUAL_TO));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol (">=");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_GREATER_THEN_OR_EQUAL_TO));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("number?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_NUMBER));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("integer?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_INTEGER));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("number->string");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_NUMBER_TO_STRING));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("string->number");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_STRING_TO_NUMBER));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("eqv?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_EQV));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("eq?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_EQ));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("equal?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_EQUAL));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("not");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_NOT));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("boolean?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_BOOLEAN));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("pair?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_PAIR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("null?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_NULL));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("list?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_LIST));
		mMachine.define (R_VAL, R_ARGL);


		mMachine.makeSymbol ("length");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_LENGTH));
		mMachine.define (R_VAL, R_ARGL);
		

		mMachine.makeSymbol ("symbol?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_SYMBOL));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("symbol->string");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_SYMBOL_TO_STRING));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("string->symbol");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_STRING_TO_SYMBOL));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("char?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_CHAR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("char=?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_CHAR_EQUAL_TO));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("char<?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_CHAR_LESS_THEN));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("char>?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_CHAR_GREATER_THEN));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("char<=?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_CHAR_LESS_THEN_OR_EQUAL_TO));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("char>=?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_CHAR_GREATER_THEN_OR_EQUAL_TO));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("char->integer");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_CHAR_TO_INTEGER));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("integer->char");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_INTEGER_TO_CHAR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("string?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_STRING));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("make-string");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_MAKE_STRING));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("string-length");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_STRING_LENGTH));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("string-ref");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_STRING_REF));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("string-set!");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_STRING_SET));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("vector?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_VECTOR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("make-vector");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_MAKE_VECTOR));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("vector-length");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_VECTOR_LENGTH));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("vector-ref");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_VECTOR_REF));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("vector-set!");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_VECTOR_SET));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("procedure?");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_IS_PROCEDURE));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("apply");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_APPLY_PROC));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("eval");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_EVAL));
		mMachine.define (R_VAL, R_ARGL);

		
		mMachine.makeSymbol ("tos::register");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_TOS_REG));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("tos::memory");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_TOS_MEMORY));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("display");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_DISPLAY));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("newline");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_NEWLINE));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("write");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_WRITE));
		mMachine.define (R_VAL, R_ARGL);

		mMachine.makeSymbol ("read");
		mMachine.assign (R_ARGL,Atom::makePrimOp (OT_READ));
		mMachine.define (R_VAL, R_ARGL);

		
		DEFINE_SYMBOL ("eof-object?", OT_IS_EOF_OBJECT);
		
			
		// 扩展用户自定义环境
		mMachine.assign (R_VAL, Atom::makeNull());
		mMachine.extend (R_VAL);
	}


} // namespace
