// -*-c++-*-

#ifndef PrimitiveProcedure_HPP
#define PrimitiveProcedure_HPP



#include "Atom.hpp"
#include "Machine.hpp"
#include "Parser.hpp"
#include "Exception.hpp"


// namespace
namespace tos {


	//
	// 基本操作定义
	//
	//
	
	template <OperationType ot>
	struct Operation ;
	

	//
	// 提供方便调用基本操作的方式
	//

	template <OperationType ot>
	static inline
	void op (Machine &m) {
		Operation <ot> op;
		op (m);
	}


	//
	// 对过程调用的参数个数进行检查
	//

	template <OperationType ot>
	struct ArityLimit;

	class Arity {
		
	public:
		Arity (int min, int max)
			: mMin(min), mMax(max) {}

		// 若上限设为这个值，就忽略上限。
		enum { A_DISABLE = -1 };

		
		bool isValid (int arity) {
			bool enough=(mMin <= arity);
			bool toomany=((mMax != A_DISABLE)
				      && (arity > mMax));
			return (enough && !toomany);
		}

		string toString () const {
			stringstream result;
			result << mMin;
			if (mMax==A_DISABLE)
				result << " or more";
			else if(mMin!=mMax)
				result << " to " << mMax;

			return result.str ();
		}

		// 检查Machine中的指定寄存器中的参数是否合乎要求
		template <OperationType ot>
		static inline
		void check (Register r, Machine &m) {
			Arity ar(ArityLimit<ot>::min
				 ,ArityLimit<ot>::max);
			Integer length=m.length (r).getInteger ();
			if (! ar.isValid (length))
				throw ArityException (m.getString(r)
						      ,ar.toString ()+" argument(s)");
		}

	private:
		// 参数个数的上限与下限，A_DISABLE为无限制
		const int mMin;
		const int mMax;
	};

	
	//
	// 下面使用偏特化来实现各个操作
	//

	
	// 语法性操作传入的参数在R_UNEV中

	// (quote obj)
	template <>
	struct ArityLimit <OT_QUOTE> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_QUOTE> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_QUOTE> (R_UNEV,pMachine);
			
			pMachine.car (R_VAL,R_UNEV);
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (set! var exp)
	template <>
	struct ArityLimit <OT_SET> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_SET> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_SET> (R_UNEV,pMachine);
			
			// 保存变量
			pMachine.car (R_ARGL,R_UNEV); // var
			pMachine.save (R_ARGL);

			// 准备求值
			pMachine.cdr (R_UNEV,R_UNEV); // (exp)
			pMachine.car (R_EXP,R_UNEV); // exp

			// 保存环境和继续
			pMachine.save (R_ENV);
			pMachine.save (R_CONTINUE);

