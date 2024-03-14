#include "header.h"

#include <vector>
#include <string>

using std::string;
using std::set;
using std::to_string;
using std::vector;
using std::pair;


/* function to remove null or lambda */
void eliminateNull() {
    set<char> eliminate;

    int flag = 0;
    for (auto i: nonTerminal) {
        if (rule[i].find("$") != rule[i].end()) {
            flag = 1;
            if (rule[i].size() == 1) {
                eliminate.insert(i);
                for (auto j: nonTerminal) {
                    //replace every nonterminal except the nullable nonterminal
                    if (j != i) {
                        set<string> temp;
                        for (auto k: rule[j]) {
                            //strcmp(var1, "dev") == 0
                            if (k.size() == 1 && k == to_string(i))
                                k = "$";
                            if (k.size() > 1 && k.find(i) != string::npos) {
                                size_t ps = k.find(i);
                                k.erase(ps, 1);
                            }
                            temp.insert(k);
                        }
                        rule[j] = temp;
                    }
                }
            } else {
                rule[i].erase("$");
                for (auto j: nonTerminal) {
                    set <string> temp;
                    string s;
                    for (auto k: rule[j]) {
                        if (k.size() > 1 && k.find(i) != string::npos) {
                            s = k;
                            size_t ps = k.find(i);
                            k.erase(ps, 1);
                        }
                        temp.insert(k);
                        if (!s.empty()) {
                            temp.insert(s);
                            s = "";
                        }

                    }
                    rule[j] = temp;
                }
            }
        }
    }

    for (auto e: eliminate) {
        nonTerminal.erase(e);
        rule.erase(e);
    }
    if (flag == 1)
        eliminateNull();
}

/* function to remove unit production */
void unitProductions() {
    int flag;
    do {
        flag = 0;
        for (auto i: nonTerminal) {
            set <string> temp;
            for (auto j: rule[i]) {
                if (j != to_string(i) && nonTerminal.find(j[0]) != nonTerminal.end()) {
                    flag = 1;
                    for (const auto& k: rule[j[0]])
                        temp.insert(k);
                } else
                    temp.insert(j);
            }
            rule[i] = temp;
        }
    } while (flag);
}

/* function to remove useless production */
void uselessProductions() {
    set<char> n1;
    for (auto n: nonTerminal)
        for (auto p: rule[n]) {
            int i = 0;
            while (i < p.size() && nonTerminal.find(p[i]) == nonTerminal.end())
                i++;
            if (i == p.size()) {
                n1.insert(n);
                break;
            }
        }
    int ok;
    do {
        ok = 0;
        for (auto n: nonTerminal)
            if (n1.find(n) == n1.end()) {
                for (auto p: rule[n]) {
                    int i = 0;
                    while (i < p.size() && nonTerminal.find(p[i]) == nonTerminal.end() || n1.find(p[i]) != n1.end())
                        i++;
                    if (i == p.size()) {
                        n1.insert(n);
                        ok = 1;
                        break;
                    }
                }
            }
    } while (ok);

    //Find symbol that are accessible
    vector<char> N2;
    set<char> n2;
    n2.insert('S');
    N2.push_back('S');
    int i = 0;
    while (i < N2.size()) {
        char n = N2[i];
        for (const auto& p: rule[n]) {
            for (char j : p)
                if (nonTerminal.find(j) != nonTerminal.end() && n2.find(j) == n2.end()) {
                    n2.insert(j);
                    N2.push_back(j);
                }
        }
        i++;
    }

    //Find common element in n1
    auto it = n1.begin();
    while (it != n1.end()) {
        if (n2.find(*it) == n2.end()) {
            nonTerminal.erase(*it);
            rule.erase(*it);
            it = n1.erase(it);

        } else {
            n2.erase(*it);
            it++;
        }
    }
    for (auto s: n2) {
        nonTerminal.erase(s);
        rule.erase(s);
    }

    //Delete production that contains non-terminal that are not in terminal set
    for (auto n: nonTerminal) {
        set <string> Pn;
        for (auto p: rule[n]) {
            int j = 0;
            while (j < p.size()) {
                if (p[j] != '$' && terminal.find(p[j]) == terminal.end() && nonTerminal.find(p[j]) == nonTerminal.end())
                    break;
                j++;
            }
            if (j == p.size())
                Pn.insert(p);
        }
        rule[n] = Pn;
    }
}

/* function that makes all production have all non-terminals or 1 terminal
   for example: S->abBB (A->a, B->b) will become S->ABBB */
void allNonTerminal() {
    set <pair<char, char>> newAdd;
    for (auto n: nonTerminal) {
        set <string> Pn;

        for (auto p: rule[n]) {
            //if string length >= 2 and first letter p[0] is terminal
            if (p.size() >= 2) {

                for (char & i : p) {
                    if (terminal.find(i) != terminal.end()) {
                        int ok = 0;
                        char c = 'A';
                        for (auto s: newAdd) {
                            if (s.second == p[i]) {
                                c = s.first;
                                ok = 1;
                                break;
                            }
                        }

                        for (auto r: nonTerminal) {
                            string sc = string(1, p[i]);
                            if (rule[r].size() == 1 && rule[r].find(sc) != rule[r].end()) {
                                c = r;
                                ok = 1;
                                break;
                            }
                        }

                        if (ok == 1) {
                            i = c;
                        } else {
                            c++;
                            while (nonTerminal.find(c) != nonTerminal.end())
                                c++;
                            string sc = string(1, c);
                            rule[c].insert(string(1, i));
                            newAdd.insert({c, i});
                            i = c;
                            nonTerminal.insert(c);
                        }

                    }
                }
            }
            Pn.insert(p);
        }
        rule[n] = Pn;
    }

}

/* function that makes all-nonterminal-production have one terminal at the begining
   for ex. S -> BBBB (B->b) will become S -> bBBB
   */
void oneBeginTerminal() {
    for (auto n: nonTerminal) {
        set <string> temp;
        for (auto s: rule[n]) {

            if (s.size() >= 2) {
                char c = 'a';
                for (auto p: rule[s[0]]) {
                    c = p[0];
                }
                s[0] = c;
            }
            temp.insert(s);

        }
        rule[n] = temp;
    }
    uselessProductions();
}

/* function to convert to GNF */
void toGNF() {
    eliminateNull();
    unitProductions();
    uselessProductions();
    allNonTerminal();
    oneBeginTerminal();
}

