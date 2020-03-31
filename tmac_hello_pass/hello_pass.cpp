#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

int tmac_func_cnt = 0;

namespace {

	struct TmacHello : public ModulePass {
	//struct TmacHello : public FunctionPass {
		static char ID; /* Pass ID */
		TmacHello() : ModulePass(ID) {}
		//TmacHello() : FunctionPass(ID) {}

		//bool runOnFunction(Function &F) override {
		bool runOnModule(Module &F) override {
			for (Function &FF : F.getFunctionList()) {
			++tmac_func_cnt;
			errs() << "Hello: ";
			errs().write_escaped(FF.getName()) << "\n";
			}
			return false;
		}
	};
}

/* Initial ID value is not used. The address of ID is used instead. */
char TmacHello::ID = 0;

/* @command-line arg: TmacHello; @name: Tmac Hello World Pass */
static RegisterPass<TmacHello> X("TmacHello", "Tmac Hello World Pass");
			     //false, false);

