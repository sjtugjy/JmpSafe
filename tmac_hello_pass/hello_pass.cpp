#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

	struct TmacHello : public ModulePass {
		static char ID; /* Pass ID */
		unsigned long inst_cnt;
		unsigned long direct_call;
		unsigned long indirect_call;
		unsigned long ret_cnt;
		unsigned long direct_branch;
		unsigned long indirect_branch;

		TmacHello() : ModulePass(ID)
		{
			inst_cnt = 0;
			direct_call = 0;
			indirect_call = 0;
			ret_cnt = 0;
			direct_branch = 0;
			indirect_branch = 0;
		}

		void pr_info()
		{
			errs() << "direct_call: " << direct_call << "\n";
			errs() << "indirect_call: " << indirect_call << "\n";
			errs() << "ret_cnt: " << ret_cnt << "\n";
			errs() << "direct_branch: " << direct_branch << "\n";
			errs() << "indirect_branch: " << indirect_branch << "\n";
		}

#if 0
		/* Sample */
		bool runOnModule(Module &m) override {
			/* Traverse function list in the module */
			for (Function &f : m.getFunctionList()) {
				++func_cnt;
				errs() << "hello: ";
				errs().write_escaped(f.getName()) << "\n";
			}
			return false;
		}
#endif
		bool runOnModule(Module &m) override;
	};

	/* tmac: Run on each module (file?) */
	bool TmacHello::runOnModule(Module &m)
	{
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

			/*
			 * The function is at least referrd by address once.
			 * hasAddressTaken - returns true if there are any uses of this function other than direct calls or invokes to it.
			 */
			if (f.hasAddressTaken()) {
				/* We should insert a TAG to these functions */
				errs() << "[Candidate] a candidate target function of indirect call: ";
				errs().write_escaped(f.getName()) << "\n";
			}

			/* Skip declaration */
			if (f.isDeclaration()) continue;

			/* Instrument-type-one: Insert the tag instruction at the entry of each function */
						
			BasicBlock &entry_block = f.front();
			// BasicBlock &entry_block = f.getEntryBlock();
			Instruction &entry_inst = entry_block.front();

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
							indirect_call += 1;
						}
						else {
							// errs() << "[tmac] A direct call instruction to func: ";
							// errs().write_escaped(cf->getName()) << "\n";

							/* Instrument-type-two: Insert the tag instruction after the call instruction */
						
							/* LLVM CallInst for inline asm */
							// CallInst *callasm = CallInst::Create(tagnop, ArrayRef<Value *>());
							// callasm->insertAfter(ci);
							direct_call += 1;
						}
						continue;
					} else if (auto *ri = dyn_cast<ReturnInst>(&inst)) {
						
						/* Instrument-type-three: check the tag before each return instruction */
						ret_cnt += 1;
						/* LLVM CallInst for inline asm */
						// CallInst *callasm = CallInst::Create(tagnop, ArrayRef<Value *>());
						// callasm->insertBefore(ri);

						continue;
					} else if (auto *bi = dyn_cast<BranchInst>(&inst)) {
						direct_branch += 1;
						continue;
					} else if (auto *ii = dyn_cast<IndirectBrInst>(&inst)) {
						indirect_branch += 1;	
						continue;
					}



					/*
					// Print each IR instruction
					if (strstr(inst.getOpcodeName(), "call")) {
						errs() << "[tmac] Find a call instruction\n";
					}

					errs() << "Inst-" << inst_cnt++
						<< " ";
					errs().write_escaped(inst.getOpcodeName())
						<< "\n";
					*/
				}
			}
		}

		pr_info();
		return true;
	}
}


/* Initial ID value is not used. The address of ID is used instead. */
char TmacHello::ID = 0;

/* @command-line arg: TmacHello; @name: Tmac Hello World Pass */
static RegisterPass<TmacHello> X("Jmpsafe", "Tmac: Jmpsafe Pass", true, false);
			     //false, false);

