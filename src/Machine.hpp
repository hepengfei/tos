// -*-c++-*-

#ifndef Machine_HPP
#define Machine_HPP


#include "common.hpp"
#include "Atom.hpp"
#include "Heap.hpp"
//#include "Trace.hpp"


// namespace
namespace tos {



	//
	// 寄存器机器的寄存器类型
	//
	
	enum Register{

		//
		// 七种基本寄存器
		//
		
		R_EXP = 0,
		R_ENV,
		R_VAL,
		R_CONTINUE,
		R_PROC,
		R_ARGL,
		R_UNEV,

		// 
		// 栈，也用寄存器来表示，便于垃圾回收操作
		//
		
		R_STACK,

		//
		// 寄存器总的数量
		// 

		R_NUM
	};

	//
	// 寄存器机器
	//
	// 这里所有操作都_必须_是通过寄存器来完成的，否则垃圾回收会导致
	// 对象丢失。
	// 

	class Machine {

	public:

		// 
		// 构造机器
		//
		// 需要指定堆大小。由于环境创建是基于寄存器机器的，因此
		// 在创建机器之前是不可能有环境的。因此不再需要传入环
		// 境，提供一套创建环境的接口即可。
		// 

		Machine (size_t pHeapSize);

		//
		// 单体模式
		//

		static Machine &instance ();

		//
		// 寄存器操作
		//

		// 获取寄存器内容
		Atom reg (Register r) const {
			return mRegisters [r];
		}

		// 设置寄存器内容
		void assign (Register r, Atom pValue) {
			mRegisters [r] = pValue;
		}

		// 用于寄存器间赋值
		void assign (Register r1, Register r2) {
			mRegisters [r1] = mRegisters [r2];
		}

		//
		// 基本表操作 - 基于寄存器操作和堆操作实现
		//
		
		//
		// 所有运算结果都保存在R_VAL寄存器中
		// 
	
		void cons (Register val, Register car, Register cdr) {
			checkSpace ();
			Atom value = mHeap.cons (reg (car), reg (cdr));
			assign (val, value);
		}

		void car (Register val, Register pValue) {
			Atom value = mHeap.car (reg (pValue));
			assign (val, value);
		}

		void cdr (Register val, Register pValue) {
			Atom value = mHeap.cdr (reg (pValue));
			assign (val, value);
		}

		void setCar (Register pair, Register val) {
			mHeap.setCar (reg (pair), reg (val));
		}

		void setCdr (Register pair, Register val) {
			mHeap.setCdr (reg (pair), reg (val));
		}

				
		//
		// 下面的操作将逐步退出历史舞台
		// 
		// 所有运算结果都保存在R_VAL寄存器中
		// 
	
		void cons (Register car, Register cdr) {
			checkSpace ();
			Atom value = mHeap.cons (reg (car), reg (cdr));
			assign (R_VAL, value);
		}

		void car (Register pValue) {
			Atom value = mHeap.car (reg (pValue));
			assign (R_VAL, value);
		}

		void cdr (Register pValue) {
			Atom value = mHeap.cdr (reg (pValue));
			assign (R_VAL, value);
		}

		
		// 
		// 栈操作 - 基于寄存器操作和堆操作实现
		//
		// 注意，在这里不能使用基本表操作，否则会产生矛盾。比
		// 如，保存一个寄存器的值时，不能破坏其它寄存器的值（包
		// 括R_VAL寄存器），而基本表操作就是基于寄存器的，必然
		// 会导致值的改变，因此产生矛盾。
		//

		void clear () {
			assign (R_STACK, Atom::makeNull ());
		}

		// 从异常中恢复栈中的保存的寄存器数据
		void recover () {
			while (! mRegs.empty ()) {
				restore (mRegs.back ());
				// 在restore中已经有了pop操作了，所以不必再写了
				//mRegs.pop_back ();
			}
			/*
			bool did[R_NUM] ={0};

			// 从前向后，每个寄存器只恢复一次
			for (vector<Register>::iterator it=mRegs.begin ();
			     it != mRegs.end (); ++ it) {
				if (! did[*it]) {
					restore (*it);
					did[*it] = true;
				}
			}
			mRegs.clear ();*/
		}

		// 保存寄存器内容
		void save (Register r) {
			cons (R_STACK, r, R_STACK);

			max_deep_stack ++;
			cur_item_stack ++;
			hook (r);

			/*
			checkSpace (); // 检查空间

			Atom value = mHeap.cons (reg (r), reg (R_STACK));
			
			assign (R_STACK, value);*/
		}

