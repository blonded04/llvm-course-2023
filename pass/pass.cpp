#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {
struct PatternAnalyzerPass : public FunctionPass {
    static inline char ID = 0;
    PatternAnalyzerPass() : FunctionPass(ID) {}

    bool isFuncLogger(StringRef name) {
        return name == "pass_callLogger" || name == "pass_funcEndLogger" || name == "pass_nonPhiNodeLogger";
    }

    bool runOnFunction(Function& F) override {
        if (isFuncLogger(F.getName())) {
            return false;
        }

        LLVMContext& Ctx = F.getContext();
        IRBuilder<> builder(Ctx);
        Type* retType = Type::getVoidTy(Ctx);

        ArrayRef<Type*> callParamTypes = {builder.getInt8Ty()->getPointerTo(),
                                          builder.getInt8Ty()->getPointerTo()};
        FunctionType* callLogFuncType = FunctionType::get(retType, callParamTypes, false);
        FunctionCallee callLogFunc = F.getParent()->getOrInsertFunction("pass_callLogger", callLogFuncType);

        ArrayRef<Type*> funcEndParamTypes = {builder.getInt8Ty()->getPointerTo()};
        FunctionType* funcEndLogFuncType = FunctionType::get(retType, funcEndParamTypes, false);
        FunctionCallee funcEndLogFunc =
            F.getParent()->getOrInsertFunction("pass_funcEndLogger", funcEndLogFuncType);

        ArrayRef<Type*> nonPhiNodeParamTypes = {builder.getInt8Ty()->getPointerTo(),
                                                builder.getInt8Ty()->getPointerTo()};
        FunctionType* nonPhiNodeLogFuncType = FunctionType::get(retType, nonPhiNodeParamTypes, false);
        FunctionCallee nonPhiNodeLogFunc =
            F.getParent()->getOrInsertFunction("pass_nonPhiNodeLogger", nonPhiNodeLogFuncType);

        for (auto& B : F) {
            for (auto& I : B) {
                Value* valueAddr = ConstantInt::get(builder.getInt64Ty(), (int64_t)(&I));
                if (auto* call = dyn_cast<CallInst>(&I)) {
                    builder.SetInsertPoint(call);

                    Function* callee = call->getCalledFunction();
                    if (callee && !isFuncLogger(callee->getName())) {
                        Value* calleeName = builder.CreateGlobalStringPtr(callee->getName());
                        Value* funcName = builder.CreateGlobalStringPtr(F.getName());
                        Value* args[] = {funcName, calleeName};
                        builder.CreateCall(callLogFunc, args);
                    }
                } else if (auto* ret = dyn_cast<ReturnInst>(&I)) {
                    builder.SetInsertPoint(ret);

                    Value* funcName = builder.CreateGlobalStringPtr(F.getName());
                    Value* args[] = {funcName};
                    builder.CreateCall(funcEndLogFunc, args);
                } else if (auto* op = dyn_cast<Instruction>(&I); dyn_cast<PHINode>(&I) == nullptr) {
                    builder.SetInsertPoint(op);

                    Value* funcName = builder.CreateGlobalStringPtr(F.getName());
                    Value* opName = builder.CreateGlobalStringPtr(op->getOpcodeName());

                    Value* args[] = {opName, funcName};
                    builder.CreateCall(nonPhiNodeLogFunc, args);
                }
            }
        }
        return true;
    }
};
} // namespace

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerPatternAnalyzerPass(const PassManagerBuilder&, legacy::PassManagerBase& PM) {
    PM.add(new PatternAnalyzerPass());
}
static RegisterStandardPasses RegisterPatternAnalyzerPass(PassManagerBuilder::EP_OptimizerLast,
                                                          registerPatternAnalyzerPass);
