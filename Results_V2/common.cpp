#include "common.h"

using namespace std;

extern ifstream inpFileStream;
int readLineFromFile(vector<int>& dataLineVec, u_int& dataVecLen){
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

int createC1(char *inpFileName, vector<int>& rowVec, vector<short>& colVec, int& noOfTrans, set<int>& itemsSet, map<int,int>& C1 ){

	inpFileStream.open(inpFileName);
	if(!inpFileStream.good()){
		inpFileStream.close();
		cout<<"While Generating C1"<<endl;
		cout<<"Error in opening datafile or datafile doesn't exists"<<endl;
		return 1;
	}
	
	vector<int> dataLine;
	u_int dataLen;
	map<int,int>::iterator C1Iter;
	while(readLineFromFile(dataLine, dataLen)){
		if(dataLen){
			rowVec.push_back(dataLen + rowVec[noOfTrans]);
			++noOfTrans;
		}
		for(int i = 0; i < dataLen; ++i){
			colVec.push_back((short)dataLine[i]);
			itemsSet.insert(dataLine[i]);
			if((C1Iter = C1.find(dataLine[i])) != C1.end()){
				++((C1Iter)->second);
			}
			else
			++C1[dataLine[i]];
		}
	}	
	inpFileStream.close();
	
	return 0;
}

void createF1(map<int,int>& C1, vector<vector<int>>& FkPruned, vector<vector<int>>& frequentItemSets, 
vector<int>& itemsetsCount, set<vector<int>>& HashSet, int& minsup){
	for(auto& itemSet : C1){
		if(itemSet.second >= minsup){
			// add it to the frequent Itemset
			vector<int> item;
			item.push_back(itemSet.first);
			frequentItemSets.push_back(item);
			itemsetsCount.push_back(itemSet.second);
			FkPruned.push_back(item);
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

void findMatchingLen(vector<vector<int>>& FkPruned, vector<int>& prefixlen, int& k){
	prefixlen.clear();
	prefixlen.assign(FkPruned.size(),0);
	if(k > 1){
		int szFkPruned = FkPruned.size();
		int lenMatched;
		for(int i = 1; i < szFkPruned; ++i){
			lenMatched = 0;
			prefixLen(FkPruned[i-1], FkPruned[i], lenMatched);
			prefixlen[i] = lenMatched;
		}
	}
}

void genPotCandidates(vector<vector<int>>& potCanItemSets, vector<int>& prefixlen, vector<vector<int>>& FkPruned, int& k){
	potCanItemSets.clear();
	int FkPrunedSz = FkPruned.size();
	for(int i = 0; i < FkPrunedSz; ++i){
		for(int j = i+1; j < FkPrunedSz; ++j){
			if(prefixlen[j] == k-1){
				vector<int> canItemset((FkPruned[i]).begin(),(FkPruned[i]).end());
				canItemset.push_back(FkPruned[j].back());
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

void genFrequentItemSets(vector<vector<int>>& Ck, vector<vector<int>>& Fk, 
set<vector<int>>& HashSet, vector<int>& rowVec, vector<short>& colVec, 
vector<vector<int>>& frequentItemSets, vector<int>& itemsetsCount,
int& noOfTrans, int& minsup){
	
	HashSet.clear();
	Fk.clear();
	vector<int> candidateItemSet;
	int colIndex = 0;
	int transLen = 0;
	bool isFound = false;
	int j,count = 0;
	vector<int> countVec(Ck.size(),0);
	int start,end, fLoc;
	
	for(int i = 0; i < noOfTrans; ++i){
		colIndex = rowVec[i];
		transLen = rowVec[i+1] - rowVec[i];
		j = 0;	
		end = colIndex + transLen - 1;

		for(auto& candidateItemSet : Ck){
			start = colIndex;
			fLoc = 0;
			isFound = true;
			for(auto& item : candidateItemSet){
				if(!binarySearch<short,int>(colVec, start, end, item, fLoc )){
					isFound = false;
					break;
				}
				start = fLoc;
			}
			if(isFound){
				++countVec[j];
			}
			++j;
		}
	}
	
	j = 0;
	for(auto& itemSet : Ck){
		if(countVec[j] < minsup){ 
			++j; 
			continue;
		}
		Fk.push_back(itemSet);
		frequentItemSets.push_back(itemSet);
		itemsetsCount.push_back(countVec[j]);
		HashSet.insert(itemSet);
		++j;
	}				
}

void pruneFk(vector<vector<int>>& Fk, vector<vector<int>>& FkPruned, int& k){
	FkPruned.clear();
	unordered_map<int, int> itemsAndCount;
	unordered_map<int, int>::iterator itemsAndCountI;
	for(auto& itemSet : Fk){
		for(auto& item : itemSet){
			if((itemsAndCountI = itemsAndCount.find(item)) != itemsAndCount.end()){
				++(itemsAndCountI->second);
			}
			else{
				itemsAndCount.insert(make_pair(item,1));
			}
		}
	}
	
	for(auto& itemSet : Fk){
		bool add = true;
		for(auto& item : itemSet){
			if((itemsAndCountI = itemsAndCount.find(item)) != itemsAndCount.end()){
				if(itemsAndCountI->second < k){
					add = false;
					break;
				}
			}
		}
		if(add)
		FkPruned.push_back(itemSet);
	}
}

void printMap(map<int,int>& mMap){
	for(auto & i : mMap){
		cout<<i.first<<" : "<<i.second<<endl;
	}	
}

template<typename U, typename T>
inline bool binarySearch(const vector<U>& colVec, T start, T end, const T& item, T& fLoc){
	T mid;
	while(end >= start){	
		mid = start + (end - start)/2;
		if(colVec[mid] == item){
			fLoc = mid;
			return true;
		}
		if(colVec[mid] > item)
		end = mid-1;
		else
		start = mid+1;
	}		
	return false;
}
template<short, int> bool binarySearch(const vector<U>& colVec, T start, T end, const T& item, T& fLoc);
template<int, int> bool binarySearch(const vector<U>& colVec, T start, T end, const T& item, T& fLoc);
template<unsigned short, unsigned int> bool binarySearch(const vector<U>& colVec, T start, T end, const T& item, T& fLoc);
template<unsigned int, unsigned int> bool binarySearch(const vector<U>& colVec, T start, T end, const T& item, T& fLoc);

template<typename T>
inline void printVec(string display, vector<T>& mVec){
	cout<<display<<endl;
	for(auto& i : mVec){
		cout<<i<<" ";
	}
	cout<<endl;
}

template<typename T>
inline void printVecOfVec(string display, vector<vector<T>>& mVecOfVec){
	cout<<display<<endl;
	for(auto& tempVec : mVecOfVec){
		for(auto& i : tempVec)
			cout<<i<<" ";
		cout<<endl;
	}
}

