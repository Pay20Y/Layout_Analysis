# Layout_Analysis
### Introduction
This is an implementation of RLSA and X-Y Cut with OpenCV
### Dependencies
+ OpenCV 3.0+
### How to use 
Compile with g++ :
```
g++ -std=c++11 RLSA.cpp -o RLSA `pkg-config --libs --cflags opencv` -ldl
```
or
``` 
g++ -std=c++11 X_YCut.cpp -o X_YCut `pkg-config --libs --cflags opencv` -ldl
```
### Reference: 
+ Wong K Y, Casey R G, Wahl F M. Document analysis system[J]. Ibm Journal of Research & Development, 2011, 26(26):647-656.
+ Ha J, Haralick R M, Phillips I T. Recursive X-Y cut using bounding boxes of connected components[C]// International Conference on Document Analysis and Recognition. IEEE, 1995:952-955 vol.2.

