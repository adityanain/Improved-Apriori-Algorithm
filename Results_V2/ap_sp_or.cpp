#include "common.h"

using namespace std;

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
	vector<vector<int>> FkPruned; // Contains the Fk pruned before generation of next level candidate itemsets
	
	set< vector<int>> HashSet; // Set that will contain the frequent itemset, used to filter out the impossible candidates. 
	vector<vector<int>> frequentItemSets; // Contains all the frequent item-sets
	vector<int> itemsetsCount; // Contains the frequent item-sets count
	vector<int> prefixlen;
	vector<vector<int>> potCanItemSets; // potential candidate itemsets
	
	set<int> itemsSet;
	map<int,int> C1;
	//map<int,int>::iterator C1Iter;
	
	//Compressed Row Storage format for Sparse Matrix
	vector<int> rowVec; // Length of the Row Vector will be (no. of transactions + 1)
	vector<short> colVec; // Length of the column vector will be on an average (avg_length_of_transaction)*(no_of_transactions)
	rowVec.push_back(0); // Always starts from 0.
	
	createC1(inpFileName, rowVec, colVec, noOfTrans, itemsSet, C1 );
	
	minsup *= noOfTrans;
	minsup = (int)ceil(((double)minsup)/ ((double)100));
	cout<<"minsup : "<<minsup<<endl;	
	cout<<"Row Vector memory consumption : "<<(double)(sizeof(int)*(rowVec.size()))/(1024*1024)<<endl;
	cout<<"Column Vector memory consumption : "<<(double)(sizeof(short)*(colVec.size()))/(1024*1024)<<endl;
	
	// //Print Row Vector and Column Vector
	printVec<int>("Printing Row Vector", rowVec);
	printVec<short>("Printing Column Vector", colVec);
	
	// cout << noOfTrans<< endl;
	// cout << "Printing C1"<<endl;
	// printMap(C1);

	// generate F1
	createF1(C1, FkPruned, frequentItemSets, itemsetsCount, HashSet, minsup);
		
	printVecOfVec<int>("Printing F1", FkPruned);
	
	// Loop until no items in frequent set or candidate set 
	while(!FkPruned.empty()){
		// Generate Next level Candidate Itemsets Ck+1
		
		// Find the length of common prefix of consecutive itemsets
		findMatchingLen(FkPruned, prefixlen, k);
		
		// Generate Potential candidate Itemsets
		genPotCandidates(potCanItemSets, prefixlen, FkPruned, k);
			
		printVecOfVec<int>("Printing Potential Candidates ", potCanItemSets);
		
		// Generate Candidates from the potential candidate itemsets by applying the downward closure property
		genCandidates(potCanItemSets, Ck, HashSet, k);
		
		printVecOfVec<int>("Printing Ck", Ck);
		
		// Generate Frequent Itemset from the Candidate Itemset
		clock_t time = clock();
		genFrequentItemSets(Ck, Fk, HashSet, rowVec, colVec, frequentItemSets, itemsetsCount, noOfTrans, minsup);
	
		cout <<"Time taken for finding frequent k : "<<k+1<<" "<< (clock()-time)/double(CLOCKS_PER_SEC)<< endl;
		
		if(Fk.empty()) break;
			
		printVecOfVec<int>("Printing Fk", Fk);

		
		//Before Generating Candidate Item-sets from current level frequent item-sets, 
		//we will further prune the frequent item-sets.
		// Items with count less than k cannot be part of  next Candidate ItemSet.
		clock_t prtime = clock();
		pruneFk( Fk, FkPruned, k );
		cout <<"Time taken for pruning frequent k : "<<k+1<<" "<< (clock()-prtime)/double(CLOCKS_PER_SEC)<< endl;

		if(FkPruned.empty()) break;

		printVecOfVec<int>("Printing FkPruned", FkPruned);
		
		++k;
	}
	
	cout <<"Time taken : "<< (clock()-start)/double(CLOCKS_PER_SEC)<< endl;
	// Apriori Ended
	// Printing the frequent Item-sets and their support value
	cout<<"The Frequent ItemSets with their Support count : "<<endl; 
	int i = 0;
	for(auto & fqItemSet : frequentItemSets){
		for(int & item : fqItemSet)
		cout<<item<<" ";
		cout<<": "<<itemsetsCount[i++]<<endl;
	}
	
	return 0;
}
