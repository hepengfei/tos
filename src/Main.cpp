// -*-c++-*-


#include "Main.hpp"

#include "Exception.hpp"
#include "Machine.hpp"
#include "Parser.hpp"
#include "Evaluator.hpp"


// namespace
namespace tos {


	void Main::run () {

		do {
			istream &stream = *mInStream;
			ostream &out = *mOutStream;
			Machine &machine = Machine::instance ();

			Parser parser (stream, machine);
			Evaluator eval (machine);

		
			while (!stream.eof ()) {

				// 提示输入
				promptForInput ();

				// 解析输入内容
				try {
					parser.parseDatum ();
				} catch (Exception &e) {
					error (e.toString ());
					machine.recover ();
					// 出错时清空剩余输入
					stream.ignore (numeric_limits<streamsize>::max(),'\n');
					continue;
				}

				// 若解析结果为EOF对象，则文件处理完毕
				if (machine.reg (R_VAL)
				    .isEof ().isTrue ())
					break;

				// 将读入的表达式放到R_EXP寄存器中，为求值做好
				// 准备
				machine.assign (R_EXP, R_VAL);

				// 约定值 - 参见Evaluator.cpp::eval
				// 设定运行的结束标志为EOF
				machine.assign (R_CONTINUE, Atom::makeEof ());

				// 求值
				try{
					eval ();
				} catch (Exception &e) {
					error (e.toString ());
					machine.recover ();
					continue;
				}

				// 将求值后的结果输出
				if (mIsInter) {
					out << endl;
					promptForOutput ();
					out << machine.getString (R_VAL);
					out << endl << endl;
				}
			}

			// 非交互式终端，不输出结果
			if (! mIsInter) {
				//out << machine.getString (R_VAL);
				//out << endl;

				// 若不立即退出，则继续从标准输入读取。
				if (! mQuit) {
					delete mInStream;
					mInStream = &cin;

					// 现在是交互式终端了
					mIsInter = true;
				} else
					break;
			} else
				break;
		} while (true);		
	}

	void Main::loadBootstrap () {
			ifstream stream( BOOTSTRAP_FILE );

			if (!stream.good ())
				throw InnerException ("无法打开bootstrap文件");

			Machine &machine = Machine::instance ();

			Parser parser (stream, machine);
			Evaluator eval (machine);

			while (! stream.eof ()) {
				parser.parseDatum ();

				if (machine.reg (R_VAL).isEof ().isTrue ())
					break;

				machine.assign (R_EXP, R_VAL);
				machine.assign (R_CONTINUE, Atom::makeEof ());

				eval ();
			}
		};
		


} // namespace



