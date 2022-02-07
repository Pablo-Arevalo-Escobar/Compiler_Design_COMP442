//
// Created by Pablo Arevalo on 07/02/2022.
//
#pragma once
#include <string>
#include <list>
#include <stack>
#include <unordered_map>
#include <fstream>

class Lexer {
private:
    std::unordered_map<int,std::list<std::string>> exceptionTable;
    std::unordered_map<int,std::string> finalStates;
    std::unordered_map<std::string,int> tokens;
    std::list<std::string> errorList;
    std::stack<std::string> stack;
    std::string inputString;
    std::fstream file;
    int **table;
    int numStates = 0;
    int finalState;
    int lineCount = 1;
public:
    Lexer(std::string tableFile, std::string tokenFile, std::string finalStateFile);
    ~Lexer();
    //todo: deep copy constructor
    bool isInteger(std::string);
    bool isFinal(int state);
    char nextChar(int state);
    std::string error(char c);
    std::string createToken(int state);
    std::string nextToken();

    void readFile(std::string file);
};


