# cfg-to-pda
Implementation and visualization of procedure Context-Free Grammar (CFG) to Push-Down Automata (PDA).\
Step 1: Convert the given productions of CFG into GNF.\
Step 2: Convert the productions of step 1 to PDA.
---
How to run:
```
g++ -std=c++11 pda-converter.cpp
```
## Example
Take the test case
````
S -> abAB
A -> aAB | $
B -> b
````
as an example.\
The result turns out to be: The grammar is not in GNF.\
Converting to Greibach normal form (GNF):
````
A -> bAC | bC
B -> b
C -> a
S -> aSB | bAC | bC
````

Converting to Push-Down Automata (PDA):
````
(p0,$,z) -> {(p,Sz)}
(p,a,S) -> {(p, SB)}
(p,b,S) -> {(p, AC),(p, C)}
(p,b,A) -> {(p, AC),(p, C)}
(p,b,B) -> {(p, $)}
(p,a,C) -> {(p, $)}
(p,$,z) -> {(p1,$)}
````
