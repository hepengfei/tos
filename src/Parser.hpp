// -*-c++-*-

#ifndef Parser_HPP
#define Parser_HPP


#include "common.hpp"
#include "Atom.hpp"
#include "Exception.hpp"
#include "Machine.hpp"


// namespace
namespace tos {


/**
 * 词法分析模块
 *
 * 
 */
class Parser {
public:
	
	Parser (istream &pStream, Machine &pMachine)
		: mStream(pStream)
		, mMachine(pMachine) {
	}

	//
	// 解析流中的一个原子，并返回
	// 

	void parseDatum ();

protected:

	int readChar () {
		return  mStream.get ();
	}

	int peekChar () {
		return mStream.peek ();
	}

	Char readCharNoEof () {
		int c = readChar ();
		if (c == EOF) {
			throw ParserException ("Unexpected EOF");
		}
		
		return (Char)c;
	}

	int skipWhitespaceRead ();

	void parseChar ();

	void parseVector ();

	void parseList ();

	void parseString ();

	void parseNumberOrSymbol (Char ch);

	bool isDelimiter (int c) {
		return isWhitespace (c)
			|| (c == '(')
			|| (c == ')')
			|| (c == '"')
			|| (c == ';');
	}

	bool isWhitespace (int c) {
		return (c == ' ')
			|| (c == '\t')
			|| (c == '\n');
	}

	bool isInitial (Char c) {
		return isLetter(c)
			|| isSpecialInitial(c);
	}

	bool isSpecialInitial (Char c) {
		string str ("!$%&*/:<=>?^_~");
		return str.find (c) != string::npos;
	}

	bool isSubsequent (Char c) {
		string str ("+-.@");
		return isInitial (c)
			|| isdigit(c)
			|| (str.find(c) != string::npos);
	}

	bool isLetter (Char c) {
		return ('a' <= c) && (c <= 'z');
	}
	
private:
	// 
	// 源程序输入流
	//
	// 可以是ifstream、istringstream等的对象
	// 
	istream &mStream;

	Machine &mMachine;
};



} // namespace


#endif	// Parser_HPP
