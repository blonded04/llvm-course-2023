#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace llvm;

namespace {
std::string instructionToString(const Instruction& instruction) {
    std::string str;
    raw_string_ostream(str) << instruction;

    return str;
}

using Pattern = std::vector<const Instruction*>;

struct PatternHash {
    std::uint64_t operator()(const Pattern& pattern) const {
        std::uint64_t result = 0ull;
        for (const auto& instruction : pattern) {
            result +=
                std::hash<std::string>{}(instructionToString(*instruction));
        }

        return result;
    }
};

struct PatternEq {
    bool operator()(const Pattern& lhs, const Pattern& rhs) const {
        if (lhs.size() != rhs.size()) {
            return false;
        }

        for (std::size_t i = 0; i < lhs.size(); i++) {
            if (instructionToString(*lhs[i]) != instructionToString(*rhs[i])) {
                return false;
            }
        }
        return true;
    }
};

using PatternStat = std::pair<Pattern, unsigned>;
using PatternStats =
    std::unordered_map<Pattern, unsigned, PatternHash, PatternEq>;

struct PatternAnalyzerPass : public FunctionPass {
  public:
    inline static char ID = 0;
    PatternAnalyzerPass() : FunctionPass(ID) {}

  private:
    void printInstructionStatistics(const Instruction& instruction, const std::string &functionName) {
        outs() << "[INFO][" << functionName << "] \tInstruction:\n[INFO] \t- ";
        instruction.print(outs(), true);
        outs() << "\n[INFO][" << functionName << "] \t\t* Used by:\n";
        for (auto& userInfo : instruction.uses()) {
            User* user = userInfo.getUser();
            outs() << "[INFO][" << functionName << "] \t\t\t- ";
            user->print(outs(), true);
            outs() << "\n";
        }
        outs() << "[INFO][" << functionName << "] \t\t* Uses:\n";
        for (auto& usedInfo : instruction.operands()) {
            Value* use = usedInfo.get();
            outs() << "[INFO][" << functionName << "] \t\t\t- ";
            use->print(outs(), true);
            outs() << "\n";
        }
    }

    static inline constexpr unsigned k_analyze_min_window_size = 1u;
    static inline constexpr unsigned k_analyze_max_window_size = 5u;
    static inline constexpr unsigned k_stats_max_patterns_output_size = 3u;

    void analyzeInstruction(const Instruction& instruction,
                            unsigned window_size, PatternStats& patterns) {
        Pattern pattern;
        unsigned free_slots_in_window = window_size;
        const Instruction* slot_candidate = &instruction;

        do {
            pattern.push_back(slot_candidate);

            slot_candidate = slot_candidate->getPrevNonDebugInstruction();
            free_slots_in_window--;
        } while (slot_candidate != nullptr && free_slots_in_window != 0u);

        std::reverse(pattern.begin(), pattern.end());

        if (pattern.size() == window_size) {
            patterns[pattern]++;
        }
    }

  public:
    bool runOnFunction(Function& function) override {
        if (function.getName().size() >= 3 && function.getName().substr(0, 3) == "sim") {
            return false;
        }

        std::vector<PatternStats> stats(k_analyze_max_window_size -
                                        k_analyze_min_window_size + 1);

        outs() << "[INFO] In a function called " << function.getName() << "!\n";
        for (const auto& basic_block : function) {
            for (const auto& instruction : basic_block) {
                printInstructionStatistics(instruction, function.getName().str());

                for (unsigned window_size = k_analyze_min_window_size;
                     window_size <= k_analyze_max_window_size; window_size++) {
                    analyzeInstruction(
                        instruction, window_size,
                        stats[window_size - k_analyze_min_window_size]);
                }
            }
        }

        outs() << "\n[STAT] Analysis results for a function "
               << function.getName() << ":\n";
        for (unsigned window_size = k_analyze_min_window_size;
             window_size <= k_analyze_max_window_size; window_size++) {
            std::vector<PatternStat> most_frequent_patterns(
                stats[window_size - k_analyze_min_window_size].begin(),
                stats[window_size - k_analyze_max_window_size].end());
            std::sort(most_frequent_patterns.begin(),
                      most_frequent_patterns.end(),
                      [](const PatternStat& lhs, const PatternStat& rhs) {
                          return lhs.second > rhs.second;
                      });

            outs() << "[STAT][" << function.getName() << "] \tFor window of size " << window_size
                   << " most frequent patterns are:\n";
            for (unsigned i = 0u;
                 i <
                 std::min(static_cast<unsigned>(most_frequent_patterns.size()),
                          k_stats_max_patterns_output_size);
                 i++) {
                outs() << "[STAT][" << function.getName() << "] \t\t* Top " << i + 1
                       << " pattern which occurs "
                       << most_frequent_patterns[i].second << " times is:\n";
                for (const auto& instruction :
                     most_frequent_patterns[i].first) {
                    outs() << "[STAT][" << function.getName() << "] \t\t\t- "
                           << instructionToString(*instruction) << "\n";
                }
            }
        }
        outs() << "\n";
        outs() << "\n";

        return false;
    }
};
} // namespace

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerPatternAnalyzerPass(const PassManagerBuilder&,
                                        legacy::PassManagerBase& PM) {
    PM.add(new PatternAnalyzerPass());
}
static RegisterStandardPasses
    RegisterPatternAnalyzerPass(PassManagerBuilder::EP_EarlyAsPossible,
                                registerPatternAnalyzerPass);
