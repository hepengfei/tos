// -*-c++-*-

#ifndef Atom_HPP
#define Atom_HPP


#include "common.hpp"
#include "Exception.hpp"


// namespace
namespace tos {


	
	// 
	// 基本类型定义
	//
	
	typedef long Integer;
	typedef char Char;
	typedef bool Boolean;
	typedef size_t Pointer;

	typedef unsigned long uInteger;



	// 
	// 基本操作类型枚举
	//
	
	enum OperationType {

		//
		// 语法性操作
		//

		OT_SYNTAX_BEGIN

		, OT_QUOTE
		, OT_LAMBDA
		, OT_IF
		, OT_SET
		, OT_DEFINE
		, OT_BEGIN

		, OT_AND
		, OT_AND_CONT
		, OT_OR
		, OT_OR_CONT

		, OT_IF_DECIDE
		, OT_SET_EVAL
		, OT_DEFINE_EVAL


		// 过程调用中
		// 求值操作符后的动作
		, OT_DID_EVAL_OPERATOR

		// 求值参数循环
		, OT_EVAL_OPERAND_LOOP
		// 求值最后一个参数
		, OT_EVAL_LAST_OPERAND
		// 积累参数值
		, OT_ACCUMULATE_OPERAND
		// 积累最后一个参数值
		, OT_ACCUMULATE_LAST

		// 函数应用
		, OT_APPLY

		// 序列求值
		, OT_EVAL_SEQUENCE

		// 序列求值的循环部分
		, OT_EVAL_SEQUENCE_LOOP

		// 序列求值结果
		, OT_EVAL_SEQUENCE_END

		// 求值操作完成的处理工作部分
		, OT_EVAL_DONE

		, OT_SYNTAX_END

		// 
		// 基本表操作
		//

		, OT_CONS
		, OT_CAR
		, OT_CDR
		, OT_SETCAR
		, OT_SETCDR

		// 
		// 基本数学操作
		//

		, OT_PLUS	// +
		, OT_MINUS	// -
		, OT_MULTI	// *
		, OT_DIV	// /

		, OT_EQUAL_TO	// =
		, OT_LESS_THEN	// <
		, OT_GREATER_THEN // >
		, OT_LESS_THEN_OR_EQUAL_TO // <=
		, OT_GREATER_THEN_OR_EQUAL_TO // >=


		, OT_IS_NUMBER	// number?
		, OT_IS_INTEGER	// integer?

		// , OT_MATH_SQRT // sqrt
		// , OT_MATH_SIN
		// ...

		, OT_NUMBER_TO_STRING // number->string
		, OT_STRING_TO_NUMBER // string->number

		, OT_IS_EQV	// eqv?
		, OT_IS_EQ	// eq?
		, OT_IS_EQUAL	// equal?

		// 布尔操作
		
		, OT_NOT	// not
		, OT_IS_BOOLEAN	// boolean?

		// 表操作

		, OT_IS_PAIR	// pair?
		, OT_IS_NULL	// null?
		, OT_IS_LIST	// list?
		, OT_LENGTH	// length

		// 符号操作

		, OT_IS_SYMBOL	// symbol?
		, OT_SYMBOL_TO_STRING // symbol->string
		, OT_STRING_TO_SYMBOL // string->symbol

		// 字符操作

		, OT_IS_CHAR	// char?
		, OT_CHAR_EQUAL_TO // char=?
		, OT_CHAR_LESS_THEN // char<?
		, OT_CHAR_GREATER_THEN // char>?
		, OT_CHAR_LESS_THEN_OR_EQUAL_TO // char<=?
		, OT_CHAR_GREATER_THEN_OR_EQUAL_TO // char>=?
		, OT_CHAR_TO_INTEGER		   // char->integer
		, OT_INTEGER_TO_CHAR		   // integer->char

		// 字符串操作

		, OT_IS_STRING	// string?
		, OT_MAKE_STRING // make-string
		, OT_STRING_LENGTH
		, OT_STRING_REF
		, OT_STRING_SET

		// 向量操作

		, OT_IS_VECTOR	//
		, OT_MAKE_VECTOR
		, OT_VECTOR_LENGTH
		, OT_VECTOR_REF
		, OT_VECTOR_SET

		// 控制特征

		, OT_IS_PROCEDURE
		, OT_APPLY_PROC
		, OT_EVAL

		// 调试系统用的过程
		, OT_TOS_REG	// 显示寄存器内容
		, OT_TOS_MEMORY // 显示指定区域的内存


		, OT_DISPLAY
		, OT_NEWLINE
		, OT_WRITE
		, OT_READ

