#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stack>
#include <unordered_map>
#include "Lexer.h"

int main() {
    Lexer parser = Lexer("../transitionTable.txt","../tokens.txt","../finalStates.txt");
    parser.readFile("/Users/pabloarevalo/CLionProjects/COMP442/nestTest.src");
    return 0;

}