		// 恢复寄存器内容
		void restore (Register r) {
			checkStack (); // 检查栈是否为空
			car (r, R_STACK);
			cdr (R_STACK, R_STACK);

			cur_item_stack --;
			cancle ();
			/*
			// 出栈
			Atom stack = reg (R_STACK);
			assign (r, mHeap.car (stack));
			assign (R_STACK, mHeap.cdr (stack));
			*/
		}

		//
		// 创建各类型对象的操作，结果始终保存在R_VAL中
		//
		// 向量相应的表结构是([vector-length] [1st-atom] [2nd] ... [n-th])
		// 字符串对象与向量类似，符号对象与字符串相同，只是类型域不同。
		// 

		// 创建字符串对象
		void makeString (string pString);

		// 创建符号对象
		void makeSymbol (string pSymbol) {
			makeString (pSymbol); // 符号对象与字符串一样
			Atom result = reg (R_VAL).toSymbol (); // 只是类型不同
			assign (R_VAL, result);
		}

		// 几种关键字符号
		void makeQuote () {
			makeSymbol ("quote");
		}

		void makeQuasiQuote () {
			makeSymbol ("quasiquote");
		}

		void makeUnquote () {
			makeSymbol ("unquote");
		}

		void makeUnquoteSplicing () {
			makeSymbol ("unquote-splicing");
		}

		/*unused
		  void isList (Register pValue) {
		  Atom value = reg (pValue);

		  if (value.isNull ().isTrue ()) {
		  setReg (R_VAL, Atom::makeTrue ());
		  } else if (value.isPair ().isTrue ()) {
		  cdr (pValue);
		  isList (R_VAL);
		  } else {
		  setReg (R_VAL, Atom::makeFalse ());
		  }
		  }*/

		//
		// 将对象转换为外部表示
		//
		
		string getString (Atom pValue);

		string getString (Register r) {
			return getString (reg (r));
		}

		//
		// 相等谓词
		//

		void eq (Register a, Register b) {
			Atom result = (base_eq (reg (a), reg (b))
				       ? Atom::makeTrue ()
				       : Atom::makeFalse ());
			assign (R_VAL, result);
		}

		void eqv (Register a, Register b) {
			eq (a,b);
		}
	
		void equal (Register a, Register b) {
			Atom result = (base_equal (reg (a), reg (b))
				       ? Atom::makeTrue ()
				       : Atom::makeFalse ());
			assign (R_VAL, result);
		}

		//
		// 表操作
		//
		
		Atom isList (Register r) {
			Atom rest=reg (r);
			// 获取最后一个元素
			while(rest.isPair().isTrue()) {
				rest = mHeap.cdr(rest);
			}
			// 测试是否为空表
			return rest.isNull ();
		}

		// 表或非严格表的长度
		Atom length (Register r) {
			Integer n=0;
			Atom rest=reg (r);
			while(rest.isPair().isTrue()) {
				rest = mHeap.cdr(rest);
				++n;
			}
			
			if (rest.isNull().isFalse())
				// 此时为非严格表，长度要加1
				++n;
			return Atom::makeInteger(n);
		}

		//
		// 环境操作
		//

		void lookup (Register var) {
			if (reg(var).isSymbol().isFalse())
				throw SymbolExpected (getString(var));
			
			Atom binding = lookupBinding (reg (var), reg (R_ENV));
			
			assign (R_VAL, getBindingValue (binding));
		}

		void set (Register var, Register val) {
			if (reg(var).isSymbol().isFalse())
				throw SymbolExpected (getString(var));
			
			Atom mval = reg (val);

			Atom binding = lookupBinding (reg (var), reg (R_ENV));
			
			setBindingValue (binding, mval);
		}


		void extend (Register frame) {
			if (reg(frame).isPair().isFalse()
			    && reg(frame).isNull().isFalse())
				throw PairExpected (getString(frame));
			
			cons (R_ENV, frame, R_ENV);
		}

		void define (Register var, Register val) {
			if (reg(var).isSymbol().isFalse())
				throw SymbolExpected (getString(var));
			
			try {
				Atom mvar = reg (var);
				Atom mval = reg (val);
				// 没有创建操作，故可不涉及寄存器，以简
				// 化代码
				Atom frame = firstFrame (reg (R_ENV));
				Atom binding = lookupBindingInFrame (mvar, frame);
				setBindingValue (binding, mval);
			} catch (UnboundVariable/*Exception*/ &uv) {
				// 因为涉及到创建绑定（点对），因此所有
				// 操作必须通过寄存器来完成，否则可能导
				// 致数据丢失

				save (R_VAL);
				save (R_ARGL);

				// 创建绑定
				cons (R_ARGL, var, val);
				
				// 获取第一个框架
				car (R_VAL, R_ENV);

				// 加入新的绑定，生成了新的框架
				cons (R_VAL, R_ARGL, R_VAL);

				// 更新环境中的框架
				setCar (R_ENV, R_VAL);

				restore (R_ARGL);
				restore (R_VAL);
			}
		}


