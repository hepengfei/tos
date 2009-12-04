

#include "Atom.hpp"


// namespace
namespace tos {


	string Atom::getString () const {
		stringstream buffer;

		switch (getType ()) {
		case T_BROKENHEART:
			buffer << "#[broken-heart "
			       << getValue ().mPointer
			       << "]";
			break;
		case T_EOF:
			buffer << "#[eof "
			       << getValue ().mPointer
			       << "]";
			break;
		case T_NULL:
			buffer << "#[null  "
			       << getValue ().mPointer
			       << "]";
			break;
		case T_CHAR:
			buffer << "#[char ";
			buffer << getChar ();
			buffer << "]";
			break;
		case T_INTEGER:
			buffer << "#[integer "
			       << getInteger ()
			       << "]";
			break;
		case T_BOOLEAN:
			buffer << "#[boolean "
			       << getBoolean ()
			       << "]";
			break;
		case T_PRIMITIVE_PROCEDURE:
			buffer << "#[primitive-procedure "
			       << getValue ().mInteger
			       << "]";
			break;
		case T_PORT:
			buffer << "#[port "
			       << getValue ().mInteger
			       << "]";
			break;
		case T_POINTER:
			buffer << "#[pointer "
			       << getPointer ()
			       << "]";
			break;
		case T_PAIR:
			buffer << "#[pair "
			       << getPointer ()
			       << "]";
			break;
		case T_VECTOR:
			buffer << "#[vector "
			       << getPointer ()
			       << "]";
			break;
		case T_STRING:
			buffer << "#[string "
			       << getPointer ()
			       << "]";
			break;
		case T_SYMBOL:
			buffer << "#[symbol "
			       << getPointer ()
			       << "]";
			break;
		case T_PROCEDURE:
			buffer << "#[procedure "
			       << getPointer ()
			       << "]";
			break;
		default:
			buffer << "#[unknown]";
			break;
		}
		return buffer.str ();
	}
} // namespace
