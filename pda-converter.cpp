/*
@author: Oanh Le, Yen Le
*/

#include <cstdlib>
#include <iostream>
#include <math.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <charconv>
using namespace std;

map<char, set <string>> rule;
set<char> terminal;
set<char> nonTerminal;

/* function to check if the given grammar is in GNF */
bool GNFcheck()
{
  // form: X -> x or X ->xYZ...
  for(auto i: rule)
  {
    for(auto j : i.second)
    {
      int n = j.size();
      //if there is only one character, check if it's terminal or null
      if(n == 1)
      {
        if(!islower(j[0]))
        { 
          if(j[0] == '$') continue;
          return false;
        }
      }
      //if more than 1 character, check if the first one is terminal, and the following characters are non-terminals
      else if(n > 1)
      {
        if(!islower(j[0])) return false;
        for(int k = 1; k < n; k++)
        {
          if(!isupper(j[k])) return false;
        }
      }
    }
  }
  return true;
}

/* funtion to remove null or lambda */
void eliminateNull(){
  set<char> eliminate;

  int flag = 0;
  for (auto i : terminal){
    if (rule[i].find("$") != rule[i].end()){
      flag = 1;
      if (rule[i].size() == 1){
        eliminate.insert(i);
        for (auto j : terminal){
          //replace every terminal except the nullable terminal
          if (j!=i){
            set<string> temp;
            for(auto k : rule[j]){
              //strcmp(var1, "dev") == 0
              if (k.size() == 1 && k == to_string(i))
                k = "$";
              if (k.size() > 1 && k.find(i)!= string::npos){
                int ps = k.find(i);
                k.erase(ps, 1);
              }
              temp.insert(k);
            }
            rule[j] = temp;
          }
        }
      }
      else {
        rule[i].erase("$");
        for (auto j : terminal){
          set<string> temp;
          string s = "";
          for (auto k : rule[j]){
            if (k.size() > 1 && k.find(i) != string::npos){
              s = k;
              int ps = k.find(i);
              k.erase(ps,1);
            }
            temp.insert(k);
            if(s!=""){
              temp.insert(s);
              s= "";
            }
      
          }
          rule[j]= temp;
        }
      }
    }
  }

  for (auto e : eliminate){
    terminal.erase(e);
    rule.erase(e);
  }
  if (flag==1)
    eliminateNull();
}

/* funtion to remove unit production */
void unitProductions() {
	int flag;
	do {
		flag = 0;
		for (auto i : terminal) {
			set<string> temp;
			for (auto j : rule[i]) {
				if (j != to_string(i) && terminal.find(j[0])!=terminal.end()) {
					flag = 1;
					for (auto k : rule[j[0]])
						temp.insert(k);
				}
				else
					temp.insert(j);
			}
			rule[i] = temp;
		}
	} while (flag);
}

