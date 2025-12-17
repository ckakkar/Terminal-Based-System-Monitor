#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

class FuzzySearch {
public:
    // Calculate Levenshtein distance between two strings
    static int levenshteinDistance(const std::string& s1, const std::string& s2);
    
    // Calculate similarity score (0.0 to 1.0, where 1.0 is exact match)
    static double similarity(const std::string& s1, const std::string& s2);
    
    // Check if query matches string (case-insensitive substring or fuzzy match)
    static bool matches(const std::string& text, const std::string& query, double threshold = 0.3);
    
    // Get match score for ranking
    static double getMatchScore(const std::string& text, const std::string& query);
    
private:
    static std::string toLower(const std::string& str);
};