		, OT_IS_EOF_OBJECT
		/* ...  */
	};


	// 
	// 值联合
	//
	// Scheme基本类型组成的联合，以保证对象大小的一致。其中Pointer
	// 是堆中内存单元的下标，所有复合对象（即基于Pair表示的）都是由
	// 它来指向实现的对象。具体的类型信息由Atom中的类型域提供。
	// 

	union Value {
		Integer mInteger; // 4Byte
		Pointer mPointer; // 4Byte
		OperationType mOT;
		Char mChar;	  // 1Byte
		Boolean mBoolean; // 1Bit
	};

	//
	// 类型 - 对象表示方式#[type value]
	//
	// 原子类型域值的定义
	//
	
	enum Type {

		// 
		// 标志对象，不属于类型系统。要保证这些标志值在一开始位
		// 置。
		// 

		// 破碎的心，在垃圾回收过程中用作标记对象，表示一个对象
		// 已经被拷贝。
		T_BROKENHEART	// #[broken-heart 0]

		// 文件结束对象
		, T_EOF		// #[eof 0]



		//
		// 基本类型
		// 
		
		, T_NULL	// #[null 0]
		, T_CHAR	// #[char a]
		, T_INTEGER	// #[integer 9]
		, T_BOOLEAN	// #[boolean true]

		// 内置实现的基本过程，是简单对象
		, T_PRIMITIVE_PROCEDURE // #[primitive-procedure op-enum]

		// 端口
		, T_PORT	// #[port file-description]


		//
		// 复合对象
		// 

		// 基本类型与复合类型的分界线
		, T_POINTER	// #[pointer 3]

		// 点对
		, T_PAIR	// #[pair 3]

		// 向量
		, T_VECTOR	// #[vector 3]

		// 字符串
		, T_STRING	// #[string 3]

		// 符号
		, T_SYMBOL	// #[symbol 3]

		// 过程
		, T_PROCEDURE	// #[procedure 3]
	};



	//
	// 原子
	//
	// Scheme中的基本对象
	//
	
	class Atom {

	public:
		//
		// 构造函数
		//
 
		Atom (Type pType, Value pValue )
			: mType(pType)
			, mValue(pValue) {}

		Atom () {}

		//
		// 类型判断
		//

		Atom isNull () const {
			return ((isType(T_NULL))
				? Atom::makeTrue ()
				: Atom::makeFalse());
		}

		Atom isNumber () const {
			// 因为只有整数这一种数字类型，所以直接返回。加
			// 入其它类型时要修改这里。
			return isInteger ();
		}

