// -*-c++-*-


#ifndef Main_HPP
#define Main_HPP


#include "common.hpp"
#include "Exception.hpp"


// 
// 交互式提示符
//

#define TOS_PROMPT_INPUT ";; TOS::Eval input:"
#define TOS_PROMPT_VALUE ";; TOS::Eval value:"

#define BOOTSTRAP_FILE "bootstrap.scm"
#define HELP_MESSAGE							\
	"-s SCRIPT_FILE \t load Scheme source code from SCRIPT_FILE, and exit\n" \
	"-l FILE        load Scheme source code from FILE\n"		\
	"-h, --help     display this help and exit"



// namespace
namespace tos {


	class Main {

	public:

		Main () {

			
		}

		void run ();


		// 启动程序的入口点
		static inline
		int main (int argc, char **argv) {
			Main main;
			try {
				if (main.parseArgs (argc, argv)) {
					try {
						main.loadBootstrap ();
					} catch (Exception &e) {
						cout << "加载bootstrap文件失败 : ";
						cout << e.toString () << endl;
					}

					main.run ();
				}
			} catch (Exception &e) {
				cout << e.toString () << endl;
			}

			return 0;
		}

	protected:

		bool parseArgs (int argc, char **argv) {
			if (argc == 1) { // no args
				mInStream = &cin;
				mOutStream = &cout;
				mIsInter = true;
			} else if (argc == 2) {
				string arg = argv[1];
				if (arg == "-h"
				    || arg == "--help") {
					cout << HELP_MESSAGE << endl;
					return false;
				} else {
					throw ArgException ();
				}
			} else if (argc == 3) {
				// 根据参数初始化

				// -s SCRIPT_FILE
				// -l FILE 从文件中加载源代码执行，完成后不退出。
				// -h 显示帮助

				string arg = argv[1];

				if (arg == "-s") {
					mQuit = true;
				} else if (arg == "-l") {
					mQuit = false;
				} else {
					throw ArgException ();
				}

				mInStream = new ifstream (argv[2]);

				if (! mInStream->good()) {
					throw InnerException ("无法打开代码文件");
				}

				mOutStream = &cout;
				mIsInter = false;
			} else {
				throw ArgException ();
			}

			return true;
		}

		void loadBootstrap ();
		
		// 
		// 打印提示符 - 根据输入流的类型来决定是否需要打印提示
		// 符，只有交互式输入流才需要。
		//
		
		void promptForInput () {
			if (mIsInter)
				(*mOutStream) << TOS_PROMPT_INPUT;
		}

		void promptForOutput () {
			if (mIsInter)
				(*mOutStream) << TOS_PROMPT_VALUE;
		}

		
		void error (string pMessage) {
			(*mOutStream) << endl
				      << pMessage
				      << endl << endl;
		}
		

	private:

		// 是否是交互式输入流
		bool mIsInter;
		// 从文件中运行完后，是否退出
		bool mQuit;


		// Scheme源代码的输入流
		istream *mInStream;

		// Scheme表达式求值结果的输出流
		ostream *mOutStream;

	};



} // namespace



#endif	// Main_HPP