/* funtion to remove useless production */
void uselessProductions() {
	set <char> n1;
	for (auto n : terminal)
		for (auto p : rule[n]) {
			int i = 0;
			while (i < p.size() && terminal.find(p[i]) == terminal.end())
				i++;
			if (i == p.size()) {
				n1.insert(n);
				break;
			}
		}
	int ok;
	do {
		ok = 0;
		for (auto n : terminal)
			if (n1.find(n) == n1.end()) {
				for (auto p : rule[n]) {
					int i = 0;
					while (i < p.size() && terminal.find(p[i]) == terminal.end() ||n1.find(p[i]) != n1.end())
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
	vector <char> N2;
	set <char> n2;
	n2.insert('S');
	N2.push_back('S');
	int i = 0;
	while (i < N2.size()) {
		char n = N2[i];
		for (auto p : rule[n]) {
			for (int j = 0; j < p.size(); j++)
				if (terminal.find(p[j]) != terminal.end() && n2.find(p[j]) == n2.end()) {
					n2.insert(p[j]);
					N2.push_back(p[j]);
				}
		}
		i++;
	}

	//Find common element in n1
	set<char>::iterator it = n1.begin();
	while (it != n1.end()) {
		if (n2.find(*it) == n2.end()) {
			terminal.erase(*it);
			rule.erase(*it);
			it=n1.erase(it);

		}
		else{
			n2.erase(*it);
			it++;
		}
	}
	for (auto s : n2) {
		terminal.erase(s);
		rule.erase(s);
	}
		
	//Delete production that contains non-terminal that are not in terminal set
	for (auto n : terminal) {
		set<string> Pn;
		for (auto p : rule[n]) {
			int i = 0;
			while (i < p.size()) {
				if (p[i] != '$' && nonTerminal.find(p[i]) == nonTerminal.end() && terminal.find(p[i]) == terminal.end())
					break;
				i++;
			}
			if (i == p.size())
				Pn.insert(p);
		}
		rule[n] = Pn;
	}
}

/* function to convert to GNF */
void toGNF(){
  eliminateNull();
  unitProductions();
  uselessProductions();
  
}

/* function to convert to PDA */
void toPDA()
{
  cout << "(p0,$,z) -> {(p,Sz)}" << endl;

  for (auto i : terminal){
    map<char, vector<string>> pda;

    for ( string s:rule[i]){
      if (s.size() > 1)
        pda[s[0]].push_back(s.substr(1, s.size()-1));
      else
        pda[s[0]].push_back("$");
    }

    for (auto j : pda){
      cout << "(p," << j.first << "," << i << ") -> {";
      for (int k = 0; k < j.second.size(); k++){
        if (k == j.second.size() - 1)
          cout << "(p, " <<j.second[k] << ")}";
        else
          cout << "(p, " << j.second[k] << "),";
      }
      cout << endl;
    }

  }
  cout << "(p,$,z) -> {(p1,$)}" << endl;

}

/* helper functions */
bool equal(string A, string B)
{
  bool s=1;
  if(A.length()==B.length())
  {
    for(int i=0;i<A.length();i++)
    {
      if(A[i]!=B[i])
      {
        s=0;
        break;             
      }               
    }                                 
  } else s=0;
  return s;                             
}
bool inlist(string N,char m){
  bool t=0;
  for(int i=0;i<N.length();i++)
    if(N[i]==m)
      t=1;
  return t;
}

int main() {
  string input;
  cout << "\nCONVERT FROM CFG TO PDA\n\n";
  cout << "Instructions: " << endl; 
  cout << "(1) Use $ for null" << endl; 
  cout << "(2) Each rule is seperated by a comma. End the production rules with a dot"<< endl;
  cout << "(3) No spaces in between the input" << endl;
  cout << "(4) LHS contains only one uppercase letter" << endl;
  cout << "(*) Sample: S->abAB,A->aAB|$,B->b.\n\n";
  cout << "Please enter the production rules here: " << endl;
  cin >> input;
  bool check = 0;
  int g1 = input.length()-1;
  int g2 = input.length()-2;
  int num_LHS = 0;

  /* input validation */
  if(input[g1]!='.' || input[g2]==',')
  {                      
    check = 1;
    cout<< "Wrong input form" <<endl;                
  }                 
  else
  {
    for(int i = 0; i < g1 ; i++){
      if(check == 1) 
        break;      
      char a = input[i];

      //check LHS
      if(a=='#'||a=='.'||a=='-'||a=='>'||a=='('||a==')')
      {
        check=1;
        cout << "Wrong input form" << endl; 
        break;                
      }
      if(!isupper(a))
      {
        check=1;
        cout<<"Wrong input form"<<endl;
        break;
      }
      num_LHS++;
      // pair<int, char> RHS= {num_LHS, a};
      char RHS = a;

      //check ->    
      i++;
      a=input[i];
      if(a!='-')
      {
        check=1;
        cout<<"Wrong input form"<<endl;
        break;
      } 
      i++;
      a=input[i];
      if(a!='>')
      {
        check=1;
        cout<<"Wrong input form"<<endl;
        break;
      } 
      i++;

      //check RHS
      a=input[i];
      if(a=='#'||a=='.'||a=='-'||a=='>'||a=='('||a==')')
      {
        check=1;
        cout<<"Wrong input form"<<endl; 
        break;                
      }
      int start = i;
      int count = 0;
      int count_or = 0;
      while(a!=',')
      {
        if(a=='#'||a=='.'||a=='-'||a=='>'||a=='('||a==')')
        {
          check=1;
          cout<<"Wrong input form"<<endl;
          break;                
        }
 
        int d=i;
        int v=i;
        if((a=='|'&&input[d+1]=='|')||(a=='|'&&input[v-1]=='|')||(a=='|'&&input[v-1]==',')||(input[v-1]=='.')||(a=='|'&&input[v-1]=='>'))
        {
          check=1;
          cout<<"Wrong input form"<<endl; 
          break;                                                                                                                                                                                                                                                                  
        }
        if(a=='|')
        {
          count_or++;
          string str;
          if(count_or > 1)
          {
            str = input.substr (start, count-1);
          }
          else
            str = input.substr (start, count);
          rule[RHS].insert(str);
          count = 0;
          cout << str << endl;
          start = i+1;
        }
        if(a==',')
        {
          if(input[d+1]==','||((a==',')&&(i==g2)))
          {
            check=1;
            cout<<"Wrong input form"<<endl; 
            break;                  
          }               
        }
        if(a!= '|' || a!= ','|| a!= '.')
          count++;
        i++;
        a=input[i];
        if(a=='.'&&i==g1)
          break;
        if(check==1)
          break;           
      }
      string str;
      if(count_or == 0)
        str = input.substr (start, count);
      else
        str = input.substr (start, count-1);
      rule[RHS].insert(str);
      cout << str << endl;           
    }       
  }

  //debug the hashmap
  // for(auto i: rule)
  // {
  //   cout << i.first.first << ")" << i.first.second << ": ";
  //   for(auto j : i.second)
  //   {
  //     cout << j << ", ";
  //   }
  //   cout << endl;
  // }

    //test
    for (auto i : rule){
      cout << i.first << " -> ";
      for (auto s : i.second)
        cout << s << ", ";
      cout << endl;
    }

  // check if the input is missing any rule
  // ex. S->abAB,A->aAB|$. (missing rule B)
  string see1="";
  string sub1; 
  for(int i=0; i<input.length(); i++)
  {    
    i+=3;
    while(input[i]!=',' && input[i]!='.')
    {
      if(input[i]=='|')
        i++;                                 
      while(input[i]!=','&&input[i]!='|'&&input[i]!='.')
      {
        sub1+=input[i];
        i++;                                    
      }
      for(int j=0;j<sub1.length();j++)
      {     
        if(!inlist(see1,sub1[j])&&isupper(sub1[j]))
        see1+=sub1[j];     
      }
      sub1="";                                                                                                         
    }                                                
  }

  bool missing = 0;
  int z = see1.length();
  for(int i=0; i < input.length(); i++)
  {
    if(inlist(see1,input[i])) z--;                            
    while(input[i]!=',' && input[i]!='.') i++;                          
  }
  if(z>0)
  {
    cout<<"Oops! you're missing some rules"<<endl;        
    missing=1;         
  }       

  // check for A->A|$. infinite loop.
  bool x=0;
  bool n=0,b=0;
  string s;       
  string a; 
  string problem;    
  for(int i=0; i<input.length(); i++)
  {
    if(n==1&&b==1)
      break;       
    n=0,b=0;        
    a+=input[i];
    i+=3;
    while(input[i]!=','&&input[i]!='.')
    {
      if(input[i]=='|')
        i++;                   
      while(input[i]!='|'&&input[i]!=','&&input[i]!='.')
      {
        s+=input[i];
        i++;                                                  
      }
      if(equal(s,a))
      {
        n=1;
        problem+=s;
      }
      if(equal(s,"$"))
        b=1;
      if(n==1&&b==1)
        break;
      s="";
      a="";                                                       
    }            
  } 
  if(n==1&&b==1)
  {
    cout <<"Infinite loop for production " << problem << endl;
    x=1;
  }
  cout<<"*-----------------------*"<<endl;
  //insert terminal to set<char> terminal
  for (auto i : rule){
    terminal.insert(i.first);
  }
  //insert nonterminal
  for (auto i : terminal){
    for (auto j : rule[i]){
      for (auto k : j){
        if (islower(k) && nonTerminal.find(k)==nonTerminal.end())
          nonTerminal.insert(k);
      }
    }
  }
  //debug nonterminal
  cout << "\nNonTerminal\n";
  for (auto n: nonTerminal){
    cout << n << " ";
  }
  /* start converting */                         
  if(x==0 && z==0)
  {
    cout << "To Greibach normal form (GNF): ";

    // check if the given CFG is in GNF
    if(GNFcheck())
      cout << "the grammar is in GNF\n";
    else {
      cout << "the grammar is not in GNF\n";
      // convert to GNF
      toGNF();
    }


    cout<<"*-----------------------*"<<endl;
    // convert to PDA
    cout << "To Pushdown Automata (PDA): \n";
    toPDA();

    //test
    for (auto i : rule){
      cout << i.first << " -> ";
      for (string s : i.second)
        cout << s << ", ";
      cout << endl;
    }
    
  }
  return 0;
}
