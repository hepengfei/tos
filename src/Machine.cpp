// -*-c++-*-

//
// headers
// 
#include "Machine.hpp"
#include "Exception.hpp"

// namespace
namespace tos {

	//
	// 单体模式 - 采用静态局部变量的方式实现单体
	//
	
	Machine &Machine::instance () {

		static Machine INSTANCE (HEAP_SIZE);

		return INSTANCE;
	}

	//
	// 构造函数
	// 
	
	Machine::Machine (size_t pHeapSize) : mHeap(pHeapSize, R_NUM) {
		assign (R_STACK, Atom::makeNull ()); // 初始化栈
		assign (R_ENV, emptyEnv ());	     // 设置环境寄存器

		// 最好将各个寄存器先初始化
		assign (R_EXP, Atom::makeNull ());
		assign (R_VAL, Atom::makeNull ());
		assign (R_CONTINUE, Atom::makeNull ());
		assign (R_PROC, Atom::makeNull ());
		assign (R_ARGL, Atom::makeNull ());
		assign (R_UNEV, Atom::makeNull ());

		 max_deep_stack=0;
		 cur_item_stack=0;
	}

	//
	// 其它操作实现
	// 

	// 创建字符串对象
	void Machine::makeString (string pString) {
		// 保存要使用的寄存器
		save (R_ARGL);

		// 通过R_VAL创建字符串对象
		assign (R_VAL, Atom::makeNull ());

		// 反向迭代，以创建正向的表
		for (string::reverse_iterator it=pString.rbegin ();
		     it != pString.rend (); ++ it) {
			assign (R_ARGL, Atom::makeChar (*it));
			// R_VAL获得结果，可供下次使用
			cons (R_ARGL, R_VAL);
		}
		// 此时R_VAL中是字符组成的表

		// 再加入长度标记
		Integer isize = (Integer)pString.size ();
		Atom asize = Atom::makeInteger (isize);
		assign (R_ARGL, asize);
		cons (R_ARGL, R_VAL);
		// 此时R_VAL中是长度及字符组成的表
			
		// 更改类型标志为字符串即可，将点对转换为字符串类型
		Atom result = reg (R_VAL).toString ();
		assign (R_VAL, result);

		// 恢复寄存器内容
		restore (R_ARGL);
	}

	// 获取对象的外部表示
	string Machine::getString (Atom pValue) {
		stringstream buffer;

		Atom cur = pValue;
		switch (cur.getType ()) {
		case T_BROKENHEART:
			buffer << "#primitive [broken heart]";
			break;
		case T_EOF:
			buffer << "#primitive [eof]";
			break;
				
		case T_NULL:
			buffer << "()";
			break;
		case T_CHAR:
			buffer << "#\\";
			switch (cur.getChar ()) {
			case ' ':
				buffer << "space";
				break;
			case '\n':
				buffer << "newline";
				break;
			default:
				buffer << cur.getChar ();
				break;
			}
			break;
		case T_INTEGER:
			buffer << cur.getInteger ();
			break;
		case T_BOOLEAN:
			buffer << '#';
			buffer << (cur.isFalse () ? 'f' : 't');
			break;
		case T_PRIMITIVE_PROCEDURE:
			buffer << "#primitive [procedure]";
			break;
				
		case T_POINTER:
			buffer << "#primitive [pointer]";
			break;

		case T_PAIR:
			// (a b c . d)
			// (a b c d e)
			buffer << '(';
			buffer << getPairString (cur);
			buffer << ')';
			break;
		case T_VECTOR:
			// #(a b c): (3 a b c)
			// #()
			buffer << "#(";

			cur = mHeap.cdr (cur); // skip the length
			buffer << getPairString (cur);

			buffer << ")";
			break;
		case T_STRING:
			// "str": (3 #\s #\t #\r)
			buffer << '\"';

			cur = mHeap.cdr (cur); // skip the length
			buffer << getStrString (cur, true);
				
			buffer << '\"';
			break;
		case T_SYMBOL:
			// symbol: (6 #\s #\y #\m #\b #\o #\l)

			cur = mHeap.cdr (cur); // skip the length

			buffer << getStrString (cur, false);

			break;
		case T_PROCEDURE:
			buffer << "#[compound procedure]";
			break;
		case T_PORT:
			buffer << "#[port]";
			break;
		default:
			buffer << "#[unknown]";
			break;
		}

		return buffer.str ();
	}

	//
	// 辅助操作实现
	// 

