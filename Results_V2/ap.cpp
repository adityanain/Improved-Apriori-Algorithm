#include <map>
#include <vector>
#include <string>
#include <set>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;
typedef unsigned int u_int;

template<class T> inline void printVec(string display, vector<T>& mVec);
template<class T> inline void printVecOfVec(string display, vector<vector<T>>& mVecOfVec);
int readLineFromFile(vector<int>& dataLineVec, int& dataVecLen);
int& prefixLen(vector<int>& left, vector<int>& right, int& len);
void printMap(map<int,int>& mMap);
void createC1(char *inpFileName, int& noOfTrans, set<int>& itemsSet, map<int,int>& C1 );
void createF1(map<int,int>& C1, vector<vector<int>>& Fk, vector<vector<int>>& frequentItemSets, 
			  vector<int>& itemsetsCount, set<vector<int>>& HashSet, int& minsup);
void findMatchingLen(vector<vector<int>>& FkPruned, vector<int>& prefixlen, int& k);			
void genPotCandidates(vector<vector<int>>& potCanItemSets, vector<int>& prefixlen, vector<vector<int>>& FkPruned, int& k);
void genCandidates(vector<vector<int>>& potCanItemSets, vector<vector<int>>& Ck, set<vector<int>>& HashSet, int& k);
void genFrequentItemSets(char * inpFileName, vector<vector<int>>& Ck, vector<vector<int>>& Fk, 
						 set<vector<int>>& HashSet, vector<vector<int>>& frequentItemSets, 
vector<int>& itemsetsCount, int& noOfTrans, int& minsup);



ifstream inpFileStream;


//Assumption : The transactions in database are sorted in ascending order and
// a transaction doesn't contain repeated items; if not, then we need to sort the data while reading and remove duplicates
int main(int argc, char* argv[]){
	
	if(argc < 3){
		cout<<"Please enter the input in the following format : <datafile> <minsup percentage>"<<endl;
	}

	clock_t start = clock();
	cout <<"Starting time : "<< (start)/double(CLOCKS_PER_SEC)<< endl;
	
	char *inpFileName = argv[1];
	int minsup = atoi(argv[2]);

	int noOfTrans = 0;
	int k = 1; // Level indicator
	vector< vector<int> > Ck; // Candidate itemsets
	vector< vector<int> > Fk;  // Frequent itemsets;
	
	set< vector<int>> HashSet; // Set that will contain the frequent itemset, used to filter out the impossible candidates. 
	vector<vector<int>> frequentItemSets; // Contains all the frequent item-sets
	vector<int> itemsetsCount; // Contains the frequent item-sets count
	vector<int> prefixlen;
	vector<vector<int>> potCanItemSets; // potential candidate itemsets
	
	set<int> itemsSet;
	map<int,int> C1;

	// generate C1
	createC1(inpFileName, noOfTrans, itemsSet, C1);
	minsup *= noOfTrans;
	//cout<<"minsup : "<<minsup<<endl;
	minsup = (int)ceil(((double)minsup)/ ((double)100));
	cout<<"minsup : "<<minsup<<endl;
	
	//printMap(C1);

	// generate F1
	createF1(C1, Fk, frequentItemSets, itemsetsCount, HashSet, minsup);
	
	//printVecOfVec<int>("Printing F1", Fk);
	
	while(!Fk.empty()){
		
		// Generate Next level Candidate Itemsets Ck+1
		//cout << "Generating C"<<k+1<<" "<<"from F"<<k<<endl;

		findMatchingLen(Fk, prefixlen, k);
		
		genPotCandidates(potCanItemSets, prefixlen, Fk, k);
		
		//printVecOfVec<int>("Printing Potential Candidates ", potCanItemSets);
		
		genCandidates(potCanItemSets, Ck, HashSet, k);

		//printVecOfVec<int>("Printing Ck", Ck);
		
		// Generate Frequent Itemset from the Candidate Itemset
		clock_t time = clock();
		genFrequentItemSets(inpFileName, Ck, Fk, HashSet, frequentItemSets, itemsetsCount, noOfTrans, minsup);
		
		cout <<"Time taken for finding frequent k = "<<k<<" : "<< (clock()-time)/double(CLOCKS_PER_SEC)<< endl;
		
		//printVecOfVec<int>("Printing Fk", Fk);

		++k;
	}
	
	// Apriori Ended
	cout<<"Time taken : "<<((double)clock() - start)/(CLOCKS_PER_SEC)<<endl;
	
	// Printing the frequent Itemsets and their support value
	cout<<"The Frequent ItemSets along with their Support count : "<<endl; 
	int i = 0;
	for(auto & fqItemset : frequentItemSets){
		for(int & item : fqItemset)
		cout<<item<<" ";
		cout<<": "<<itemsetsCount[i++]<<endl;
	}
}

int readLineFromFile(vector<int>& dataLineVec, int& dataVecLen){
	if(inpFileStream.eof()) return 0;
	char c = inpFileStream.get();
	int no;
	dataVecLen = 0;
	dataLineVec.clear();
	while( c != '\n' && !inpFileStream.eof()){
		if( c == ' ')
		c = inpFileStream.get();
		no = 0;
		bool empty = true;
		while(c>='0' && c<='9'){
			empty = false;
			no *= 10;
			no += (c - '0');
			c = inpFileStream.get();
		}
		if(!empty){
			dataLineVec.push_back(no);
			++dataVecLen;
		}
	}
	return 1;
}

