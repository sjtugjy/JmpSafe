#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IntrinsicInst.h"
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
		/* Traverse function list in the module */
		for (Function &f : m.getFunctionList()) {
			for (BasicBlock &b : f.getBasicBlockList()) {
				for (Instruction &inst : b.getInstList()) {

					//errs() << "[tmac] skip the inline asm (inserted by myself\n";
					//continue;

					if (auto *ci = dyn_cast<CallInst>(&inst)) {
						errs() << "[tmac] A call instruction (ci) to func ";
						errs().write_escaped(ci->getCalledFunction()->getName())
							<< "\n";
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
		return false;
	}
}


/* Initial ID value is not used. The address of ID is used instead. */
char TmacHello::ID = 0;

/* @command-line arg: TmacHello; @name: Tmac Hello World Pass */
static RegisterPass<TmacHello> X("Jmpsafe", "Tmac: Jmpsafe Pass");
			     //false, false);

