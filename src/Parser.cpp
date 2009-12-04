// -*-c++-*-



#include "Parser.hpp"
#include "Machine.hpp"



// namespace
namespace tos {


	int Parser::skipWhitespaceRead () {
		bool inComment = false;

		for (;;) {
			int c = readChar ();

			if (inComment) {
				if (c=='\n')
					inComment = false;
				else if (c==EOF)
					return c;
			} else {
				if (c==';')
					inComment = true;
				else if (!isWhitespace(c))
					return c;
			}
		}
	}

	void Parser::parseDatum () {
		int ch1 = skipWhitespaceRead ();

		switch (ch1) {
		case '#':
		{
			int ch2 = readChar ();

			switch (ch2) {
			case 't':
				mMachine.assign (R_VAL, Atom::makeTrue ());
				break;
			case 'f':
				mMachine.assign (R_VAL, Atom::makeFalse ());
				break;
			case '\\':
				parseChar ();
				break;
			case '(':
				parseVector ();
				break;
			default:
			{
				Char tmp[2] = {ch2,'\0'};
				throw ParserException (string("'") + tmp
						      +"' can't follow '#'");
			}
			}
			break;
		}
		case '(':
			parseList ();
			break;
		case '"':
			parseString ();
			break;
		case '\'':	// (quote ...)
			parseDatum ();
			mMachine.save (R_ARGL);
			mMachine.assign (R_ARGL, Atom::makeNull ());
			mMachine.cons (R_VAL, R_ARGL);
			mMachine.assign (R_ARGL, R_VAL);
			mMachine.makeQuote ();
			mMachine.cons (R_VAL, R_ARGL);
			mMachine.restore (R_ARGL);
			break;
		case '`':	// (quasiquote ...)
			parseDatum ();
			mMachine.save (R_ARGL);
			mMachine.assign (R_ARGL, Atom::makeNull ());
			mMachine.cons (R_VAL, R_ARGL);
			mMachine.assign (R_ARGL, R_VAL);
			mMachine.makeQuasiQuote ();
			mMachine.cons (R_VAL, R_ARGL);
			mMachine.restore (R_ARGL);
			break;
			/*case ',':
		{
			int ch2 = peekChar ();
			Atom symbol = mMachine.makeUnquote ();
		
			if (ch2 == '@') {
				readChar ();
				symbol = mMachine.makeUnquoteSplicing ();
			}

			return mMachine.cons (symbol, parseDatum ());
			}*/
		case EOF:		// 到达结尾
			mMachine.assign(R_VAL, Atom::makeEof ());
			break;
		default:
			parseNumberOrSymbol((Char)ch1);
			break;
		}
	}


	void Parser::parseChar () {
		int c = readCharNoEof ();

		if ((c=='s') || (c=='n')) {
			stringstream sstream;

			do {
				sstream << (Char)c;
				c = readChar ();
			} while ((c!=EOF) && ! isDelimiter(c));

			if (c!=EOF) {
				mStream.unget ();
			}

			string str = sstream.str ();

			if (str == "s")
				mMachine.assign (R_VAL, Atom::makeChar ('s'));
			else if (str == "n")
				mMachine.assign (R_VAL, Atom::makeChar ('n'));
			else if (str == "space")
				mMachine.assign (R_VAL, Atom::makeChar (' '));
			else if (str == "newline")
				mMachine.assign (R_VAL, Atom::makeChar ('\n'));
			else
				throw ParserException (string("invalid character name '")
						      + str + "'");
		} else {
			mMachine.assign (R_VAL, Atom::makeChar ((Char)c));
		}
	
		/*int c = readChar ();

		  return (c == EOF)
		  ? Atom::makeEof ()
		  : Atom ((Char)c);*/
	}

	void Parser::parseString () {
		stringstream sstream;

		for (Char c=readCharNoEof ();
		     c != '"';
		     c=readCharNoEof ()) {
			if (c == '\\') {
				// 读取转义字符，不管后面是什么
				sstream << (Char)readCharNoEof ();
				continue;
			} else
				sstream << (Char)c;

		}

		mMachine.makeString (sstream.str ());
	}

