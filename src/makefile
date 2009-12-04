
CC=g++

CXXFLAGS=-O3
#-ggdb -Wall -DDEBUG=1

SOURCES=Trace.cpp Parser.cpp  Machine.cpp Heap.cpp Atom.cpp Test.cpp Evaluator.cpp Main.cpp main.cpp

HEADERS=common.hpp Atom.hpp Exception.hpp Heap.hpp Machine.hpp Trace.hpp Parser.hpp Evaluator.hpp Main.hpp PrimitiveProcedure.hpp

OBJECTS=Trace.o  Parser.o Machine.o Heap.o Atom.o Evaluator.o Main.o main.o Test.o 

BINARYS=test eval

eval:main
	mv main eval

all:main Test
	mv main eval
	mv Test test



main:main.o Trace.o  Parser.o Machine.o Heap.o Atom.o Evaluator.o Main.o

Test:Test.o Trace.o  Parser.o Machine.o Heap.o Atom.o Evaluator.o Main.o


Test.o:Test.cpp $(HEADERS)

Parser.o:Parser.cpp Parser.hpp common.hpp Exception.hpp Machine.hpp Atom.hpp

Trace.o:Trace.cpp Trace.hpp

Machine.o:Machine.cpp Machine.hpp Exception.hpp common.hpp Atom.hpp Heap.hpp

Atom.o:Atom.cpp Atom.hpp common.hpp Exception.hpp

Heap.o:Heap.cpp Heap.hpp Atom.hpp common.hpp Exception.hpp Trace.hpp

Evaluator.o:Evaluator.cpp Evaluator.hpp PrimitiveProcedure.hpp Exception.hpp Atom.hpp Machine.hpp Atom.hpp common.hpp

Main.o:Main.cpp Main.hpp Exception.hpp Machine.hpp Parser.hpp Evaluator.hpp common.hpp

main.o:main.cpp Main.hpp

clean:
	rm -rf $(OBJECTS)

distclean:
	rm -rf $(OBJECTS) $(BINARYS)

