

#include <cassert>

#include "common.hpp"
#include "Atom.hpp"
#include "Machine.hpp"
#include "Trace.hpp"
#include "Parser.hpp"


using namespace tos;


class TestAtom {
public:

	TestAtom () : mMachine (Machine::instance ()) {}
	
	void test () {
		//testNull ();

		//testStack ();
		
		//testGC ();

		//testParseNull ();
		//testParseNumber ();
		//testParseSymbol ();
		//testParseString ();
		//testParsePair ();

		testParseProgram ();
		
		//testParseList ();
	}


	
	void testParseProgram () {

		ifstream infile ("../doc/test-code.scm");
		
		Parser p(infile, mMachine);

		while (! infile.eof ()) {
			p.parseDatum ();
			
			cout << mMachine.getString (R_VAL) << std::endl;
		}
		infile.close ();
			
		/*assert ("parser cannot parse program from file"
			&& result.isNull ().isTrue ()
			&& result == Atom::makeNull ());*/
	}

#if 0
	void testParseNull () {

		Atom result;
		try {
		        result = parse ("()");
		} catch (Exception &e) {
			cerr << e.getString () << std::endl;
		}

		assert ("parser cannot parse ()"
			&& result.isNull ().isTrue ()
			&& result == Atom::makeNull ());
	}
	
	void testParseNumber () {

		Atom result;
		try {
			result = parse ("-123");
		} catch (Exception &e) {
			cerr << e.getString () << std::endl;
		}

		assert ("parser cannot parse number"
			&& result.isInteger ().isTrue ()
			&& mMachine.getString (result) == "-123");
	}

	
	
	void testParseSymbol () {

		Atom result;
		try {
			result = parse (" list->VECTOR");
		} catch (Exception &e) {
			cerr << e.getString () << std::endl;
		}

		cout << mMachine.getString (result) << std::endl;

		assert ("parser cannot parse symbol"
			&& result.isSymbol ().isTrue ()
			&& mMachine.getString (result) == "list->vector");
	}

	

	
		
	void testParseString () {

		Atom result;
		try {
			result = parse ("\"string\"");
		} catch (Exception &e) {
			cerr << e.getString () << std::endl;
		}

		cout << mMachine.getString(result) << std::endl;

		assert ("parser cannot parse string"
			&& result.isString ().isTrue ()
			&& mMachine.getString (result) == "\"string\"");
	}

	

	//TODO: List的输出功能有待改进
	void testParseList () {

		Atom result;
		try {
			result = parse ("(-123 (a b 'c) \"string\" end)");
		} catch (Exception &e) {
			cerr << e.getString () << std::endl;
		}

		cout << mMachine.getString (result) << std::endl;

		assert ("parser cannot parse list"
			&& result.isPair ().isTrue ()
			&& mMachine.getString (result) == "(-123 (a b (quote c)) \"string\" end)");
	}

	void testParsePair () {

		Atom result;
		try {
			result = parse ("(a . ( c . Z))");
		} catch (Exception &e) {
			cerr << e.getString () << std::endl;
		}

		cout << mMachine.getString (result) << std::endl;

		assert ("parser cannot parse pair"
			&& result.isPair ().isTrue ()
			&& mMachine.getString (result) == "(a . (c . z))");
	}

	
	// eq eqv equal
	void testCompare () {
	}

	// OK
	void testNull () {
		Atom aNull = Atom::makeNull ();

		assert ("null list is not null"
			&& aNull.isNull ().isTrue());

		Atom other (T_NULL);
		assert ("two null is not equal in c++"
			&& aNull == other);

		mMachine.setReg (R_ARGL, other);
		mMachine.setReg (R_UNEV, Atom::makeNull ());

		mMachine.eq (R_ARGL, R_UNEV);
		assert ("two null is not eq in scheme"
			&& mMachine.getReg (R_VAL).isTrue());

		mMachine.eqv (R_ARGL, R_UNEV);
		assert ("two null is not eqv in scheme"
			&& mMachine.getReg (R_VAL).isTrue());

		mMachine.equal (R_ARGL, R_UNEV);
		assert ("two null is not equal in scheme"
			&& mMachine.getReg (R_VAL).isTrue());
	}

