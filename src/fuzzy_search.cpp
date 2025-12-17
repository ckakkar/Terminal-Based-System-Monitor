#include "fuzzy_search.hpp"
#include <algorithm>
#include <cctype>

int FuzzySearch::levenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t len1 = s1.size();
    const size_t len2 = s2.size();
    
    if (len1 == 0) return len2;
    if (len2 == 0) return len1;
    
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));
    
    for (size_t i = 0; i <= len1; ++i) {
        dp[i][0] = i;
    }
    for (size_t j = 0; j <= len2; ++j) {
        dp[0][j] = j;
    }
    
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i - 1][j] + 1,      // deletion
                dp[i][j - 1] + 1,      // insertion
                dp[i - 1][j - 1] + cost // substitution
            });
        }
    }
    
    return dp[len1][len2];
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
    
    // Check for substring match first (exact match)
    if (lower_text.find(lower_query) != std::string::npos) {
        return true;
    }
    
    // Check fuzzy similarity
    double sim = similarity(lower_text, lower_query);
    return sim >= threshold;
}

double FuzzySearch::getMatchScore(const std::string& text, const std::string& query) {
    if (query.empty()) return 1.0;
    
    std::string lower_text = toLower(text);
    std::string lower_query = toLower(query);
    
    // Exact substring match gets highest score
    if (lower_text.find(lower_query) != std::string::npos) {
        size_t pos = lower_text.find(lower_query);
        // Prefer matches at the beginning
        return 1.0 + (1.0 / (pos + 1));
    }
    
    // Return similarity score
    return similarity(lower_text, lower_query);
}

std::string FuzzySearch::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