void createC1(char *inpFileName, int& noOfTrans, set<int>& itemsSet, map<int,int>& C1 ){

	inpFileStream.open(inpFileName);
	if(!inpFileStream.good()){
		inpFileStream.close();
		cout<<"While Generating C1"<<endl;
		cout<<"Error in opening datafile or datafile doesn't exists"<<endl;
		exit(1);
	}
	
	vector<int> dataLine;
	int dataLen;
	map<int,int>::iterator C1Iter;
	while(readLineFromFile(dataLine, dataLen)){
		if(dataLen)
		++noOfTrans;
		for(int i = 0; i < dataLen; ++i){
			itemsSet.insert(dataLine[i]);
			if((C1Iter = C1.find(dataLine[i])) != C1.end()){
				++((C1Iter)->second);
			}
			else
			++C1[dataLine[i]];
		}
	}
	inpFileStream.close();
}

void createF1(map<int,int>& C1, vector<vector<int>>& Fk, vector<vector<int>>& frequentItemSets, 
vector<int>& itemsetsCount, set<vector<int>>& HashSet, int& minsup){
	for(auto& itemSet : C1){
		if(itemSet.second >= minsup){
			// add it to the frequent Itemset
			vector<int> item;
			item.push_back(itemSet.first);
			frequentItemSets.push_back(item);
			itemsetsCount.push_back(itemSet.second);
			Fk.push_back(item);
			HashSet.insert(item);
		}
	}
}

int& prefixLen(vector<int>& left, vector<int>& right, int& len){
	int temp = 0;
	for(temp = 0; temp < left.size() && temp < right.size(); ++temp){
		if(left[temp] != right[temp]){
			len = temp;
			return len;
		}
	}
	len = temp;
	return len;
}

void findMatchingLen(vector<vector<int>>& Fk, vector<int>& prefixlen, int& k){
	prefixlen.clear();
	prefixlen.assign(Fk.size(),0);
	if(k > 1){
		int szFk = Fk.size();
		int lenMatched;
		for(int i = 1; i < szFk; ++i){
			lenMatched = 0;
			prefixLen(Fk[i-1], Fk[i], lenMatched);
			prefixlen[i] = lenMatched;
		}
	}
}

void genPotCandidates(vector<vector<int>>& potCanItemSets, vector<int>& prefixlen, vector<vector<int>>& Fk, int& k){
	potCanItemSets.clear();
	int szFk = Fk.size();
	for(int i = 0; i < szFk; ++i){
		for(int j = i+1; j < szFk; ++j){
			if(prefixlen[j] == k-1){
				vector<int> canItemset((Fk[i]).begin(),(Fk[i]).end());
				canItemset.push_back(Fk[j].back());
				potCanItemSets.push_back(canItemset);
			}
			else break;
		}
	}
}

void genCandidates(vector<vector<int>>& potCanItemSets, vector<vector<int>>& Ck, set<vector<int>>& HashSet, int& k){
	Ck.clear();
	for(auto& itemSet : potCanItemSets){
		bool isCand = true;
		if(k > 1){
			for(auto& item : itemSet){
				vector<int> checkItemSet;
				for(auto& mItem : itemSet){
					if(item != mItem){
						checkItemSet.push_back(mItem);
					}
				}
				if(!HashSet.count(checkItemSet)){
					isCand = false;
					break;
				}
			} 
		}
		if(isCand)
		Ck.push_back(itemSet);
	}
}

void genFrequentItemSets(char *inpFileName, vector<vector<int>>& Ck, vector<vector<int>>& Fk, 
set<vector<int>>& HashSet, vector<vector<int>>& frequentItemSets, 
vector<int>& itemsetsCount, int& noOfTrans, int& minsup){
	
	HashSet.clear();
	Fk.clear();
	vector<int> candidateItemSet;
	bool isFound = false;
	int j;
	vector<int> countVec(Ck.size(),0);
	//int start,end, fLoc;
	vector<int> dataLine;
	int dataLen;
	
	inpFileStream.open(inpFileName);
	if(!inpFileStream.good()){
		inpFileStream.close();
		cout<<"Error in opening datafile"<<endl;
		exit(1);
	}
	
	for(int i = 0; i < noOfTrans; ++i){
		if(readLineFromFile(dataLine, dataLen)){
			j = 0;
			for(auto& candidateItemSet : Ck){
				isFound = true;
				for(auto& item : candidateItemSet){
					if(!binary_search(dataLine.begin(), dataLine.end(), item)){
						isFound = false;
						break;
					}
				}
				if(isFound)
				++countVec[j];
				++j;
			}
		}
	}
	
	inpFileStream.close();
	j = 0;
	for(auto& kth : Ck){
		if(countVec[j] < minsup){ 
			++j; 
			continue;
		}
		Fk.push_back(kth);
		frequentItemSets.push_back(kth);
		itemsetsCount.push_back(countVec[j]);
		HashSet.insert(kth);
		++j;
	}
}

void printMap(map<int,int>& mMap){
	for(auto & i : mMap){
		cout<<i.first<<" : "<<i.second<<endl;
	}	
}

template<class T>
inline void printVec(string display, vector<T>& mVec){
	cout<<display<<endl;
	for(auto& i : mVec){
		cout<<i<<" ";
	}
	cout<<endl;
}

template<class T>
inline void printVecOfVec(string display, vector<vector<T>>& mVecOfVec){
	cout<<display<<endl;
	for(auto& tempVec : mVecOfVec){
		for(auto& i : tempVec)
		cout<<i<<" ";
		cout<<endl;
	}
}
