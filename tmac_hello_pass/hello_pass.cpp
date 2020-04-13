#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

int tmac_func_cnt = 0;

namespace {

	struct TmacHello : public ModulePass {
		static char ID; /* Pass ID */
		unsigned long inst_cnt;

		TmacHello() : ModulePass(ID), inst_cnt(0) {}

#if 0
		/* Sample */
		bool runOnModule(Module &m) override {
			/* Traverse function list in the module */
			for (Function &f : m.getFunctionList()) {
				++tmac_func_cnt;
				errs() << "hello: ";
				errs().write_escaped(f.getName()) << "\n";
			}
			return false;
		}
#endif
		bool runOnModule(Module &m) override;
	};

	/* tmac: Run on each module (file?) */
	bool TmacHello::runOnModule(Module &m) {
		/*
		   InlineAsm * InlineAsm::get(
		   FunctionType *	Ty,
		   StringRef	AsmString,
		   StringRef	Constraints,
		   bool	hasSideEffects,
		   bool	isAlignStack = false,
		   AsmDialect	asmDialect = AD_ATT 
		   )	
		   */

		/*
		 * This is the factory function for the FunctionType class.
		 * FunctionType *FunctionType::get(Type *ReturnType, ArrayRef<Type*> Params, bool isVarArg)
		 */

		/* Instrumentation: mark each legal indirect jmp/call/ret target with a TAG */
		InlineAsm *tagnop;
		tagnop = InlineAsm::get(FunctionType::get(Type::getVoidTy(m.getContext()), ArrayRef<Type *>(), false),
					"nopl 5201314(%rax, %rax, 4)\n\t",
					"",
					false);

		/* Traverse function list in the module */
		for (Function &f : m.getFunctionList()) {

			/* Instrument-type-one: Insert the tag instruction at the entry of each function */
						
			// BasicBlock &entry_block = f.front();
			BasicBlock &entry_block = f.getEntryBlock();
			Instruction &entry_inst = entry_block.front();

			/* LLVM CallInst for inline asm */
			CallInst *callasm = CallInst::Create(tagnop, ArrayRef<Value *>());
			if (auto *inst = dyn_cast<Instruction>(&entry_inst)) {
				errs() << "[tag-type-1] Function entry.\n";
				callasm->insertBefore(inst);
			}

			for (BasicBlock &b : f.getBasicBlockList()) {
				for (Instruction &inst : b.getInstList()) {

					if (auto *ci = dyn_cast<CallInst>(&inst)) {

						if (ci->isInlineAsm()) {
							errs() << "[tmac] skip the inline asm.\n";
							continue;
						}

						/* ci calls which function (cf) */
						Function *cf;

						cf = ci->getCalledFunction();
						if (cf == NULL) {
							/* TODO: no called function means indrect call? */
							errs() << "[tmac] An indrect call instruction\n";
						}
						else {
							errs() << "[tmac] A direct call instruction to func: ";
							errs().write_escaped(cf->getName()) << "\n";

							/* Instrument-type-two: Insert the tag instruction after the call instruction */
						
							/* LLVM CallInst for inline asm */
							CallInst *callasm = CallInst::Create(tagnop, ArrayRef<Value *>());
							callasm->insertAfter(ci);
						}
					}

					if (strstr(inst.getOpcodeName(), "call")) {
						errs() << "[tmac] Find a call instruction\n";
					}

					errs() << "Inst-" << inst_cnt++
						<< " ";
					errs().write_escaped(inst.getOpcodeName())
						<< "\n";
				}
			}
		}
		return true;
	}
}


/* Initial ID value is not used. The address of ID is used instead. */
char TmacHello::ID = 0;

/* @command-line arg: TmacHello; @name: Tmac Hello World Pass */
static RegisterPass<TmacHello> X("Jmpsafe", "Tmac: Jmpsafe Pass", true, false);
			     //false, false);

