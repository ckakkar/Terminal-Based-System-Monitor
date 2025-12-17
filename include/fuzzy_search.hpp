#pragma once

#include <string>

class FuzzySearch {
public:
    static int levenshteinDistance(const std::string& s1, const std::string& s2);
    static double similarity(const std::string& s1, const std::string& s2);
    static bool matches(const std::string& text, const std::string& query, double threshold = 0.3);
    static double getMatchScore(const std::string& text, const std::string& query);

private:
    static std::string toLower(const std::string& str);
};

