// -*-c++-*-

#ifndef Heap_HPP
#define Heap_HPP

// headers
#include "common.hpp"
#include "Atom.hpp"

// namespace
namespace tos {


	// 
	// 内存
	// 
	// 向量形式的内存组织。应该可以用std::vector来替代它。
	// 

	class Memory {

	public:

		Memory (const size_t pLength)
			: mLength(pLength) {
			mStart = new Atom [pLength];
		}

		~Memory () {
			delete [] mStart;
		}

		size_t length () const {
			return mLength;
		}

		Atom &operator[] (Pointer pIndex) {
			checkIndex (pIndex);
			return mStart [pIndex];
		}

	protected:
		// 检查下标是否越界
		void checkIndex (Pointer index);

	private:
		Atom *mStart;		// 数组起始位置
		const size_t mLength;	// 数组长度
	};



	//
	// 堆
	//
	
	class Heap {

	public:

		// 指定堆大小和保留位置大小
		Heap (const size_t pSize, const size_t pSkip)
			: mMemory(4 * pSize)
			, mSize(pSize)
			, mSkipForRegister(pSkip) {
			initPointer ();
		}

		size_t size () const {
			return mSize;
		}

		Pointer free () const {
			return mFree;
		}

		// 检查是否有可分配空间
		bool hasSpace () const {
			return free () + mSkipForRegister < size ();
		}


		//
		// 基本表操作 - 基于向量操作实现
		//
		// 在进行这些操作之前，已经检查过堆空间了，因此有足够的
		// 空间来完成这些操作
		// 

		// 创建复合对象 - 在调用此函数前_一定_要检查剩余空间是
		// 否足够。
		Atom cons (Atom car, Atom cdr) {
			mMemory[mTheCars + mFree] = car;
			mMemory[mTheCdrs + mFree] = cdr;
			Atom pair = Atom::makePair (mFree);
			++ mFree;

			return pair;
		}

		Atom car (Atom pair) {
			Pointer ptr = mTheCars + pair.getPointer();
			return mMemory[ptr];
		}

		Atom cdr (Atom pair) {
			Pointer ptr = mTheCdrs + pair.getPointer();
			return mMemory[ptr];
		}

		void setCar (Atom pair, Atom value) {
			Pointer ptr = mTheCars + pair.getPointer();
			mMemory[ptr] = value;
		}

		void setCdr (Atom pair, Atom value) {
			Pointer ptr = mTheCdrs + pair.getPointer();
			mMemory[ptr] = value;
		}

		// 保存mSkipForRegister个寄存器的值到当前存储区的开始位置，并形
		// 成一张表。
		Atom saveRegisters (Atom *pRegs);

		// 从回收后的存储区中还原寄存器。第一个寄存器一定在存储区的第一
		// 个位置，然后顺着表即可找到所有寄存器。
		void restoreRegisters (Atom *pRegs);

		// 进行垃圾回收
		void gcStopAndCopy (Atom root);

		// 获取某一段内存的内容
		string getString (Pointer start, Pointer end) {
			stringstream buffer;

			// header
			buffer << "index \t cars \t cdrs" << endl;

			for (Pointer it=start; it<=end; ++it) {
				buffer << it << '\t';
				buffer << mMemory [mTheCars + it].getString () << '\t';
				buffer << mMemory [mTheCdrs + it].getString () << endl;
			}
			return buffer.str ();
		}

	protected:

		// 将对象移到新存储区，并返回新存储区相应的对象。旧存储区中相应
		// 位置要设置BROKENHEART标志和前向指针。传入的始终是一个复合对
		// 象。
		Atom moveToNew (Atom old);

		// 初始化堆指针
		void initPointer () {
			mFree = 0;

			mTheCars = 0;
			mTheCdrs = mSize;

			mNewCars = mSize + mSize;
			mNewCdrs = mSize + mNewCars; 
		}

	private:
		Memory mMemory;		// 堆空间，其长度为4*mSize
		const size_t mSize;	// 堆大小

		Pointer mFree;

		Pointer mTheCars;
		Pointer mTheCdrs;

		Pointer mNewCars;
		Pointer mNewCdrs;

		const size_t mSkipForRegister; // = REG_NUM
	};

} // namespace

#endif	// Heap_HPP

