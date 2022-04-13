#pragma once
#include <string>

class SyntacticAnalyzer {
private:
public:
    void firstSet(std::string token);
    void followSet(std::string token);
    std::string findProductionRule(std::string token, std::string nonterminal);
    std::string findSentential(std::string token, std::string term);
};