	// OK
	void testStack () {
		Atom null = Atom::makeNull ();
		Atom t = Atom::makeTrue ();
		Atom f = Atom::makeFalse ();
		Atom c ('S');
		Atom i ((Integer)9);
		Atom p ((Pointer)3);
		Atom pair = Atom::makePair ((Pointer)0);

		mMachine.setReg (R_EXP, null);
		mMachine.save (R_EXP);
		mMachine.save (R_ENV);
		mMachine.save (R_VAL);
		mMachine.save (R_ARGL);
		mMachine.save (R_UNEV);

		// 栈中已有5个数据
		
		mMachine.setReg (R_EXP, pair);
		mMachine.save (R_EXP);
		mMachine.setReg (R_EXP, p);
		mMachine.save (R_EXP);

		mMachine.setReg (R_ENV, i);
		mMachine.save (R_ENV);

		mMachine.setReg (R_VAL, c);
		mMachine.save (R_VAL);

		mMachine.setReg (R_ARGL, f);
		mMachine.save (R_ARGL);

		mMachine.setReg (R_UNEV, t);
		mMachine.save (R_UNEV);

		mMachine.restore (R_VAL);
		assert ("stack restore make a error data"
			&& mMachine.getReg (R_VAL) == t);

		mMachine.restore (R_UNEV);
		assert ("stack restore make a error data"
			&& mMachine.getReg (R_UNEV) == f);

		mMachine.restore (R_EXP);
		assert ("stack restore make a error data"
			&& mMachine.getReg (R_EXP) == c);

		mMachine.restore (R_EXP);
		assert ("stack restore make a error data"
			&& mMachine.getReg (R_EXP) == i);

		mMachine.restore (R_EXP);
		assert ("stack restore make a error data"
			&& mMachine.getReg (R_EXP) == p);
		
		mMachine.restore (R_EXP);
		assert ("stack restore make a error data"
			&& mMachine.getReg (R_EXP) == pair);

		mMachine.setReg (R_EXP, null);
		mMachine.save (R_EXP);
		mMachine.restore (R_EXP);
		assert ("stack restore make a error data"
			&& mMachine.getReg (R_EXP) == null);

		mMachine.restore (R_EXP);
		mMachine.restore (R_EXP);
		mMachine.restore (R_EXP);
		mMachine.restore (R_EXP);
		mMachine.restore (R_EXP);

		try {
			mMachine.restore (R_EXP);
		} catch (EmptyStackException &e) {
			// 此处的异常应该抛出
			std::cout << "Exception expected:" << e.getString () << std::endl;
		}

		mMachine.setReg (R_EXP, c);
		mMachine.save (R_EXP);
		mMachine.restore (R_EXP);
		assert ("stack error after throw a exception"
			&& mMachine.getReg (R_EXP) == c);
	}


	void testType () {
		/*
		mMachine.setReg(R_VAL, Atom::makeNull ());
		for (Char c='a';c<='z';++c) {
			Atom ch(c);

			mMachine.setReg (R_ARGL, ch);
			mMachine.cons (R_ARGL, R_VAL);
		}

		*/
	}

	void testPair () {
		
	}

	// OK
	void testGC () {
		//(0 1 2 3 4 ...)
		// 创建HEAP_SIZE个元素的表对象一定将引起垃圾回收
		mMachine.setReg(R_UNEV, Atom::makeNull ());
		mMachine.setReg(R_PROC, Atom::makeNull ());
		for (Integer i=0; i<HEAP_SIZE; ++i) {
			Atom integer(i);

			mMachine.setReg (R_ARGL, integer);

			mMachine.cons (R_ARGL, R_UNEV);

			// 复制结果
			mMachine.setReg (R_UNEV, mMachine.getReg(R_VAL));

			// 要注意生成垃圾，否则垃圾回收不会起作用
			if (i%20 == 0) {
				mMachine.setCdr (R_UNEV, R_PROC);
			}
		}

		{
			Atom list = mMachine.getReg (R_UNEV);
			assert ("cons' result is not pair"
				&& list.isPair ().isTrue ());
		}

		//TRACE_ENABLE(false);
		{TRACE_LOCK;
		std::cout << '(';
		for (Atom list=mMachine.getReg (R_VAL);
		     list.isNull ().isFalse ();
		     mMachine.cdr (R_VAL), list=mMachine.getReg (R_VAL)) {

			//assert ("cons' result is not pair"
			//&& list.isPair ().isTrue ());

			mMachine.save (R_VAL);
			
			mMachine.car (R_VAL);
			std::cout << mMachine.getReg (R_VAL).getInteger ()
				  << ' ';

			mMachine.restore (R_VAL);
		}
		std::cout << ')' << std::endl;
		}
	}

protected:
	
	Atom parse (string exp) {
		istringstream sstream(exp);
		
		Parser p (sstream, mMachine);

		p.parseDatum ();

		return mMachine.reg (R_VAL);
	}
#endif	// if 0
	
private:
	Machine &mMachine;
};



int main () {
	//try {
	TestAtom testAtom;
	testAtom.test ();
	//} catch (Exception &e) {
//		cerr << e.toString () << endl;
//		throw;
//	}
	
	return 0;
}