		Atom isInteger () const {
			return ((isType(T_INTEGER))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		Atom isChar () const {
			return ((isType(T_CHAR))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		Atom isBoolean () const {
			return ((isType(T_BOOLEAN))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		Atom isPointer () const {
			return ((getType() >= T_POINTER
						  && getType() <= T_PROCEDURE)
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		Atom isPair () const {
			return ((isType(T_PAIR))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		Atom isVector () const {
			return ((isType(T_VECTOR))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		Atom isString () const {
			return ((isType(T_STRING))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		Atom isSymbol () const {
			return ((isType(T_SYMBOL))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		Atom isProcedure () const {
			return ((isType(T_PROCEDURE))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		Atom isPort () const {
			return ((isType(T_PORT))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}
		
		Atom isPrimOp () const {
			return ((isType(T_PRIMITIVE_PROCEDURE))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		//
		// 检测是否垃圾回收标志
		//

		Atom isBrokenHeart () const {
			return ((isType (T_BROKENHEART))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		//
		// 检测是否文件结束标志
		//

		Atom isEof () const {
			return ((isType (T_EOF))
				? Atom::makeTrue ()
				: Atom::makeFalse ());
		}

		// 
		// 真值判断
		//
		// 必须先检测类型，否则可能出错。
		// 
 
		Boolean isTrue () const {
			return getType () != T_BOOLEAN // 非布尔类型的对象都认为是真
				|| getValue().mBoolean == true;
		}

		Boolean isFalse () const {
			return getType () == T_BOOLEAN
				&& getValue().mBoolean == false;
		}

		// 获取对象的字符串表示，如#Char #\a #Integer 32
		// #Boolean #t #Pointer 34 #[type] value
		string getString () const;

		//
		// 获取值
		//

		Char getChar () const {
			if (isChar ().isFalse ())
				throw CharExpected(getString ());
 
			return getValue().mChar;
		}

		Integer getInteger () const {
			if (isInteger ().isFalse ())
				throw IntegerExpected(getString ());
 
			return getValue().mInteger;
		}

		Boolean getBoolean () const {
			if (isBoolean ().isFalse ())
				throw BooleanExpected(getString ());
 
			return getValue().mBoolean;
		}

		OperationType getOpType () const {
			if (isPrimOp().isFalse ())
				throw PrimOpExpected (getString ());

			return getValue().mOT;
		}

		Pointer getPointer () const {
			if (isPointer ().isFalse ())
				throw CompoundExpected(getString ());
 
			return getValue().mPointer;
		}


		//
		// 相等比较
		//

		bool operator== (const Atom pOther) const {
			bool result = (pOther.getType () == getType ());

			if (! result)
				return false;

			switch (getType ()) {
				case T_BROKENHEART:
				case T_EOF:
				case T_NULL:
					result = true;
					break;
				case T_BOOLEAN:
					result = (pOther.getBoolean() == getBoolean());
					break;
				case T_CHAR:
					result = (pOther.getChar() == getChar());
					break;
				case T_INTEGER:
				case T_PRIMITIVE_PROCEDURE: // #[prim* op-enum]
				case T_PORT: // #[port fd]
					result = (pOther.getValue().mInteger == getValue().mInteger); // 
					break;
				default: // >= T_POINTER
					result = (pOther.getPointer() == getPointer());
				}
			
 			return result;
		}

		//
		// 工厂方法
		//

		// 空表
		static Atom makeNull () {
			Value v;
			v.mPointer = 0;
			return Atom (T_NULL,v);
		}

		/*static Atom makeBoolean (Boolean b) {
			Value v;
			v.mBoolean = b;
			return Atom (T_BOOLEAN,v);
			}*/

		static Atom makeTrue () {
			Value v;
			v.mBoolean = true;
			return Atom (T_BOOLEAN, v);
		}

		static Atom makeFalse () {
			Value v;
			v.mBoolean = false;
			return Atom (T_BOOLEAN, v);
		}

		static Atom makeBrokenHeart () {
			Value v;
			v.mPointer = 0;
			return Atom (T_BROKENHEART,v);
		}

		static Atom makeEof () {
			Value v;
			v.mPointer = 0;
			return Atom (T_EOF, v);
		}

		static Atom makePair (Pointer p) {
			Value v;
			v.mPointer = p;
			return Atom (T_PAIR,v);
		}

		static Atom makeChar (Char c) {
			Value v;
			v.mChar = c;
			return Atom (T_CHAR, v);
		}

		static Atom makeInteger (Integer i) {
			Value v;
			v.mInteger = i;
			return Atom (T_INTEGER, v);
		}

		static Atom makePrimOp (OperationType ot) {
			Value v;
			v.mOT = ot;
			return Atom (T_PRIMITIVE_PROCEDURE, v);
		}

		Type getType () const {
			return mType;
		}

		Atom toString () const {
			if (isPointer ().isFalse ())
				throw CompoundExpected (getString ());

			return Atom (T_STRING, mValue);
		}

		Atom toSymbol () const {
			if (isPointer ().isFalse ())
				throw CompoundExpected (getString ());

			return Atom (T_SYMBOL, mValue);
		}

		Atom toVector () const {
			if (isPointer ().isFalse ())
				throw CompoundExpected (getString ());

			return Atom (T_VECTOR, mValue);
		}

		Atom toProcedure () const {
			if (isPointer ().isFalse ())
				throw CompoundExpected (getString ());

			return Atom (T_PROCEDURE, mValue);
		}

		Atom &operator+= (Atom other) {
			Integer i = other.getInteger ();
			mValue.mInteger += i;
			return *this;
		}

		Atom &operator-= (Atom other) {
			Integer i = other.getInteger ();
			mValue.mInteger -= i;
			return *this;
		}

		Atom &operator*= (Atom other) {
			Integer i = other.getInteger ();
			mValue.mInteger *= i;
			return *this;
		}

		Atom &operator/= (Atom other) {
			Integer i = other.getInteger ();
			if (i == 0)
				throw DivisionByZeroException ();
			mValue.mInteger /= i;
			return *this;
		}

		// 是否语法性操作
		bool isSyntax () {
			OperationType ot=getOpType ();
			return (ot > OT_SYNTAX_BEGIN
				&& ot < OT_SYNTAX_END);
		}
				

		

	protected:
		//
		// 辅助操作
		//

		Value getValue () const {
			return mValue;
		}

		void setType (Type pType) {
			mType = pType;
		}

		// 比较类型域，测试是哪种类型
		Boolean isType (const Type pType) const {
			return getType () == pType;
		}

	private:

		Type mType;	// 类型域
		Value mValue;	// 值域 
	};
 
} // namespace

#endif // Atom_HPP