		static inline
		Atom emptyEnv () {
			return Atom::makeNull ();
		}

		Atom enclosing (Atom env) {
			return mHeap.cdr (env);
		}

		Atom firstFrame (Atom env) {
			return mHeap.car (env);
		}

		void setBindingValue (Atom binding, Atom val) {
			mHeap.setCdr (binding, val);
		}

		Atom getBindingValue (Atom binding) {
			return mHeap.cdr (binding);
		}

		Atom getBindingVariable (Atom binding) {
			return mHeap.car (binding);
		}

		void makeFrame (){//Register param, Register argl) {
			Register param = R_UNEV, argl = R_ARGL;

			// 保存参数，以便出错时显示
			save (param);
			save (argl);
			
			
			// 处理(variable1 ...)和
			// (variable1 ... varn . varn+1)形式的参数
			// 框架保存到R_VAL中
			assign (R_VAL,Atom::makeNull ());
			while (reg (param).isPair ().isTrue ()
			       && reg (argl).isPair ().isTrue ()) {
				save (param);
				save (argl);

				car (param, param);
				car (argl, argl);
				// 创建绑定
				cons (param, param, argl);
				// 加入框架
				cons (R_VAL, param, R_VAL);

				restore (argl);
				restore (param);

				cdr (param, param);
				cdr (argl, argl);
			}

			bool isOk = false;
			
			// 处理variable形式的参数或最后的varn+1部分
			if (reg (param).isSymbol ().isTrue ()) {
				// 直接将参数值的表绑定到变量上
				cons (param, param, argl);
				// 创建框架
				cons (R_VAL, param, R_VAL);

				isOk = true;
			} else
				isOk = (reg (param).isNull ().isTrue ()
					&& reg (argl).isNull ().isTrue ());
			

			restore (argl);
			restore (param);
			if (! isOk)
				throw RuntimeException (getString(argl)
							,"arglist cannot match formals "
							+getString (param));
		}
		
		// 在一个框架中找指定变量的约束并返回。
		Atom lookupBindingInFrame (Atom var, Atom frame) {
			for (Atom rest = frame, binding;
			     rest.isNull ().isFalse ();
			     rest = mHeap.cdr (rest)) {
				binding = mHeap.car (rest);
				if (base_eq (getBindingVariable (binding),
					     var)) {
					return binding;
				}
			}
			throw UnboundVariable (getString (var));
		}

		// 在整个环境中查找指定变量的约束并返回。
		Atom lookupBinding (Atom var, Atom env) {
			while ( ! base_eq (env, emptyEnv ())) {
				try {
					return lookupBindingInFrame (var, firstFrame (env));
				} catch (UnboundVariable &uv) {
					env = enclosing (env);
					continue;
				}

			}
			throw UnboundVariable (getString (var));
		}

		// 获取某一段内存的内容
		string getMemoryString (Integer start, Integer end) {
			return mHeap.getString ((Pointer)start, (Pointer)end);
		}


	protected:

		//
		// 基本的比较操作
		//
		
		bool base_eq (Atom v1, Atom v2);
		bool base_eqv (Atom v1, Atom v2) {
			return base_eq (v1, v2);
		}
		bool base_equal (Atom v1, Atom v2);

		// 
		// 检查空间是否足够分配，并在无可用空间时自动调用垃圾回
		// 收。堆耗尽时抛出异常。
		//
		// 由于空间不足时要保存寄存器到预留空间中，因此该操作只
		// 能在此类中实现
		//
		
		void checkSpace ();

		//
		// 检查栈是否可以执行出栈操作，若是空栈则抛出异常。
		// 
		
		void checkStack ();

		// 
		// 将点对的内容转换成字符串表示，不带括号和前后的空格输
		// 入内容形式为以下几种：
		// () (a) (a . b)  (a b c . d) (a b c d e)
		//
		
		string getPairString (Atom pPair);

		// 
		// 传入字符的表，输出对应的字符串。trans表示是否转义，
		// 因为处理的是符号的话就不需要转义字符。
		//
		
		string getStrString (Atom pList, bool trans);

		// 用于记录栈操作，以便异常时恢复状态
		void cancle () {
			mRegs.pop_back ();
		}
		
		void hook (Register reg) {
			mRegs.push_back (reg);
		}
	
	private:
		Heap mHeap;		 // 堆空间
		Atom mRegisters [R_NUM]; // 寄存器组

		// 栈名称记录表，记录栈中元素是哪一个寄存器的
		vector <Register> mRegs;

		int max_deep_stack;
		int cur_item_stack;
	};

	
} // namespace

#endif	// Machine_HPP