	// 获取表内容的外部表示
	string Machine::getPairString (Atom pPair) {
		// 空表直接返回
		if (pPair.isNull ().isTrue ())
			return "";
		
		stringstream buffer;
			
		Atom thecdr = pPair;
		Atom tmp;
		while (true) {
			//car (R_VAL, r); // 先输出第一个
			tmp = mHeap.car (thecdr);
			buffer << ' ' << getString (tmp);

			//assign (R_VAL, thecdr);
			//cdr (R_VAL, r);
			//thecdr = reg (R_VAL);
			thecdr = mHeap.cdr (thecdr);

			if (thecdr.isNull ().isTrue ()) {
				break;
			} else if (thecdr.isPair ().isFalse ()) {
				// 若cdr不是点对，则本身是为点对或非严格表
				buffer << " . " << getString (thecdr);
				break;
			} else {
				// 若是点对
				;//assign (R_VAL, thecdr);
			}
		}

		// 需要跳过第一个元素前的空格
		return buffer.str ().substr(1);
	}

	// 获取字符表形式对象的外部表示
	string Machine::getStrString (Atom pList, bool trans) {
		stringstream buffer;
		//save (R_VAL);

		Atom rest = pList;
		Atom tmp;
		Char ch;
		while (rest.isNull ().isFalse ()) {
			//assign (R_VAL, rest); //可提到循环前面
			//car (R_VAL);
			tmp = mHeap.car (rest);

			ch = tmp.getChar ();
			if (trans && (ch == '\"' || ch == '\\')) // 需要转义
				buffer << '\\';
			buffer << ch;
			
			rest = mHeap.cdr (rest);
		}
		
		//restore (R_VAL);
		return buffer.str ();
	}
	
	// 
	// 基本相等谓词 - 由于用到了mHeap的一些操作，所以不能独立出来。
	// 若将Heap也实现为单体模式，就可以将其独立出来了。
	//
	
	bool Machine::base_eq (Atom v1, Atom v2) {
		// 类型不同
		if (v1.getType () != v2.getType ()) {
			return false;
		}

		// 不是符号对象
		if (v1.isSymbol ().isFalse ()) {
			return v1 == v2;
		}

		// 是符号，则比较其外部表示
		return (getString (v1) == getString (v2));
		
		/*
		// 是符号对象，迭代比较
		bool result=true;
		for (;result == true
			     && v1.isNull ().isFalse ()
			     && v2.isNull ().isFalse ();
		     v1=mHeap.cdr (v1), v2=mHeap.cdr (v2)) {
			result = (mHeap.car (v1) == mHeap.car (v2));
		}
		

		// 比较结果为真，并且都比较完了，才认为相等。
		result = (result == true
			  && v1.isNull ().isTrue ()
			  && v2.isNull ().isTrue ());

		return result;		*/
	}

	bool Machine::base_equal (Atom v1, Atom v2) {
		// 类型是否不同
		if (v1.getType () != v2.getType ()) {
			return false;
		}

		// 不是点对、向量和字符串，用eqv比较
		if (v1.isPair ().isFalse ()
		    && v1.isVector ().isFalse ()
		    && v1.isString ().isFalse ()) {
			return base_eqv (v1,v2);
		}

		// 是同类型的点对、向量或字符串对象，递归比较
		bool result = true;
		while (result == true
		       && v1.isNull ().isFalse ()
		       && v2.isNull ().isFalse ()) {
			result = base_equal (mHeap.car (v1), mHeap.car (v2));

			v1=mHeap.cdr (v1);
			v2=mHeap.cdr (v2);
		}

		// 全部比较完并且结果为真，才认为相等
		result = (result == true
			  && v1.isNull ().isTrue ()
			  && v2.isNull ().isTrue ());

		return result;
	}

	// 检查堆是否有空闲
	void Machine::checkSpace () {
		// 第一次检查，不足时进行回收
		if (! mHeap.hasSpace()) {
			Atom root = mHeap.saveRegisters (mRegisters); // 保存寄存器组
			mHeap.gcStopAndCopy (root);	     // 回收
			mHeap.restoreRegisters (mRegisters); // 恢复寄存器组

			// 第二次检查，看是否有垃圾对象被回收。
			// 回收后若空间仍不足，则内存耗尽。
			if (! mHeap.hasSpace())
				throw MemoryExhaustedException ();
		}
	}
	
	// 检查栈是否为空
	void Machine::checkStack () {
		Atom top = reg (R_STACK);
		if (top.isNull ().isTrue ())
			throw EmptyStackException ();
	}

	
} // namespace