	void Parser::parseVector () {
		
		parseList ();

		mMachine.save (R_VAL);

		//mMachine.cdr (R_VAL);
		Integer length = 0;
		while (mMachine.reg (R_VAL).isNull ().isFalse ()) {
			++length;
			mMachine.cdr (R_VAL);
		}
		mMachine.restore (R_VAL);

		mMachine.save (R_ARGL);
		mMachine.assign (R_ARGL, Atom::makeInteger (length));
		mMachine.cons (R_ARGL, R_VAL);
		mMachine.restore (R_ARGL);

		Atom result = mMachine.reg (R_VAL).toVector ();
		mMachine.assign (R_VAL, result);

		return;
		
		// 使用R_ARGL作为中间变量
		mMachine.save (R_ARGL);
		
		int ch;
		// 获取每一个原子，并依次保存到栈中
		for (ch = skipWhitespaceRead ();
		     ch != EOF && ch != ')';
		     ch = skipWhitespaceRead ()) {
			mStream.unget ();

			parseDatum ();
			mMachine.save (R_VAL);

			// 并统计长度
			++length;
		}

		if (ch == EOF)
			throw ParserException ("unexpected EOF");
		else/* if (ch == ')') */
		{
			// 开始构建表
			mMachine.assign (R_VAL, Atom::makeNull ());
			// 依次弹出栈中的原子，正好是反序，构建出正序的表
			for (Integer i=0; i<length; ++i) {
				mMachine.restore (R_ARGL);
				mMachine.cons (R_ARGL, R_VAL);
			}
			// 现在R_VAL中是所有原子的表

			// 再设置长度标志
			mMachine.assign (R_ARGL, Atom::makeInteger (length));
			mMachine.cons (R_ARGL, R_VAL);

			// 再将表类型转换为向量即可
			Atom result = mMachine.reg (R_VAL).toVector ();
			mMachine.assign (R_VAL, result);
		}

		// 恢复中间变量
		mMachine.restore (R_ARGL);
	}

	void Parser::parseList () {
		int ch = skipWhitespaceRead ();

		if (ch == ')') {
			mMachine.assign (R_VAL, Atom::makeNull ());
			return;
		} else if (ch == EOF) {
			throw ParserException ("unexpected EOF");
		}

		mStream.unget ();

		// 中间寄存器
		mMachine.save (R_ARGL);
		
		parseDatum ();
		mMachine.assign (R_ARGL, R_VAL); // 将head保存到R_ARGL中

		ch = skipWhitespaceRead ();
		if (ch == '.') {
			parseDatum ();
			mMachine.cons (R_ARGL, R_VAL);

			if (skipWhitespaceRead () != ')')
				throw ParserException ("expected ')'");
		} else if (ch == EOF) {
			throw ParserException ("unexpected EOF");
		} else {
			mStream.unget ();
			parseList ();
			mMachine.cons (R_ARGL, R_VAL);
		}

		// 恢复中间寄存器
		mMachine.restore (R_ARGL);
	}

	void Parser::parseNumberOrSymbol (Char first) {
		stringstream sstream;

		first = tolower (first);
		sstream << first;

		for (;;) {
			int c = readChar ();

			if (c == EOF)
				break;
			else if (isDelimiter (c)) {
				mStream.unget ();
				break;
			}
			sstream << (Char)tolower ((Char)c);
		}

		string str = sstream.str ();

		if (str == "+" || str == "-" || str == "...") {
			mMachine.makeSymbol (str);
			return;
		}

	//checkNumber
		try {
			if (!isdigit(first)) {
				if ((first != '+')
				    && (first != '-'))
					throw (int)0;
			}
			for (size_t i=1; i<str.size(); ++i) {
				if (!isdigit(str[i]))
					throw (int)0;
			}

			Integer num;
			sstream >> num;
			mMachine.assign (R_VAL, Atom::makeInteger (num));
			return;
		} catch (int zero) {
			if (zero != 0)	// 不是我刚才抛出的
				throw;
		}

		if (!isInitial(first))
			throw ParserException ("invalid identifier");

		for (size_t i=1; i<str.size (); ++i) {
			if (!isSubsequent (str[i]))
				throw ParserException ("invalid identifier");
		}

		mMachine.makeSymbol (str);
		return;
	}


} // namespace

