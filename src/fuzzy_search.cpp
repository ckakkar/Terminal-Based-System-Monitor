#include "fuzzy_search.hpp"
#include <algorithm>
#include <cctype>
#include <vector>

int FuzzySearch::levenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t m = s1.size();
    const size_t n = s2.size();
    
    if (m == 0) return static_cast<int>(n);
    if (n == 0) return static_cast<int>(m);
    
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));
    
    for (size_t i = 0; i <= m; ++i) dp[i][0] = static_cast<int>(i);
    for (size_t j = 0; j <= n; ++j) dp[0][j] = static_cast<int>(j);
    
    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i - 1][j] + 1,
                dp[i][j - 1] + 1,
                dp[i - 1][j - 1] + cost
            });
        }
    }
    
    return dp[m][n];
}

double FuzzySearch::similarity(const std::string& s1, const std::string& s2) {
    if (s1.empty() && s2.empty()) return 1.0;
    if (s1.empty() || s2.empty()) return 0.0;
    
    int distance = levenshteinDistance(s1, s2);
    int max_len = std::max(s1.size(), s2.size());
    
    return 1.0 - (static_cast<double>(distance) / max_len);
}

bool FuzzySearch::matches(const std::string& text, const std::string& query, double threshold) {
    if (query.empty()) return true;
    
    std::string lower_text = toLower(text);
    std::string lower_query = toLower(query);
    
    if (lower_text.find(lower_query) != std::string::npos) {
        return true;
    }
    
    return similarity(lower_text, lower_query) >= threshold;
}

double FuzzySearch::getMatchScore(const std::string& text, const std::string& query) {
    if (query.empty()) return 1.0;
    
    std::string lower_text = toLower(text);
    std::string lower_query = toLower(query);
    
    size_t pos = lower_text.find(lower_query);
    if (pos != std::string::npos) {
        return 1.0 + (1.0 / static_cast<double>(pos + 1));
    }
    
    return similarity(lower_text, lower_query);
}

std::string FuzzySearch::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

