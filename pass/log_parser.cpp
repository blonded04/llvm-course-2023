#include <algorithm>
#include <cstdint>
#include <deque>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>
#include <utility>

constexpr std::size_t k_min_window_size = 2u;
constexpr std::size_t k_max_window_size = 6u;
constexpr std::size_t k_each_window_limit = 3u;

int main() {
    std::vector<std::map<std::vector<std::string>, unsigned>> statistics(k_max_window_size -
                                                                         k_min_window_size + 1);

    std::deque<std::string> last_lines;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (last_lines.size() == k_max_window_size) {
            last_lines.pop_front();
        }

        last_lines.push_back(line);

        for (std::size_t sz = k_min_window_size; sz <= std::min(last_lines.size(), k_max_window_size); sz++) {
            statistics[sz - k_min_window_size][std::vector<std::string>(
                std::next(last_lines.end(), -static_cast<std::int64_t>(sz)), last_lines.end())]++;
        }
    }
    
    std::cout << "Statistics:\n\n";
    for (std::size_t sz = k_min_window_size; sz <= k_max_window_size; sz++) {
        std::vector<std::pair<std::vector<std::string>, unsigned>> most_frequent(
            std::min(k_each_window_limit, statistics[sz - k_min_window_size].size()));
        std::partial_sort_copy(statistics[sz - k_min_window_size].begin(),
                               statistics[sz - k_min_window_size].end(), most_frequent.begin(),
                               most_frequent.end(),
                               [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

        if (most_frequent.empty()) {
            break;
        }

        std::cout << "\tTop " << most_frequent.size() << " most frequent patterns of size " << sz << " are:\n";
        for (std::size_t iter = 0ul; iter < most_frequent.size(); iter++) {
            std::cout << "\t* Top " << iter + 1 << " pattern occurs " << most_frequent[iter].second << " times\n";
            for (const auto& frequent_line : most_frequent[iter].first) {
                std::cout << "\t--" << iter + 1 << "-- " << frequent_line << "\n";
            }
        }
        std::cout << '\n';
    }
}
