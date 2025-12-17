#include <gtest/gtest.h>
#include "fuzzy_search.hpp"

class FuzzySearchTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FuzzySearchTest, LevenshteinDistance_ExactMatch) {
    EXPECT_EQ(0, FuzzySearch::levenshteinDistance("hello", "hello"));
    EXPECT_EQ(0, FuzzySearch::levenshteinDistance("", ""));
}

TEST_F(FuzzySearchTest, LevenshteinDistance_SimpleCases) {
    EXPECT_EQ(1, FuzzySearch::levenshteinDistance("hello", "hallo"));
    EXPECT_EQ(1, FuzzySearch::levenshteinDistance("hello", "hell"));
    EXPECT_EQ(1, FuzzySearch::levenshteinDistance("hello", "helloo"));
    EXPECT_EQ(3, FuzzySearch::levenshteinDistance("kitten", "sitting"));
}

TEST_F(FuzzySearchTest, Similarity_ExactMatch) {
    EXPECT_DOUBLE_EQ(1.0, FuzzySearch::similarity("hello", "hello"));
    EXPECT_DOUBLE_EQ(1.0, FuzzySearch::similarity("", ""));
}

TEST_F(FuzzySearchTest, Similarity_SimilarStrings) {
    double sim = FuzzySearch::similarity("hello", "hallo");
    EXPECT_GT(sim, 0.7);
    EXPECT_LE(sim, 1.0);
    
    sim = FuzzySearch::similarity("kitten", "sitting");
    EXPECT_GT(sim, 0.0);
    EXPECT_LT(sim, 1.0);
}

TEST_F(FuzzySearchTest, Matches_SubstringMatch) {
    EXPECT_TRUE(FuzzySearch::matches("chromium", "chr"));
    EXPECT_TRUE(FuzzySearch::matches("firefox", "fire"));
    EXPECT_TRUE(FuzzySearch::matches("process_name", "process"));
}

TEST_F(FuzzySearchTest, Matches_FuzzyMatch) {
    EXPECT_TRUE(FuzzySearch::matches("chromium", "chrom"));
    EXPECT_TRUE(FuzzySearch::matches("firefox", "firefox"));
    // Should match with some tolerance
    EXPECT_TRUE(FuzzySearch::matches("chromium", "chromiumm", 0.8));
}

TEST_F(FuzzySearchTest, Matches_CaseInsensitive) {
    EXPECT_TRUE(FuzzySearch::matches("ChRoMiUm", "chrome"));
    EXPECT_TRUE(FuzzySearch::matches("FIREFOX", "fire"));
}

TEST_F(FuzzySearchTest, Matches_NoMatch) {
    EXPECT_FALSE(FuzzySearch::matches("hello", "xyz"));
    EXPECT_FALSE(FuzzySearch::matches("process", "completely_different"));
}

TEST_F(FuzzySearchTest, GetMatchScore_SubstringAtStart) {
    double score1 = FuzzySearch::getMatchScore("chromium", "chr");
    double score2 = FuzzySearch::getMatchScore("chromium", "ium");
    EXPECT_GT(score1, score2); // Match at start should score higher
}

TEST_F(FuzzySearchTest, GetMatchScore_ExactMatch) {
    double score = FuzzySearch::getMatchScore("exact", "exact");
    EXPECT_GT(score, 1.0); // Exact match should score > 1.0
}

