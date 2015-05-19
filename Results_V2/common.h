#include <map>
#include <vector>
#include <list>
#include <string>
#include <set>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>

//typedef unsigned int u_int;

template<typename U, typename T> inline bool binarySearch(const vector<U>& colVec, T start, T end, const T& item, T& fLoc);
template<typename T> inline void printVec(string display, vector<T>& mVec);
template<typename T> inline void printVecOfVec(string display, vector<vector<T>>& mVecOfVec);

int readLineFromFile(vector<int>& dataLineVec, int& dataVecLen);
int& prefixLen(vector<int>& left, vector<int>& right, int& len);
void printMap(map<int,int>& mMap);
int createC1(char *inpFileName, vector<int>& rowVec, vector<short>& colVec, int& noOfTrans, set<int>& itemsSet, map<int,int>& C1 );
void createF1(map<int,int>& C1, vector<vector<int>>& FkPruned, vector<vector<int>>& frequentItemSets, 
vector<int>& itemsetsCount, set<vector<int>>& HashSet, int& minsup);
void findMatchingLen(vector<vector<int>>& FkPruned, vector<int>& prefixlen, int& k);			
void genPotCandidates(vector<vector<int>>& potCanItemSets, vector<int>& prefixlen, vector<vector<int>>& FkPruned, int& k);
void genCandidates(vector<vector<int>>& potCanItemSets, vector<vector<int>>& Ck, set<vector<int>>& HashSet, int& k);
void genFrequentItemSets(vector<vector<int>>& Ck, vector<vector<int>>& Fk, 
set<vector<int>>& HashSet, vector<int>& rowVec, vector<short>& colVec, 
vector<vector<int>>& frequentItemSets, vector<int>& itemsetsCount,
int& noOfTrans, int& minsup);
void pruneFk(vector<vector<int>>& Fk, vector<vector<int>>& FkPruned, int& k);