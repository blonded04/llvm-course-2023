#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

using namespace llvm;

namespace {
struct MyPass : public FunctionPass {
  public:
    inline static char ID = 0;
    MyPass() : FunctionPass(ID) {}

  private:
    using Pattern = std::vector<Instruction>;
    using PatternStat = std::pair<Pattern, unsigned> using PatternStats =
        std::map<Pattern, unsigned>;

    void printInstructionStatistics(const Instruction& instruction) {
        outs() << "\tInstruction:\n\t- ";
        instruction.print(outs(), true);
        outs() << "\n\t\tUsed by:\n";
        for (auto& userInfo : instruction.uses()) {
            User* user = userInfo.getUser();
            outs << "\t\t* " user->print(outs(), true);
            outs() << "\n";
        }
        outs() << "\t\tUses:\n";
        for (auto& usedInfo : instruction.operands()) {
            Value* use = usedInfo.get();
            outs() << "\t\t- " use->print(outs(), true);
            outs() << "\n";
        }
    }

    static inline constexpr unsigned k_analyze_min_window_size = 2u;
    static inline constexpr unsigned k_analyze_max_window_size = 5u;
    static inline constexpr unsigned k_stats_max_patterns_output_size = 5u;

    void analyzeInstruction(const Instruction& instruction,
                            unsigned window_size, PatternStats& patterns) {
        Pattern pattern;
        unsigned free_slots_in_window = window_size;
        const Instruction* slot_candidate = &instruction;

        do {
            pattern.push_back(instruction);

            slot_candidate = slot_candidate->getPrevNonDebugInstruction();
            free_slots_in_window--;
        } while (slot_candidate != nullptr && free_slots_in_window != 0u);

        std::reverse(pattern.begin(), pattern.end());

        if (pattern.size() == window_size) {
            pattern[pattern]++;
        }
    }

  public:
    bool runOnFunction(const Function& function) override {
        std::vector<PatternStats> stats(k_analyze_max_window_size -
                                        k_analyze_min_window_size + 1);

        outs() << "In a function called " << function.getName() << "!\n";
        for (const auto& basic_block : function) {
            for (const auto& instruction : basic_block) {
                printInstructionStatistics(instruction);

                for (unsigned window_size = k_analyze_min_window_size;
                     window_size <= k_analyze_max_window_size; window_size++) {
                    analyzeInstruction(
                        instruction, window_size,
                        stats[window_size - k_analyze_min_window_size]);
                }
            }
        }

        outs() << "Analysis results for a function " << function.getName()
               << ":\n";
        for (unsigned window_size = k_analyze_min_window_size;
             window_size <= k_analyze_max_window_size; window_size++) {
            std::vector<PatternStat> most_frequent_patterns(
                stats[window_size].begin(), stats[window_size].end());
            std::sort(most_frequent_patterns.begin(),
                      most_frequent_patterns.end(),
                      [](const PatternStat& lhs, const PatternStat& rhs) {
                          return lhs.second > rhs.second;
                      });

            outs() << "\tFor window of size " << window_size
                   << " most frequent patterns are:\n";
            for (unsigned i = 0u;
                 i < std::max(most_frequent_patterns.size(),
                              k_stats_max_patterns_output_size);
                 i++) {
                outs() << "\t\t* Top " << i + 1 << " pattern is:\n";
                for (const auto& instruction :
                     most_frequent_patterns[i].first) {
                    outs << "\t\t\t- ";
                    instruction.print(outs(), true);
                    outs << "\n";
                }
            }
        }
        outs << "\n";

        return false;
    }
};
} // namespace

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerMyPass(const PassManagerBuilder&,
                           legacy::PassManagerBase& PM) {
    PM.add(new MyPass());
}
static RegisterStandardPasses
    RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerMyPass);
