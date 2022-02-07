//
// Created by Pablo Arevalo on 07/02/2022.
//
#include "Lexer.h"
#include<iostream>


Lexer::Lexer(std::string tableFile, std::string tokenFile, std::string finalStateFile) {
    //Reading transition table
    file.open(tableFile,std::ios::in);
    if (!file) {
        std::cout << "No such file table";
    }
    std::string value;
    value += file.get();value += file.get();
    int numOfRows = std::stoi(value);
    numStates = numOfRows;
    value.clear();
    value += file.get();value += file.get();value += file.get();
    int numOfCol = std::stoi(value);
    finalState = numOfCol-1;
    table = new int*[numOfRows];
    for(int i = 0; i < numOfRows; i++){
        table[i] = new int[numOfCol];
    }
    char c;
    file.get();
    value.clear();
    int col=0, row=0;
    while(!file.eof()){
        c = file.get();
        if(c != '\t'  && c != '\n'){
            value += c;
        }
        else{
            if(c == '\n'){
                row += 1;
                col = -1;
            }
            if(!value.empty()){
                table[row][col] = stoi(value);
                value.clear();
            }
            col += 1;
        }
    }
    file.close();

    //Reading Tokens
    int numTokens = numOfRows;
    file.open(tokenFile,std::ios::in);
    if (!file) {
        std::cout << "No such file token";
    }
    value.clear();
    int index = 0;
    while(!file.eof()){
        c = file.get();
        if(c != '\t'){
            value += c;
        }
        else{
            tokens.insert({value,index});
            index++;
            value.clear();
        }
    }
    file.close();


    //Reading final state
    //Hashtable storing the final state names
    file.open(finalStateFile,std::ios::in);
    if (!file) {
        std::cout << "No such file final state";
    }
    int stateValue = -1;
    std::string input; input.clear();
    while(!file.eof()){
        c = file.get();
        if(c != '\t'){
            if(c == '\n'){
                finalStates.insert({stateValue, input});
                input.clear();
            }
            else{input.append(std::string{c});}
        }
        else{
            if(!input.empty()){
                if(isInteger(input)){
                    stateValue = std::stoi(input);
                    input.clear();
                }
            }
        }
    }
    file.close();

    //Initializing exception table for reserved words
    exceptionTable = {
            {2,{"if"}},
            {3, {"var","let"}},
            {4,{"read","then","else","func","self","void","impl"}},
            {5,{"write","float","while"}},
            {6,{"public","return","struct"}},
            {7,{"private","integer"}},
            {8,{"inherits"}}
    };
}

Lexer::~Lexer() {
    for(int i = 0; i < numStates; i++){
        delete table[i];
    }
    delete table;
}

bool Lexer::isInteger(std::string s) {
    for(char c: s){
        if(!isdigit(c)){
            return false;
        }
    }
    return true;
}

bool Lexer::isFinal(int state) {
    if(table[state][finalState] == 1){
        return true;
    }
    return false;
}

char Lexer::nextChar(int state) {
    char c = file.get();
    if(c == '\r'){
        c = file.get();
    }

    if(c == '\n'){
        inputString.append("\\n");
        lineCount++;
        return c;
    }
    inputString.append(1,c);

    if(inputString.length() > 1){
        if(inputString.substr( inputString.length() - 2 ).compare("/*") == 0){
            stack.push("/*");
        }
        if(inputString.substr( inputString.length() - 2 ).compare("*/") == 0){
            if(!stack.empty()){
                stack.pop();
            }
        }
    }

    //Exponential e instead of alphabet e
    if(c == 'e' && state == 5){
        return c;
    }

    if(c == '\t'){
        return ' ';
    }

    if(isalpha(c)){
        return 'l';
    }

    if(isdigit(c)){
        if(c != '0') {
            return 'n';
        }
    }

    return c;
}

std::string Lexer::error(char c) {
    std::string errorString; errorString.clear();
    errorString.append("Lexical error: Invalid character: ");
    errorString.append(std::string{c});
    errorString.append(": line ").append(std::to_string(lineCount));
    errorList.emplace_back(errorString);
    std::string character = std::string{c};
    return ("invalidchar " + character + " " + std::to_string(lineCount));

}

std::string Lexer::createToken(int state) {
    bool reserved = false;
    std::string token; token.clear();
    std::string name = finalStates.at(state);

    //Check if reserved word
    if(name.compare("id") == 0){
        int len = inputString.length();
        if(len > 1 && len < 9){ //MUST BE CHANGED IF LONGER KEYWORDS ARE ADDED
            for(auto& p: exceptionTable[len]){
                if(inputString.compare(p) == 0){
                    reserved = true;
                    break;
                }
            }
        }
    }

    //create token string
    token.append("[");
    if(reserved){
        token.append(inputString);
        token.append(", ");
        token.append(inputString);
    }
    else{
        token.append(name);
        token.append(", ");
        token.append(inputString);
    }
    token.append(", ");
    token.append(std::to_string(lineCount));
    token.append("]");
    return token;
}

std::string Lexer::nextToken() {
    int state = 0;
    std::string token;
    char lookup;
    int tokenIndex = 0;

    token.clear();
    inputString.clear();
    while(true){
        lookup = nextChar(state);
        try{
            tokenIndex = tokens.at(std::string{lookup});
        }
        catch (std::exception e){
            return error(lookup);
        }

        state = table[state][tokenIndex];
        if(state == -1){
            inputString.clear();
            if(!token.empty()){
                file.unget();
            }
            else if(!(lookup == ' ' || lookup == '\n')){
                return error(lookup);
            }
            break;
        }
        //If block comment, check stack for nested comment handling
        if(state == 17){
            if(!stack.empty()){state = 15;}
        }
        if(isFinal(state)){
            token = createToken(state);
        }
    }
    return token;
}

void Lexer::readFile(std::string fileIn) {
    file.open(fileIn,std::ios::in);
    if (!file) {
        std::cout << "No such file";
    }
    std::string token;
    while(!file.eof()){
        token = nextToken();
        if(!token.empty()){
            std::cout << token << std::endl;
        }
        if(file.eof()){
            if(!stack.empty()){
                std::string temp = "Lexical Error: Block Comment Unclosed";
                errorList.emplace_back(temp);
            }
        }
    }
    file.close();
}