			// 重新设置继续
			Atom set_eval = Atom::makePrimOp (OT_SET_EVAL);
			pMachine.assign (R_CONTINUE, set_eval);
		}
	};

	template <>
	struct Operation <OT_SET_EVAL> {
		void operator() (Machine &pMachine) {
			// 恢复继续和环境
			pMachine.restore (R_CONTINUE);
			pMachine.restore (R_ENV);
			// 取出变量
			pMachine.restore (R_ARGL);
		
			// 修改约束
			pMachine.set (R_ARGL, R_VAL);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (define var exp)
	template <>
	struct ArityLimit <OT_DEFINE> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_DEFINE> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_DEFINE> (R_UNEV,pMachine);
			
			// 保存变量到R_ARGL中
			pMachine.car (R_ARGL,R_UNEV); // var
			pMachine.save (R_ARGL);

			// 保存环境和继续
			pMachine.save (R_ENV);
			pMachine.save (R_CONTINUE);
		
			// 准备求值第三个操作符，即变量的值表达式
			pMachine.cdr (R_UNEV,R_UNEV); // (exp)
			pMachine.car (R_EXP,R_UNEV);  // exp

			// 设置继续
			Atom define_val = Atom::makePrimOp (OT_DEFINE_EVAL);
			pMachine.assign (R_CONTINUE, define_val);
		}
	};

	template <>
	struct ArityLimit <OT_BEGIN> {
		enum { min = 1,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_BEGIN> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_BEGIN> (R_UNEV,pMachine);

			// 将表达式序列保存到R_UNEV中
			// 本来就在R_UNEV中

			// 保存继续
			pMachine.save (R_CONTINUE);
			// 保存环境
			pMachine.save (R_ENV);

			// 立即转到求值序列的部分
			Atom eval = Atom::makePrimOp (OT_EVAL_SEQUENCE);
			pMachine.assign (R_EXP, eval);
		}
	};

	template <>
	struct Operation <OT_DEFINE_EVAL> {
		void operator() (Machine &pMachine) {
			// 恢复继续和环境
			pMachine.restore (R_CONTINUE);
			pMachine.restore (R_ENV);
			// 获取变量名
			pMachine.restore (R_ARGL);
			// 定义绑定
			pMachine.define (R_ARGL,R_VAL);
			// 设置返回值为变量名
			pMachine.assign (R_VAL,R_ARGL);
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (if predicate consequent alternate)
	// (if predicate consequent)
	template <>
	struct ArityLimit <OT_IF> {
		enum { min = 2,
		       max = 3 };
	};
	template <>
	struct Operation <OT_IF> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IF> (R_UNEV,pMachine);

			// 准备求值谓词
			pMachine.car (R_EXP, R_UNEV);
			// 从参数中去掉谓词部分
			pMachine.cdr (R_UNEV, R_UNEV);

			// 保存剩余参数、环境和继续
			pMachine.save (R_UNEV);
			pMachine.save (R_ENV);
			pMachine.save (R_CONTINUE);

			// 设置求值谓词后返回的地方
			Atom decide = Atom::makePrimOp (OT_IF_DECIDE);
			pMachine.assign (R_CONTINUE, decide);
		}
	};

	template <>
	struct Operation <OT_IF_DECIDE> {
		void operator() (Machine &pMachine) {
			// 恢复继续和环境、剩余参数
			pMachine.restore (R_CONTINUE);
			pMachine.restore (R_ENV);
			pMachine.restore (R_UNEV);

			// R_UNEV:(c a) / (c)
			if (pMachine.reg (R_VAL).isTrue ()) {
				// 谓词为真，准备求值后继表达式
				pMachine.car (R_EXP, R_UNEV); // c
			} else {
				// 否则求值替代表达式
				pMachine.cdr (R_EXP, R_UNEV); // (a) | ()

				if (pMachine.reg (R_EXP)
				    .isPair ().isTrue ()) // 有替代表达式
					pMachine.car (R_EXP, R_EXP); // a
				else
					// 无替代表达式，提前返回
					pMachine.assign (R_EXP, R_CONTINUE);
			}
		}
	};


	// (lambda formals body) body:=exp ...
	template <>
	struct ArityLimit <OT_LAMBDA> {
		enum { min = 2,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_LAMBDA> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_LAMBDA> (R_UNEV,pMachine);

			// 保存函数体
			pMachine.cdr (R_EXP, R_UNEV); // (exp ...)
			
			// 获取lambda参数部分，放到R_UNEV中
			pMachine.car (R_UNEV, R_UNEV); // formals

			// 创建过程对象 (formals body env)
			pMachine.assign (R_VAL, Atom::makeNull ()); // ()
			pMachine.cons (R_VAL, R_ENV, R_VAL); // (env)
			pMachine.cons (R_VAL, R_EXP, R_VAL); // (body env)
			pMachine.cons (R_VAL, R_UNEV, R_VAL); // (formals body env)
			// 转换类型
			Atom p = pMachine.reg (R_VAL);
			pMachine.assign (R_VAL, p.toProcedure ());

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (and test1 ...)
	template <>
	struct ArityLimit <OT_AND> {
		enum { min = 0,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_AND> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_AND> (R_UNEV, pMachine);

			// 若参数为空，则值为真
			if (pMachine.reg (R_UNEV).isNull ().isTrue ()) {
				pMachine.assign (R_VAL, Atom::makeTrue ());
				// 返回
				pMachine.assign (R_EXP, R_CONTINUE);
			} else {
				// 准备第一个表达式
				pMachine.car (R_EXP, R_UNEV);

				// 准备剩余表达式
				pMachine.cdr (R_UNEV, R_UNEV);

				// 看是否只有一个表达式
				// 只有一个，则不需要保存继续，直接对其求值即可
				if (pMachine.reg (R_UNEV).isNull().isFalse()) {
					// 不止一个，则保存继续，求值以转到后面的部分
					pMachine.save (R_CONTINUE);
					// 保存剩余表达式
					pMachine.save (R_UNEV);
				
					// 设置继续
					Atom cont=Atom::makePrimOp (OT_AND_CONT);
					pMachine.assign (R_CONTINUE, cont);
				}
			}
		}
	};
	template <>
	struct Operation <OT_AND_CONT> {
		void operator() (Machine &pMachine) {
			// 取出表达式
			pMachine.restore (R_UNEV);

			// 若上一次计算结果为假，则直接返回即可
			if (pMachine.reg(R_VAL).isFalse ()) {
				pMachine.restore (R_CONTINUE);
				pMachine.assign (R_EXP,R_CONTINUE);
				return;
			}
			// 否则

			// 准备下一个表达式
			pMachine.car (R_EXP, R_UNEV);

			// 准备剩余表达式
			pMachine.cdr (R_UNEV, R_UNEV);

			// 看是否到达最后一个表达式
			if (pMachine.reg (R_UNEV).isNull().isTrue()) {
				// 若是最后一个，则恢复继续，并对其求值
				pMachine.restore (R_CONTINUE);
			} else {
				// 若还有，则设置继续，以便求值后返回到这里
				Atom cont=Atom::makePrimOp (OT_AND_CONT);
				pMachine.assign (R_CONTINUE, cont);
				// 保存剩余表达式
				pMachine.save (R_UNEV);
			}
		}
	};

	// (or test1 ...)
	template <>
	struct ArityLimit <OT_OR> {
		enum { min = 0,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_OR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_OR> (R_UNEV, pMachine);

			// 若参数为空，则值为假
			if (pMachine.reg (R_UNEV).isNull ().isTrue ()) {
				pMachine.assign (R_VAL, Atom::makeFalse ());
				// 返回
				pMachine.assign (R_EXP, R_CONTINUE);
			} else {
				// 准备第一个表达式
				pMachine.car (R_EXP, R_UNEV);

				// 准备剩余表达式
				pMachine.cdr (R_UNEV, R_UNEV);

				// 看是否只有一个表达式
				// 只有一个，则不需要保存继续，直接对其求值即可
				if (pMachine.reg (R_UNEV).isNull().isFalse()) {
					// 不止一个，则保存继续，求值以转到后面的部分
					pMachine.save (R_CONTINUE);
					// 保存剩余表达式
					pMachine.save (R_UNEV);
				
					// 设置继续
					Atom cont=Atom::makePrimOp (OT_OR_CONT);
					pMachine.assign (R_CONTINUE, cont);
				}
			}
		}
	};
	template <>
	struct Operation <OT_OR_CONT> {
		void operator() (Machine &pMachine) {
			// 取出表达式
			pMachine.restore (R_UNEV);

			// 若上一次计算结果为真，则直接返回即可
			if (pMachine.reg(R_VAL).isTrue ()) {
				pMachine.restore (R_CONTINUE);
				pMachine.assign (R_EXP,R_CONTINUE);
				return;
			}
			// 否则
			
			// 准备下一个表达式
			pMachine.car (R_EXP, R_UNEV);

			// 准备剩余表达式
			pMachine.cdr (R_UNEV, R_UNEV);

			// 看是否到达最后一个表达式
			if (pMachine.reg (R_UNEV).isNull().isTrue()) {
				// 若是最后一个，则恢复继续，并对其求值
				pMachine.restore (R_CONTINUE);
			} else {
				// 若还有，则设置继续，以便求值后返回到这里
				Atom cont=Atom::makePrimOp (OT_OR_CONT);
				pMachine.assign (R_CONTINUE, cont);
				// 保存剩余表达式
				pMachine.save (R_UNEV);
			}
		}
	};

	

	// 操作符求值完成，转而求操作数
	template <>
	struct Operation <OT_DID_EVAL_OPERATOR> {
		void operator() (Machine &pMachine) {
			// 恢复操作数和环境
			pMachine.restore (R_UNEV);
			pMachine.restore (R_ENV);

			// 保存运算符
			pMachine.assign (R_PROC, R_VAL);

			// 准备求值参数表
			pMachine.assign (R_ARGL, Atom::makeNull());

			// 看有没有参数
			bool noOperands = pMachine.reg (R_UNEV).isNull ().isTrue ();

			if (noOperands) {
				// 没有参数，则直接转到求表达式的过程
				Atom apply = Atom::makePrimOp(OT_APPLY);
				pMachine.assign (R_EXP, apply);
			} else {
				// 有参数，则保存运算符，并对参数求值
				pMachine.save (R_PROC);

				// 跳到求值参数循环中
				Atom loop = Atom::makePrimOp(OT_EVAL_OPERAND_LOOP);
				pMachine.assign (R_EXP, loop);

			}
		}
	};



	template <>
	struct Operation<OT_EVAL_OPERAND_LOOP> {
		void operator() (Machine &pMachine) {

			pMachine.save (R_ARGL);
			
			// 先准备求值第一个参数
			pMachine.car (R_EXP, R_UNEV);

			// 取出后面的参数
			pMachine.cdr (R_UNEV, R_UNEV);

			// 测试是不是最后一个参数
			bool islast = pMachine.reg (R_UNEV).isPair().isFalse();
			if (islast) {
				// 是最后一个参数

				// 将其存到R_UNEV中
				pMachine.assign (R_UNEV, R_EXP);
				
				// 转到处理最后一个参数
				Atom last = Atom::makePrimOp(OT_EVAL_LAST_OPERAND);
				pMachine.assign (R_EXP, last);
			} else {
				// 依次处理参数

				// 保存环境和剩余参数
				pMachine.save (R_ENV);
				pMachine.save (R_UNEV);
				
				// 设置继续，以求值后面的参数
				Atom accu = Atom::makePrimOp(OT_ACCUMULATE_OPERAND);
				pMachine.assign (R_CONTINUE,accu);
			}
		}
	};


		
	template <>
	struct Operation <OT_EVAL_LAST_OPERAND> {
		void operator() (Machine &pMachine) {
			// 将最后一个参数放到R_EXP中，以求值
			pMachine.assign (R_EXP, R_UNEV);
			// 设置继续
			Atom acc=Atom::makePrimOp (OT_ACCUMULATE_LAST);
			pMachine.assign (R_CONTINUE,acc);
		}
	};
	
	template <>
	struct Operation <OT_ACCUMULATE_LAST> {
		void operator() (Machine &pMachine) {
			// 取出前面的参数值
			pMachine.restore (R_ARGL);
			// 加入最后一个
			pMachine.cons (R_VAL, R_VAL, R_ARGL);
			// 调整参数顺序
			//pMachine.reverse (R_ARGL,R_VAL);
			pMachine.assign (R_ARGL,Atom::makeNull());
			for (;pMachine.reg (R_VAL).isNull ().isFalse();
			     pMachine.cdr (R_VAL,R_VAL)) {
				pMachine.car (R_PROC,R_VAL);
				pMachine.cons (R_ARGL, R_PROC, R_ARGL);
			}

			// 取出操作符
			pMachine.restore (R_PROC);
			// 转到调用函数上去
			Atom apply=Atom::makePrimOp(OT_APPLY);
			pMachine.assign (R_EXP, apply);
		}
	};

	template <>
	struct Operation <OT_ACCUMULATE_OPERAND> {
		void operator() (Machine &pMachine) {
			pMachine.restore (R_UNEV);
			pMachine.restore (R_ENV);
			pMachine.restore (R_ARGL);
			pMachine.cons (R_ARGL, R_VAL, R_ARGL);

			Atom loop=Atom::makePrimOp(OT_EVAL_OPERAND_LOOP);
			pMachine.assign (R_EXP, loop);
		}
	};

	template <>
	struct Operation <OT_APPLY> {
		void operator() (Machine &pMachine) {
			Atom exp=pMachine.reg (R_PROC);
			if (exp.isPrimOp ().isTrue ()) {
				// if primitive
				pMachine.assign (R_EXP, R_PROC);
				pMachine.restore (R_CONTINUE);
			} else if (exp.isProcedure ().isTrue ()) {
				// if compound (formal body env)
				// 保存环境
				pMachine.save (R_ENV);
				
				// 取出参数表
				pMachine.car (R_UNEV, R_PROC);
				// 绑定参数表和值
				pMachine.makeFrame ();
				// 取出函数体
				pMachine.cdr (R_PROC, R_PROC);
				pMachine.car (R_UNEV, R_PROC);
				// 取出环境
				pMachine.cdr (R_PROC, R_PROC);
				pMachine.car (R_ENV, R_PROC);

				// 扩展环境
				pMachine.extend (R_VAL);

				// 跳转到序列求值部分
				Atom sequence=Atom::makePrimOp (OT_EVAL_SEQUENCE);
				pMachine.assign (R_EXP, sequence);
			} else {
				// 即不是基本过程也不是复合过程，报错
				throw RuntimeException (pMachine.getString(R_PROC)
							,"non-applicable exception");
			}
		}
	};


	template <>
	struct Operation <OT_EVAL_SEQUENCE> {
		void operator() (Machine &pMachine) {
			// 从R_UNEV取出第一个表达式，准备求值
			pMachine.car (R_EXP, R_UNEV);

			// 取出剩余的表达式
			pMachine.cdr (R_UNEV, R_UNEV);
			bool islast=pMachine.reg (R_UNEV).isNull().isTrue();
			if (islast) {
				// 求值最后一个表达式
				//pMachine.restore (R_CONTINUE);
				Atom last=Atom::makePrimOp (OT_EVAL_SEQUENCE_END);
				pMachine.assign (R_CONTINUE,last);
			} else {
				// 保存起剩余表达式和环境
				pMachine.save (R_UNEV);
				pMachine.save (R_ENV);

				// 设置求值后的返回地方
				Atom seq=Atom::makePrimOp (OT_EVAL_SEQUENCE_LOOP);
				pMachine.assign (R_CONTINUE,seq);
			}
		}
	};

	template <>
	struct Operation <OT_EVAL_SEQUENCE_LOOP> {
		void operator() (Machine &pMachine) {
			// 恢复环境和剩余表达式
			pMachine.restore (R_ENV);
			pMachine.restore (R_UNEV);
			
			Atom seq=Atom::makePrimOp (OT_EVAL_SEQUENCE);
			pMachine.assign (R_EXP,seq);
		}
	};

	
	template <>
	struct Operation <OT_EVAL_SEQUENCE_END> {
		void operator() (Machine &pMachine) {
			// 恢复环境
			pMachine.restore (R_ENV);
			// 直接返回
			pMachine.restore (R_EXP); // CONTINUE
		}
	};


	// /////////////////



	// (cons a b)
	template <>
	struct ArityLimit <OT_CONS> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_CONS> {
		void operator() (Machine &pMachine) {
			Arity::check <OT_CONS> (R_ARGL, pMachine);
			
			pMachine.car (R_VAL, R_ARGL);
			
			pMachine.cdr (R_ARGL,R_ARGL);
			pMachine.car (R_ARGL,R_ARGL);

			pMachine.cons (R_VAL, R_VAL, R_ARGL);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (car pair) =>R_ARGL=> (pair)	
	template <>
	struct ArityLimit <OT_CAR> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_CAR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_CAR> (R_ARGL, pMachine);

			pMachine.car (R_VAL, R_ARGL); // pair
			pMachine.car (R_VAL, R_VAL);  // pair's car
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (cdr pair) =>R_ARGL=> (pair)
	template <>
	struct ArityLimit <OT_CDR> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_CDR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_CDR> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL); // pair
			pMachine.cdr (R_VAL,R_VAL);
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (set-car! pair obj)
	// =>R_ARGL=> (pair obj)
	template <>
	struct ArityLimit <OT_SETCAR> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_SETCAR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_SETCAR> (R_ARGL,pMachine);
			pMachine.car (R_EXP, R_ARGL); // pair
			pMachine.cdr (R_ARGL, R_ARGL);	   // (obj)
			pMachine.car (R_ARGL, R_ARGL);	   // obj
			
			pMachine.setCar (R_EXP, R_ARGL);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (set-cdr! pair obj)
	// ->R_ARGL=> (pair obj)
	template <>
	struct ArityLimit <OT_SETCDR> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_SETCDR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_SETCDR> (R_ARGL, pMachine);

			pMachine.car (R_EXP, R_ARGL); // pair
			pMachine.cdr (R_ARGL, R_ARGL);	   // (obj)
			pMachine.car (R_ARGL, R_ARGL);	   // obj
			
			pMachine.setCar (R_EXP, R_ARGL);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};


	//
	// 数学操作实现
	// 

	// (+ ...) =>R_ARGL=> (...)
	template <>
	struct ArityLimit <OT_PLUS> {
		enum { min = 0,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_PLUS> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_PLUS> (R_ARGL, pMachine);

			Atom sum = Atom::makeInteger((Integer)0);
			
			while (pMachine.reg (R_ARGL).isNull().isFalse()) {
				pMachine.car (R_VAL, R_ARGL);  // get first
				pMachine.cdr (R_ARGL, R_ARGL); // get rest
				sum += pMachine.reg (R_VAL);
			}

			pMachine.assign (R_VAL, sum);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (- z1 ...) =>R_ARGL=> (z1 ...)
	template <>
	struct ArityLimit <OT_MINUS> {
		enum { min = 1,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_MINUS> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_MINUS> (R_ARGL, pMachine);

			Atom sum;

			pMachine.car (R_VAL, R_ARGL);  // get first
			pMachine.cdr (R_ARGL, R_ARGL); // get rest
			
			if (pMachine.reg (R_ARGL).isNull().isTrue()) {
				// 只有一个参数时，返回-z1
				sum=Atom::makeInteger((Integer)0);
				sum -= pMachine.reg (R_VAL);
			} else {
				sum = pMachine.reg (R_VAL);

				while (pMachine.reg(R_ARGL).isNull ().isFalse ()) {
					pMachine.car (R_VAL,R_ARGL);
					pMachine.cdr(R_ARGL, R_ARGL);
					sum -= pMachine.reg (R_VAL);
				}
			}

			pMachine.assign (R_VAL, sum);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (* ...)
	template <>
	struct ArityLimit <OT_MULTI> {
		enum { min = 0,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_MULTI> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_MULTI> (R_ARGL, pMachine);
			
			Atom result = Atom::makeInteger((Integer)1);
			
			while (pMachine.reg (R_ARGL).isNull().isFalse()) {
				pMachine.car (R_VAL, R_ARGL);  // get first
				pMachine.cdr (R_ARGL, R_ARGL); // get rest
				result *= pMachine.reg (R_VAL);
			}

			pMachine.assign (R_VAL, result);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (/ z1 ...)
	template <>
	struct ArityLimit <OT_DIV> {
		enum { min = 1,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_DIV> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_DIV> (R_ARGL, pMachine);

			Atom result;

			pMachine.car (R_VAL, R_ARGL);  // get first
			pMachine.cdr (R_ARGL, R_ARGL); // get rest
			
			if (pMachine.reg (R_ARGL).isNull().isTrue()) {
				// 只有一个参数时，返回-z1
				result=Atom::makeInteger((Integer)1);
				result /= pMachine.reg (R_VAL);
			} else {
				result = pMachine.reg (R_VAL);

				while (pMachine.reg(R_ARGL).isNull ().isFalse ()) {
					pMachine.car (R_VAL,R_ARGL);
					pMachine.cdr(R_ARGL, R_ARGL);
					result /= pMachine.reg (R_VAL);
				}
			}

			pMachine.assign (R_VAL, result);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (= ...)
	template <>
	struct ArityLimit <OT_EQUAL_TO> {
		enum { min = 0,	// 没有参数或只有一个时，结果为真
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_EQUAL_TO> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_EQUAL_TO> (R_ARGL, pMachine);

			bool result=true;
			Integer n;
			if (pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				n = pMachine.reg (R_VAL).getInteger ();
			}

			while (result
			       && pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				result = (n == pMachine.reg (R_VAL).getInteger ());
			}

			Atom val=(result ? Atom::makeTrue() : Atom::makeFalse ());
						 
			pMachine.assign (R_VAL, val);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (< ...)
	template <>
	struct ArityLimit <OT_LESS_THEN> {
		enum { min = 0,	// 没有参数或只有一个时，结果为真
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_LESS_THEN> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_LESS_THEN> (R_ARGL, pMachine);

			bool result=true;
			Integer n;
			if (pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				n = pMachine.reg (R_VAL).getInteger ();
			}

			Integer next;
			while (result
			       && pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				next = pMachine.reg (R_VAL).getInteger ();
				result = (n < next);
				n = next;
			}

			Atom val=(result ? Atom::makeTrue() : Atom::makeFalse ());
						 
						 
			pMachine.assign (R_VAL, val);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (> x1 ...)
	template <>
	struct ArityLimit <OT_GREATER_THEN> {
		enum { min = 0,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_GREATER_THEN> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_GREATER_THEN> (R_ARGL, pMachine);

			bool result=true;
			Integer n;
			if (pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				n = pMachine.reg (R_VAL).getInteger ();
			}

			Integer next;
			while (result
			       && pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				next = pMachine.reg (R_VAL).getInteger ();
				result = (n > next);
				n = next;
			}

			Atom val=(result ? Atom::makeTrue() : Atom::makeFalse ());
						 
						 
			pMachine.assign (R_VAL, val);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (<= x1 ...)
	template <>
	struct ArityLimit <OT_LESS_THEN_OR_EQUAL_TO> {
		enum { min = 0,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_LESS_THEN_OR_EQUAL_TO> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_LESS_THEN_OR_EQUAL_TO> (R_ARGL, pMachine);

			bool result=true;
			Integer n;
			if (pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				n = pMachine.reg (R_VAL).getInteger ();
			}

			Integer next;
			while (result
			       && pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				next = pMachine.reg (R_VAL).getInteger ();
				result = (n <= next);
				n = next;
			}

			Atom val=(result ? Atom::makeTrue() : Atom::makeFalse ());
						 
						 
			pMachine.assign (R_VAL, val);
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (>= x1 ...)
	template <>
	struct ArityLimit <OT_GREATER_THEN_OR_EQUAL_TO> {
		enum { min = 0,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_GREATER_THEN_OR_EQUAL_TO> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_GREATER_THEN_OR_EQUAL_TO> (R_ARGL, pMachine);
			
			bool result=true;
			Integer n;
			if (pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				n = pMachine.reg (R_VAL).getInteger ();
			}

			Integer next;
			while (result
			       && pMachine.reg (R_ARGL).isNull ().isFalse()) {
				pMachine.car (R_VAL,R_ARGL);
				pMachine.cdr (R_ARGL,R_ARGL);
				next = pMachine.reg (R_VAL).getInteger ();
				result = (n >= next);
				n = next;
			}

			Atom val=(result ? Atom::makeTrue() : Atom::makeFalse ());
						 
			pMachine.assign (R_VAL, val);
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};


	// (number? obj)
	template <>
	struct ArityLimit <OT_IS_NUMBER> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_NUMBER> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_NUMBER> (R_ARGL, pMachine);

			pMachine.car (R_EXP, R_ARGL); // (obj)

			Atom obj=pMachine.reg (R_EXP);
			pMachine.assign (R_VAL, obj.isNumber ());

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (integer? obj)
	template <>
	struct ArityLimit <OT_IS_INTEGER> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_INTEGER> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_INTEGER> (R_ARGL, pMachine);

			pMachine.car (R_EXP, R_ARGL); // (obj)

			Atom obj=pMachine.reg (R_EXP);
			pMachine.assign (R_VAL, obj.isInteger ());

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (number->string z)
	// 这个没有实现：(number->string z radix)
	template <>
	struct ArityLimit <OT_NUMBER_TO_STRING> {
		enum { min = 1,
		       max = 2 };
	};
	template <>
	struct Operation <OT_NUMBER_TO_STRING> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_NUMBER_TO_STRING> (R_ARGL, pMachine);

			// 获取数字
			pMachine.car (R_VAL, R_ARGL);
			Integer number=pMachine.reg (R_VAL).getInteger ();

			// 获取基数
			Integer radix=10;
			pMachine.cdr (R_VAL, R_ARGL);
			if (pMachine.reg (R_VAL).isNull ().isFalse ()) {
				pMachine.car (R_VAL, R_VAL);
				radix=pMachine.reg (R_VAL).getInteger ();
			}

			// 按照基数将数字转换成字符串
			stringstream buffer;
			switch (radix) {
			case 2:
			{
				buffer << "#b";
				uInteger n=number;
				bool bit=false;
				while (n) {
					bit = (n & 0x1);
					buffer << (bit ? '1' : '0');
					n = n>>1;
				}
				if (bit==false)
					// 如果bit为假，则n一开始即为0。因为最后一个输出位一定是1
					buffer << '0';
				break;
			}
			case 8:
				buffer.setf (ios::oct,ios::basefield);
				buffer << "#o" << number;
				break;
			case 10:
				buffer.setf (ios::dec,ios::basefield);
				buffer << number;
				break;
			case 16:
				buffer.setf (ios::hex,ios::basefield);
				buffer << "#x" << number;
				break;
			default:
				// 报错
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"wrong radix, only can be 2,8,10 and 16");
				break;
			}

			pMachine.makeString (buffer.str ());
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (string->number string)
	// (string->number string radix)
	template <>
	struct ArityLimit <OT_STRING_TO_NUMBER> {
		enum { min = 1,
		       max = 2 };
	};
	template <>
	struct Operation <OT_STRING_TO_NUMBER> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_STRING_TO_NUMBER> (R_ARGL, pMachine);

			// 获取字符串
			pMachine.car (R_VAL,R_ARGL);
			// 判断是否字符串
			if (pMachine.reg(R_VAL).isString ().isFalse ())
				throw RuntimeException(pMachine.getString(R_VAL)
						       ,"expect a string object");
			// 这是得到的是字符串的外部表示，有引号
			string str=pMachine.getString (R_VAL); //
			// 去掉前后的引号
			str = str.substr(1, str.length() - 2);
			stringstream buffer;
			buffer << str;

			// 获取参数中的基数
			pMachine.cdr (R_VAL,R_ARGL);
			Integer radix=10;
			if (pMachine.reg(R_VAL).isNull().isFalse ()) {
				pMachine.car (R_VAL,R_VAL);
				radix = pMachine.reg (R_VAL).getInteger ();
			}

			bool error=false;

			// 获取字符串中的基数，有的话就可以将参数中的基数覆盖掉
			if (str[0] == '#') {
				switch (str[1]) {
				case 'b':
					radix = 2;
					break;
				case 'o':
					radix = 8;
					break;
				case 'd':
					radix = 10;
					break;
				case 'x':
					radix = 16;
					break;
				default: // 返回假
					error = true;
					break;
				}
				char tmp;
				buffer >> tmp; // 去掉'#'
				buffer >> tmp; // 去掉第二个字符
				str = str.substr(2);
			}

			// 格式错误，返回假
			if (error)
				pMachine.assign (R_VAL, Atom::makeFalse ());
			else {
				Integer number=0;
				switch (radix) {
				case 2:
					for (string::size_type i=0;i<str.length();++i) {
						number = number << 1; // 必须先移位
						if (str[i] == '1')
							// 最后一位设为1
							number = number | 0x1;
						else if (str[i] != '0')
							break; // 不是0或1就退出循环
						else	       
							continue; // 是0,则继续
					}
					break;
				case 8:
					buffer.setf (ios::oct,ios::basefield);
					buffer >> number;
					break;
				case 10:
					buffer >> number;
					break;
				case 16:
					buffer.setf (ios::hex,ios::basefield);
					buffer >> number;
					break;
				default: // 不可能出现这种情况
					throw RuntimeException (str,"bad number format");
					break;
				}

				Atom result=Atom::makeInteger(number);
				pMachine.assign (R_VAL,result);
			}

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (eqv? obj1 obj2)
	template <>
	struct ArityLimit <OT_IS_EQV> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_IS_EQV> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_EQV> (R_ARGL, pMachine);

			// 准备第一个参数
			pMachine.car (R_EXP,R_ARGL);

			// 准备第二个参数
			pMachine.cdr (R_VAL,R_ARGL);
			pMachine.car (R_VAL,R_VAL);

			// 比较
			pMachine.eqv (R_EXP, R_VAL);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (eq? obj1 obj2)
	template <>
	struct ArityLimit <OT_IS_EQ> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_IS_EQ> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_EQ> (R_ARGL, pMachine);

			// 准备第一个参数
			pMachine.car (R_EXP,R_ARGL);

			// 准备第二个参数
			pMachine.cdr (R_VAL,R_ARGL);
			pMachine.car (R_VAL,R_VAL);

			// 比较
			pMachine.eq (R_EXP, R_VAL);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (equal? obj1 obj2)
	template <>
	struct ArityLimit <OT_IS_EQUAL> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_IS_EQUAL> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_EQUAL> (R_ARGL, pMachine);

			// 准备第一个参数
			pMachine.car (R_EXP,R_ARGL);

			// 准备第二个参数
			pMachine.cdr (R_VAL,R_ARGL);
			pMachine.car (R_VAL,R_VAL);

			// 比较
			pMachine.equal (R_EXP, R_VAL);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (not obj)
	template <>
	struct ArityLimit <OT_NOT> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_NOT> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_NOT> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			bool result=pMachine.reg (R_VAL).isFalse ();

			Atom obj= (result ? Atom::makeTrue () : Atom::makeFalse ());

			pMachine.assign (R_VAL,obj);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (boolean? obj)
	template <>
	struct ArityLimit <OT_IS_BOOLEAN> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_BOOLEAN> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_BOOLEAN> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom result=pMachine.reg (R_VAL).isBoolean ();

			pMachine.assign (R_VAL,result);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (pair? obj)
	template <>
	struct ArityLimit <OT_IS_PAIR> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_PAIR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_PAIR> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom result=pMachine.reg (R_VAL).isPair ();

			pMachine.assign (R_VAL,result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (null? obj)
	template <>
	struct ArityLimit <OT_IS_NULL> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_NULL> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_NULL> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom result=pMachine.reg (R_VAL).isNull ();

			pMachine.assign (R_VAL,result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (list? obj)
	template <>
	struct ArityLimit <OT_IS_LIST> {
		enum { min = 0,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_IS_LIST> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_LIST> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom result=pMachine.isList (R_VAL);

			pMachine.assign (R_VAL,result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (length list)
	template <>
	struct ArityLimit <OT_LENGTH> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_LENGTH> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_LENGTH> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom result=pMachine.length (R_VAL);

			pMachine.assign (R_VAL,result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};	

	// (symbol? obj)
	template <>
	struct ArityLimit <OT_IS_SYMBOL> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_SYMBOL> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_SYMBOL> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom result=pMachine.reg (R_VAL).isSymbol ();

			pMachine.assign (R_VAL,result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (symbol->string symbol)
	template <>
	struct ArityLimit <OT_SYMBOL_TO_STRING> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_SYMBOL_TO_STRING> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_SYMBOL_TO_STRING> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom symbol=pMachine.reg (R_VAL);

			if (symbol.isSymbol().isTrue ()) {
				// 若是符号，则只重写类型信息即可
				symbol = symbol.toString ();
			} else {
				throw RuntimeException (pMachine.getString(R_ARGL)
							,"expect a symbol object");
			}

			pMachine.assign (R_VAL,symbol);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (string->symbol string)
	template <>
	struct ArityLimit <OT_STRING_TO_SYMBOL> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_STRING_TO_SYMBOL> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_STRING_TO_SYMBOL> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom str=pMachine.reg (R_VAL);

			if (str.isString().isTrue ()) {
				// 若是字符串，则只重写类型信息即可
				str = str.toSymbol ();
			} else {
				throw RuntimeException (pMachine.getString(R_ARGL)
							,"expect a string object");
			}

			pMachine.assign (R_VAL,str);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (char? obj)
	template <>
	struct ArityLimit <OT_IS_CHAR> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_CHAR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_CHAR> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom result=pMachine.reg (R_VAL).isChar ();

			pMachine.assign (R_VAL,result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (char=? char1 char2)
	template <>
	struct ArityLimit <OT_CHAR_EQUAL_TO> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_CHAR_EQUAL_TO> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_CHAR_EQUAL_TO> (R_ARGL, pMachine);

			// 获取字符
			Char ch1, ch2;

			pMachine.car (R_VAL, R_ARGL);
			ch1 = pMachine.reg (R_VAL).getChar ();

			pMachine.cdr (R_VAL, R_ARGL);
			pMachine.car (R_VAL, R_VAL);
			ch2 = pMachine.reg (R_VAL).getChar ();

			Atom result = ((ch1 == ch2)
				       ? Atom::makeTrue ()
				       : Atom::makeFalse ());

			pMachine.assign (R_VAL, result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (char<? char1 char2)
	template <>
	struct ArityLimit <OT_CHAR_LESS_THEN> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_CHAR_LESS_THEN> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_CHAR_LESS_THEN> (R_ARGL, pMachine);

			// 获取字符
			Char ch1, ch2;

			pMachine.car (R_VAL, R_ARGL);
			ch1 = pMachine.reg (R_VAL).getChar ();

			pMachine.cdr (R_VAL, R_ARGL);
			pMachine.car (R_VAL, R_VAL);
			ch2 = pMachine.reg (R_VAL).getChar ();

			Atom result = ((ch1 < ch2)
				       ? Atom::makeTrue ()
				       : Atom::makeFalse ());

			pMachine.assign (R_VAL, result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (char>? char1 char2)
	template <>
	struct ArityLimit <OT_CHAR_GREATER_THEN> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_CHAR_GREATER_THEN> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_CHAR_GREATER_THEN> (R_ARGL, pMachine);

			// 获取字符
			Char ch1, ch2;

			pMachine.car (R_VAL, R_ARGL);
			ch1 = pMachine.reg (R_VAL).getChar ();

			pMachine.cdr (R_VAL, R_ARGL);
			pMachine.car (R_VAL, R_VAL);
			ch2 = pMachine.reg (R_VAL).getChar ();

			Atom result = ((ch1 > ch2)
				       ? Atom::makeTrue ()
				       : Atom::makeFalse ());

			pMachine.assign (R_VAL, result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (char<=? char1 char2)
	template <>
	struct ArityLimit <OT_CHAR_LESS_THEN_OR_EQUAL_TO> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_CHAR_LESS_THEN_OR_EQUAL_TO> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_CHAR_LESS_THEN_OR_EQUAL_TO> (R_ARGL, pMachine);

			// 获取字符
			Char ch1, ch2;

			pMachine.car (R_VAL, R_ARGL);
			ch1 = pMachine.reg (R_VAL).getChar ();

			pMachine.cdr (R_VAL, R_ARGL);
			pMachine.car (R_VAL, R_VAL);
			ch2 = pMachine.reg (R_VAL).getChar ();

			Atom result = ((ch1 <= ch2)
				       ? Atom::makeTrue ()
				       : Atom::makeFalse ());

			pMachine.assign (R_VAL, result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (char>=? char1 char2)
	template <>
	struct ArityLimit <OT_CHAR_GREATER_THEN_OR_EQUAL_TO> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_CHAR_GREATER_THEN_OR_EQUAL_TO> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_CHAR_GREATER_THEN_OR_EQUAL_TO> (R_ARGL, pMachine);

			// 获取字符
			Char ch1, ch2;

			pMachine.car (R_VAL, R_ARGL);
			ch1 = pMachine.reg (R_VAL).getChar ();

			pMachine.cdr (R_VAL, R_ARGL);
			pMachine.car (R_VAL, R_VAL);
			ch2 = pMachine.reg (R_VAL).getChar ();

			Atom result = ((ch1 >= ch2)
				       ? Atom::makeTrue ()
				       : Atom::makeFalse ());

			pMachine.assign (R_VAL, result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (char->integer char)
	template <>
	struct ArityLimit <OT_CHAR_TO_INTEGER> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_CHAR_TO_INTEGER> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_CHAR_TO_INTEGER> (R_ARGL, pMachine);

			// 获取字符
			Char ch;

			pMachine.car (R_VAL, R_ARGL);
			ch = pMachine.reg (R_VAL).getChar ();

			Atom result = Atom::makeInteger ((Integer)ch);

			pMachine.assign (R_VAL, result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (integer->char n)
	template <>
	struct ArityLimit <OT_INTEGER_TO_CHAR> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_INTEGER_TO_CHAR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_INTEGER_TO_CHAR> (R_ARGL, pMachine);

			// 获取数字
			Integer ch;

			pMachine.car (R_VAL, R_ARGL);
			ch = pMachine.reg (R_VAL).getInteger ();

			Atom result;
			if (ch > 0 && ch < 256)
				result = Atom::makeChar ((Char)ch);
			else
				throw RuntimeException (pMachine.getString(R_ARGL)
							,"out of char range");
			
			pMachine.assign (R_VAL, result);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (string? obj)
	template <>
	struct ArityLimit <OT_IS_STRING> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_STRING> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_STRING> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom result=pMachine.reg (R_VAL).isString ();

			pMachine.assign (R_VAL,result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (make-string k)
	// (make-string k char)
	template <>
	struct ArityLimit <OT_MAKE_STRING> {
		enum { min = 1,
		       max = 2 };
	};
	template <>
	struct Operation <OT_MAKE_STRING> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_MAKE_STRING> (R_ARGL, pMachine);

			pMachine.car (R_VAL, R_ARGL);
			Integer k=pMachine.reg (R_VAL).getInteger ();

			if (k<0)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"expect a non-negative integer");

			pMachine.cdr (R_VAL, R_ARGL);
			Char ch='_';
			if (pMachine.reg (R_VAL).isNull ().isFalse ()) {
				pMachine.car (R_VAL, R_VAL);
				ch = pMachine.reg (R_VAL).getChar ();
			}

			string str (k,ch);
			pMachine.makeString (str);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (string-length string)
	template <>
	struct ArityLimit <OT_STRING_LENGTH> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_STRING_LENGTH> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_STRING_LENGTH> (R_ARGL, pMachine);

			// 取出字符串
			pMachine.car (R_VAL, R_ARGL);

			// 检测是否字符串
			if (pMachine.reg (R_VAL).isString ().isFalse ())
				throw RuntimeException (pMachine.getString(R_ARGL)
							,"expect a string object");

			// 再取出字符串中的第一个元素，即长度
			pMachine.car (R_VAL, R_VAL);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (string-ref string k)
	template <>
	struct ArityLimit <OT_STRING_REF> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_STRING_REF> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_STRING_REF> (R_ARGL, pMachine);

			// 取出字符串
			pMachine.car (R_VAL, R_ARGL);

			// 取出下标
			pMachine.cdr (R_EXP, R_ARGL);
			pMachine.car (R_EXP, R_EXP);
			Integer k=pMachine.reg (R_EXP).getInteger ();
			if (k<0)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"expect a non-negative integer");

			// 取出字符串的长度信息，进行比较
			pMachine.car (R_EXP, R_VAL);
			Integer len=pMachine.reg(R_EXP).getInteger ();
			if (k>=len)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"string index out of range");
			
			// 再找出字符串中的第k+1个元素，因为前面有一个
			// 元素保存的是长度信息
			for (Integer i=k+1; i>0; --i) {
				pMachine.cdr (R_VAL, R_VAL);
			}
			pMachine.car (R_VAL, R_VAL);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (string-set! string k char)
	template <>
	struct ArityLimit <OT_STRING_SET> {
		enum { min = 3,
		       max = 3 };
	};
	template <>
	struct Operation <OT_STRING_SET> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_STRING_SET> (R_ARGL, pMachine);

			pMachine.cdr (R_VAL, R_ARGL);

			// 取出下标
			pMachine.car (R_EXP, R_VAL);
			Integer k=pMachine.reg (R_EXP).getInteger ();

			if (k<0)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"expect a non-negative integer");

			// 取出字符，存在EXP中
			pMachine.cdr (R_VAL, R_VAL);
			pMachine.car (R_EXP, R_VAL);
			if (pMachine.reg (R_EXP).isChar ().isFalse ())
				throw RuntimeException (pMachine.getString(R_ARGL)
							,"expected char object at the third");


			// 取出字符串的长度信息，进行比较
			pMachine.car (R_VAL, R_ARGL);
			pMachine.car (R_VAL, R_VAL);
			Integer len=pMachine.reg(R_VAL).getInteger ();
			if (k>=len)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"string index out of range");

			// 最后，取出字符串
			pMachine.car (R_VAL, R_ARGL);
			
			// 找出字符串中的第k+1个元素，因为前面有一个
			// 元素保存的是长度信息
			for (Integer i=k+1; i>0; --i) {
				pMachine.cdr (R_VAL, R_VAL);
			}

			// 设置car的值即可
			pMachine.setCar (R_VAL, R_EXP);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	

	// (vector? obj)
	template <>
	struct ArityLimit <OT_IS_VECTOR> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_VECTOR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_VECTOR> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom result=pMachine.reg (R_VAL).isVector ();

			pMachine.assign (R_VAL,result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};
	
	// (make-vector k)
	// (make-vector k fill)
	template <>
	struct ArityLimit <OT_MAKE_VECTOR> {
		enum { min = 1,
		       max = 2 };
	};
	template <>
	struct Operation <OT_MAKE_VECTOR> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_MAKE_VECTOR> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Integer k=pMachine.reg (R_VAL).getInteger ();

			if (k<0)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"expect a non-negative integer");
			// 准备填充内容，若没有则填充数字0
			pMachine.cdr (R_EXP,R_ARGL);
			if (pMachine.reg (R_EXP).isNull ().isFalse ())
				pMachine.car (R_EXP,R_EXP);
			else
				pMachine.assign (R_EXP,Atom::makeInteger(0));

			// 创建向量
			pMachine.assign (R_VAL, Atom::makeNull ());
			for (int i=0; i<k; ++i) {
				pMachine.cons (R_VAL, R_EXP, R_VAL);
			}
			// 添加长度信息
			pMachine.assign (R_EXP, Atom::makeInteger(k));
			pMachine.cons (R_VAL, R_EXP, R_VAL);
			// 转换类型
			Atom vec=pMachine.reg (R_VAL);
			pMachine.assign (R_VAL, vec.toVector ());

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (vector-length vector)
	template <>
	struct ArityLimit <OT_VECTOR_LENGTH> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_VECTOR_LENGTH> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_VECTOR_LENGTH> (R_ARGL, pMachine);

			// 取出向量
			pMachine.car (R_VAL, R_ARGL);

			// 检测是否向量
			if (pMachine.reg (R_VAL).isVector ().isFalse ())
				throw RuntimeException (pMachine.getString(R_ARGL)
							,"expect a vector object");

			// 再取出第一个元素，即长度
			pMachine.car (R_VAL, R_VAL);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (vector-ref vector k)
	template <>
	struct ArityLimit <OT_VECTOR_REF> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_VECTOR_REF> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_VECTOR_REF> (R_ARGL, pMachine);

			// 取出向量
			pMachine.car (R_VAL, R_ARGL);

			// 取出下标
			pMachine.cdr (R_EXP, R_ARGL);
			pMachine.car (R_EXP, R_EXP);
			Integer k=pMachine.reg (R_EXP).getInteger ();
			if (k<0)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"expect a non-negative integer");

			// 取出长度信息，进行比较
			pMachine.car (R_EXP, R_VAL);
			Integer len=pMachine.reg(R_EXP).getInteger ();
			if (k>=len)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"index out of range");
			
			// 再找出第k+1个元素，因为前面有一个元素保存的
			// 是长度信息
			for (Integer i=k+1; i>0; --i) {
				pMachine.cdr (R_VAL, R_VAL);
			}
			pMachine.car (R_VAL, R_VAL);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	// (vector-set! vector k obj)
	template <>
	struct ArityLimit <OT_VECTOR_SET> {
		enum { min = 3,
		       max = 3 };
	};
	template <>
	struct Operation <OT_VECTOR_SET> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_VECTOR_SET> (R_ARGL, pMachine);

			pMachine.cdr (R_VAL, R_ARGL);

			// 取出下标
			pMachine.car (R_EXP, R_VAL);
			Integer k=pMachine.reg (R_EXP).getInteger ();

			if (k<0)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"expect a non-negative integer");

			// 取出对象，存在EXP中
			pMachine.cdr (R_VAL, R_ARGL);
			pMachine.car (R_EXP, R_VAL);

			// 取出长度信息，进行比较
			pMachine.car (R_VAL, R_ARGL);
			pMachine.car (R_VAL, R_VAL);
			Integer len=pMachine.reg(R_VAL).getInteger ();
			if (k>=len)
				throw RuntimeException (pMachine.getString (R_ARGL)
							,"index out of range");

			// 最后，取出向量
			pMachine.car (R_VAL, R_ARGL);
			
			// 找出第k+1个元素，因为前面有一个元素保存的是
			// 长度信息
			for (Integer i=k+1; i>0; --i) {
				pMachine.cdr (R_VAL, R_VAL);
			}

			// 设置car的值即可
			pMachine.setCar (R_VAL, R_EXP);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};


	// (procedure? obj)
	template <>
	struct ArityLimit <OT_IS_PROCEDURE> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_PROCEDURE> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_PROCEDURE> (R_ARGL, pMachine);

			pMachine.car (R_VAL,R_ARGL);
			Atom obj=pMachine.reg (R_VAL);

			bool is = ! obj.isSyntax (); // 不能是语法性操作
			is = is && obj.isProcedure ().isTrue (); // 是复合函数
			is = is && obj.isPrimOp ().isTrue ();	 // 或基本操作

			Atom result = (is ? Atom::makeTrue ()
				       : Atom::makeFalse ());

			pMachine.assign (R_VAL,result);
			
			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};

	
	// (apply proc arg1 ... args)
	template <>
	struct ArityLimit <OT_APPLY_PROC> {
		enum { min = 1,
		       max = Arity::A_DISABLE };
	};
	template <>
	struct Operation <OT_APPLY_PROC> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_APPLY_PROC> (R_ARGL, pMachine);

			// 将操作符存到R_PROC中
			pMachine.car (R_VAL, R_ARGL);
			pMachine.assign (R_PROC, R_VAL);

			// 参数表存到R_ARGL中
			pMachine.cdr (R_ARGL, R_ARGL);

			// 保存当前继续
			pMachine.save (R_CONTINUE);

			// 转到OT_APPLY中去
			Atom apply = Atom::makePrimOp (OT_APPLY);
			pMachine.assign (R_EXP, apply);
		}
	};

	
	// (eval expression environment)
	template <>
	struct ArityLimit <OT_EVAL> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_EVAL> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_EVAL> (R_ARGL, pMachine);

			// 将表达式放到EXP中，准备求值
			pMachine.car (R_EXP, R_ARGL);

			// 保存当前环境，并设置新的环境
			pMachine.save (R_ENV);
			pMachine.cdr (R_ENV, R_ARGL);
			pMachine.car (R_ENV, R_ENV);

			// 保存当前继续
			pMachine.save (R_CONTINUE);

			// 设置继续
			Atom done=Atom::makePrimOp (OT_EVAL_DONE);
			pMachine.assign (R_CONTINUE, done);
		}
	};

	template <>
	struct Operation <OT_EVAL_DONE> {
		void operator() (Machine &pMachine) {
			// 恢复继续和环境
			pMachine.restore (R_CONTINUE);
			pMachine.restore (R_ENV);

			// 返回
			pMachine.assign (R_EXP, R_CONTINUE);
		}
	};



	
	// (tos::register reg-name)
	template <>
	struct ArityLimit <OT_TOS_REG> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_TOS_REG> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_TOS_REG> (R_ARGL, pMachine);

			// 获取参数的字符串表示
			pMachine.car (R_VAL,R_ARGL);
			string name=pMachine.getString (R_VAL);

			Register target;
			if (name == "exp")
				target = R_EXP;
			else if (name == "env")
				target = R_ENV;
			else if (name == "val")
				target = R_VAL;
			else if (name == "continue")
				target = R_CONTINUE;
			else if (name == "proc")
				target = R_PROC;
			else if (name == "argl")
				target = R_ARGL;
			else if (name == "unev")
				target = R_UNEV;
			else if (name == "stack")
				target = R_STACK;
			else
				throw RuntimeException (name,"unknown register name");

			// 将寄存器放入结果中，后面就可以操作该寄存器了，例如环境寄存器。
			pMachine.assign (R_VAL, target);

			//cout << pMachine.getString (target) << endl;
			
			// 返回
			pMachine.assign (R_EXP,R_CONTINUE);
		}
	};

	
	// (tos::memory start end)
	template <>
	struct ArityLimit <OT_TOS_MEMORY> {
		enum { min = 2,
		       max = 2 };
	};
	template <>
	struct Operation <OT_TOS_MEMORY> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_TOS_MEMORY> (R_ARGL, pMachine);

			// 获取参数
			pMachine.car (R_VAL,R_ARGL);
			Integer start=pMachine.reg (R_VAL).getInteger ();
			pMachine.cdr (R_VAL, R_ARGL);
			pMachine.car (R_VAL,R_VAL);
			Integer end=pMachine.reg (R_VAL).getInteger ();

			cout << pMachine.getMemoryString (start, end) << endl;
			
			// 返回
			pMachine.assign (R_EXP,R_CONTINUE);
		}
	};


	// (display obj)
	template <>
	struct ArityLimit <OT_DISPLAY> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_DISPLAY> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_DISPLAY> (R_ARGL, pMachine);

			// 获取参数
			pMachine.car (R_VAL,R_ARGL);
			stringstream buffer;

			Atom obj=pMachine.reg (R_VAL);
			if ( obj.isChar ().isTrue () ) {
				buffer << obj.getChar ();
			} else if ( obj.isString ().isTrue () ) {
				pMachine.car (R_EXP, R_VAL); // skip the length
				pMachine.cdr (R_VAL, R_VAL);

				// get the string
				for (;pMachine.reg (R_VAL).isNull ().isFalse ();
				     pMachine.cdr (R_VAL, R_VAL)) {
					pMachine.car (R_EXP, R_VAL);
					buffer << pMachine.reg (R_EXP).getChar ();
				}
			} else {
				buffer << pMachine.getString (R_VAL);
			}

			//pMachine.assign (R_VAL, target);

			cout << buffer.str ();
			
			// 返回
			pMachine.assign (R_EXP,R_CONTINUE);
		}
	};


	// (write obj)
	template <>
	struct ArityLimit <OT_WRITE> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_WRITE> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_WRITE> (R_ARGL, pMachine);

			// 获取参数
			pMachine.car (R_VAL,R_ARGL);

			//pMachine.assign (R_VAL, target);

			cout << pMachine.getString (R_VAL);
			
			// 返回
			pMachine.assign (R_EXP,R_CONTINUE);
		}
	};

	// (newline)
	template <>
	struct ArityLimit <OT_NEWLINE> {
		enum { min = 0,
		       max = 0 };
	};
	template <>
	struct Operation <OT_NEWLINE> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_NEWLINE> (R_ARGL, pMachine);

			//pMachine.assign (R_VAL, target);

			cout << endl;
			
			// 返回
			pMachine.assign (R_EXP,R_CONTINUE);
		}
	};

	// (read)
	template <>
	struct ArityLimit <OT_READ> {
		enum { min = 0,
		       max = 0 };
	};
	template <>
	struct Operation <OT_READ> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_READ> (R_ARGL, pMachine);

			Parser parser (cin, pMachine);

			//try {
			parser.parseDatum (); // 解析后的结果已经在R_VAL中了。
				//} catch (Exception &e) {
				
				//}

			//pMachine.assign (R_VAL, target);
			
			// 返回
			pMachine.assign (R_EXP,R_CONTINUE);
		}
	};

	
	// (eof-object? obj)
	template <>
	struct ArityLimit <OT_IS_EOF_OBJECT> {
		enum { min = 1,
		       max = 1 };
	};
	template <>
	struct Operation <OT_IS_EOF_OBJECT> {
		void operator() (Machine &pMachine) {
			Arity::check<OT_IS_EOF_OBJECT> (R_ARGL, pMachine);

			// 获取参数
			pMachine.car (R_VAL,R_ARGL);

			Atom result = pMachine.reg (R_VAL).isEof ();
			pMachine.assign (R_VAL, result);
			
			// 返回
			pMachine.assign (R_EXP,R_CONTINUE);
		}
	};

} // namespace


#endif	// PrimitiveProcedure_HPP
