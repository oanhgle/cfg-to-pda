#ifndef __HEADER_H
#define __HEADER_H

#include <map>
#include <iostream>
#include <set>

std::map<char, std::set<std::string>> rule;
std::set<char> terminal; //A B C D
std::set<char> nonTerminal; //a b c d

#endif
