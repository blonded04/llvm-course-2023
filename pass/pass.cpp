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
        return name == "pass_binOptLogger" || name == "pass_callLogger" || name == "pass_funcStartLogger" ||
               name == "pass_funcEndLogger";
    }

    bool runOnFunction(Function& F) override {
        if (isFuncLogger(F.getName())) {
            return false;
        }

        // Prepare builder for IR modification
        LLVMContext& Ctx = F.getContext();
        IRBuilder<> builder(Ctx);
        Type* retType = Type::getVoidTy(Ctx);

        // Prepare funcStartLogger function
        ArrayRef<Type*> funcStartParamTypes = {builder.getInt8Ty()->getPointerTo()};
        FunctionType* funcStartLogFuncType = FunctionType::get(retType, funcStartParamTypes, false);
        FunctionCallee funcStartLogFunc =
            F.getParent()->getOrInsertFunction("pass_funcStartLogger", funcStartLogFuncType);

        // Insert a call to funcStartLogger function in the function begin
        BasicBlock& entryBB = F.getEntryBlock();
        builder.SetInsertPoint(&entryBB.front());
        Value* funcName = builder.CreateGlobalStringPtr(F.getName());
        Value* args[] = {funcName};
        builder.CreateCall(funcStartLogFunc, args);

        // Prepare callLogger function
        ArrayRef<Type*> callParamTypes = {builder.getInt8Ty()->getPointerTo(),
                                          builder.getInt8Ty()->getPointerTo(), Type::getInt64Ty(Ctx)};
        FunctionType* callLogFuncType = FunctionType::get(retType, callParamTypes, false);
        FunctionCallee callLogFunc = F.getParent()->getOrInsertFunction("pass_callLogger", callLogFuncType);

        // Prepare funcEndLogger function
        ArrayRef<Type*> funcEndParamTypes = {builder.getInt8Ty()->getPointerTo(), Type::getInt64Ty(Ctx)};
        FunctionType* funcEndLogFuncType = FunctionType::get(retType, funcEndParamTypes, false);
        FunctionCallee funcEndLogFunc =
            F.getParent()->getOrInsertFunction("pass_funcEndLogger", funcEndLogFuncType);

        // Prepare binOptLogger function
        ArrayRef<Type*> binOptParamTypes = {Type::getInt32Ty(Ctx),
                                            Type::getInt32Ty(Ctx),
                                            Type::getInt32Ty(Ctx),
                                            builder.getInt8Ty()->getPointerTo(),
                                            builder.getInt8Ty()->getPointerTo(),
                                            Type::getInt64Ty(Ctx)};
        FunctionType* binOptLogFuncType = FunctionType::get(retType, binOptParamTypes, false);
        FunctionCallee binOptLogFunc =
            F.getParent()->getOrInsertFunction("pass_binOptLogger", binOptLogFuncType);

        // Insert loggers for call, binOpt and ret instructions
        for (auto& B : F) {
            for (auto& I : B) {
                Value* valueAddr = ConstantInt::get(builder.getInt64Ty(), (int64_t)(&I));
                if (auto* call = dyn_cast<CallInst>(&I)) {
                    // Insert before call
                    builder.SetInsertPoint(call);

                    // Insert a call to callLogger function
                    Function* callee = call->getCalledFunction();
                    if (callee && !isFuncLogger(callee->getName())) {
                        Value* calleeName = builder.CreateGlobalStringPtr(callee->getName());
                        Value* funcName = builder.CreateGlobalStringPtr(F.getName());
                        Value* args[] = {funcName, calleeName};
                        builder.CreateCall(callLogFunc, args);
                    }
                }
                if (auto* ret = dyn_cast<ReturnInst>(&I)) {
                    // Insert before ret
                    builder.SetInsertPoint(ret);

                    // Insert a call to funcEndLogFunc function
                    Value* funcName = builder.CreateGlobalStringPtr(F.getName());
                    Value* args[] = {funcName};
                    builder.CreateCall(funcEndLogFunc, args);
                }
                if (auto* op = dyn_cast<BinaryOperator>(&I)) {
                    // Insert after op
                    builder.SetInsertPoint(op);
                    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

                    // Insert a call to binOptLogFunc function
                    Value* funcName = builder.CreateGlobalStringPtr(F.getName());
                    Value* opName = builder.CreateGlobalStringPtr(op->getOpcodeName());
                    Value* args[] = {opName, funcName};
                    builder.CreateCall(binOptLogFunc, args);
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
static RegisterStandardPasses RegisterPatternAnalyzerPass(PassManagerBuilder::EP_EarlyAsPossible,
                                                          registerPatternAnalyzerPass);
