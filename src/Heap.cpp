// -*-c++-*-

// headers
#include "Heap.hpp"
#include "Exception.hpp"
#include "Trace.hpp"


// namespace
namespace tos {


	void Memory::checkIndex (Pointer index) {
		if (index >= mLength)
			throw OutOfRangeException ();
	}


	Atom Heap::saveRegisters (Atom *pRegs) {
		// 拷贝寄存器，并将它们连起来
		Atom root = Atom::makeNull ();
		for (Pointer i=0; i<mSkipForRegister; ++i) {
			root = cons (pRegs [i], root);
			//mMemory [mTheCars + i] = pRegs [i];
			//mMemory [mTheCdrs + i] = Atom::makePair(i+1);
		}
		// 设置最后一个元素的CDR域为NullList
		//mMemory [mTheCdrs + mSkipForRegister - 1] = Atom::makeNull();
		return root;
	}

	void Heap::restoreRegisters (Atom *pRegs) {
		Pointer i = mSkipForRegister - 1;
		Atom root = Atom::makePair (0);

		for(;root.isPair().isTrue() && i<mSkipForRegister;
		    root = cdr (root), --i) {
			pRegs[i] = car(root);
		}
	}

	// 进行垃圾回收
	void Heap::gcStopAndCopy (Atom root) {
		TRACE ("Heap::gcStopAndCopy");
		
		Pointer scan = 0;	// 扫描指针，指向的对象的car、
					// cdr域是旧存储区中的。
		mFree = 0;

		//Atom root = Atom::makePair(0);

		Atom oldAtom, newAtom; // 分别对应旧存储区中的对象及其新的位置的对象

		// 拷贝第一个对象
		mMemory[mNewCars+0] = car(root);
		mMemory[mNewCdrs+0] = cdr(root);
		// 设置BROKENHEART标志以及前向指针
		setCar(root,Atom::makeBrokenHeart());
		Value v;
		v.mPointer = mFree;
		Atom newRoot (root.getType(), v);
		setCdr(root,newRoot);
		
		++ mFree;

		// 拷贝所有活动对象
		for (;scan < mFree;++scan) {
			oldAtom = mMemory[mNewCars+scan];
			if (oldAtom.isPointer().isFalse())
				newAtom = oldAtom; // 基本对象直接复制
			else
				newAtom = moveToNew (oldAtom);
			mMemory[mNewCars+scan] = newAtom;

			oldAtom = mMemory[mNewCdrs+scan];
			if (oldAtom.isPointer().isFalse())
				newAtom = oldAtom;
			else
				newAtom = moveToNew (oldAtom);
			mMemory[mNewCdrs+scan] = newAtom;
		}

		// 交换存储区角色
		Pointer tmp;
		tmp = mTheCars;
		mTheCars = mNewCars;
		mNewCars = tmp;

		tmp = mTheCdrs;
		mTheCdrs = mNewCdrs;
		mNewCdrs = tmp;
	}

	Atom Heap::moveToNew (Atom old) {
		Atom oldcr = car(old);

		if(oldcr.isBrokenHeart().isTrue()) {
			return cdr(old);
		} else {
			// 拷贝
			mMemory[mNewCars+mFree] = oldcr;
			oldcr = cdr(old);
			mMemory[mNewCdrs+mFree] = oldcr;

			// 生成新的对象，注意类型要一致
			Value v;
			v.mPointer = mFree;
			Atom newAtom (old.getType(), v);
			
			// 设置broken heart标志
			setCar(old, Atom::makeBrokenHeart());
			// 设置前向指针
			setCdr(old, newAtom);

			// 增加free指针
			++ mFree;

			// 返回新对象
			return newAtom;
		}
	}

} // namespace
