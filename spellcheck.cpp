/*
Jordan Francia
CSCI335
Fall 2013

Spellcheck: Loads a dictionary from a text file provided at the command line, uses it to spellcheck a text file provided at the command line. 
Additional option to run a load-text experiment on a hash table using linear probing. 

To run: ./spellcheck <textfile> <dictionaryfile>

*/

#include "Dictionary.h"
#include "QuadraticProbing.h"
#include <string> 
#include <vector>  
#include <fstream>
#include <stdlib.h> 
#include <algorithm> 
#include <math.h>
#include <iomanip>

using std::cout; 

vector<string> getMisWords(string textFile, Dictionary & dict);   //Returns a list of words from textFile that are not in the dictionary 'dict'
void spellcheck(string textfile, Dictionary & dict);   		//Spell-checks a document 'textfile' using a dictionary 'dict'
void compareIntProbes();   			       		//Runs a load test using linear probing and random ints as elements 
float * getAvgIntProbes(float input);				//Gets the average # of linear probes used to insert N random ints into a table, where N = input*table_size

int main(int argc, char *argv[]){
    bool running = true,
	 testMode = false;    
    string dFile,
           tFile; 

    //Command Line Arguments
    if(!testMode){
	if(argc != 3){
		cout << "\nTo use this program, type ./spellcheck <document file> <dictionary file>" << "\n";
		return 1;
	}
	tFile = argv[1];
	dFile = argv[2];
	std::ifstream texttest(tFile.c_str());
	std::ifstream dicttest(dFile.c_str());
	if(!texttest.is_open()){
		cout << "\nERROR: file \"" << tFile << "\" not found\n";
		return 1;
	}		
	if(!dicttest.is_open()){
		cout << "\nERROR: file \"" << dFile << "\" not found\n";
		return 1;
	}
	texttest.close();
	dicttest.close(); 
    } else {
    //test args
    	   dFile = "wordsEn.txt",
           tFile = "turing.txt";
    }

    cout << "\n\n";
    cout << "Assignment 3: Hash Tables\n";
    cout << "-------------------------\n";
    cout << "Coded by Jordan Francia\n\n";

    Dictionary * dict = NULL;  
    srand (time(NULL));
    
    int input = 0;
    while(running){				//main loop
	cout << "Choose Function:\n\n";
    cout << "1: Spell check " << tFile << " using dictionary from " << dFile << "\n";
	cout << "2: Run load test using linear probing\n";
	cout << "3: Quit\n";
	while(!input){
		cout << "\nEnter 1, 2 or 3: ";
		cin >> input;
		if(input < 1 || input > 3){
		    cout << "\n\n" << "Invalid selection. Please try again." << "\n\n";
		    input = 0;
		    cin.clear();
		    cin.ignore(1000, '\n');
		} else {	
			cout << "\n\n";
			switch(input){
				case 1:
					if(dict == NULL)
						dict = new Dictionary(dFile, 0, true); 
		    			spellcheck(tFile, *dict);
					break;
				case 2: 
					compareIntProbes();
					break;
				case 3: 
					running = false;
					break;
			}
		}
	}
	input = 0;
	cin.clear();
	cin.ignore(1000, '\n');
	cout << "\n\n";
    }

    if(dict != NULL)
	delete dict;
    
    cout << "Goodbye!\n\n";
    
    return 0;    
}

void compareIntProbes(){
    vector<int> inputs;
    int tableSize = 1009,
	rando;
    float upperLoad = 1,
	  lowerLoad = .1,
	  result[2],
	  thisSize; 
    HashTable<int> table(tableSize, "LINEAR", false),
		   inputCheck(tableSize);

   for(int n = 0; n < floor(2*tableSize*upperLoad); n++){	//Create list of 2N unique random numbers. Use both a vector and hash table to store numbers - vector to access element indexes, hash table to ensure unique numbers added
	rando = rand()%(tableSize*10);
	if(!inputCheck.contains(rando)){
		inputCheck.insert(rando);
		inputs.push_back(rando);
	}
	else
		n--;
   }

    cout << "Linear Probe Load Test -- Table Size: 1009" << "\n";
    cout << "------------------------------------------\n";
    cout << "Load\t    Inserted\t    Avg. Probes (found)     Avg. Probes (Not Found)\n";

    for(float load = lowerLoad; load <= upperLoad; load += .05){  //For each load...
       result[0] = 0;
       result[1] = 0;
       thisSize = floor(load*tableSize);
       for(int n = 0; n < thisSize; n++)        //Load M elements
           table.insert(inputs[n]);
       
       for(int n = 0; n < thisSize; n++)		 //Search for M known elements, record probes
	   result[0] += static_cast<float>(table.getSearchProbes(inputs[n]));  
       
       for(int n = 0; n < thisSize; n++)            //Search for M unknown elements, record probes
	   result[1] += static_cast<float>(table.getSearchProbes(inputs[inputs.size()-n-1]));
       
       result[0] /= thisSize;			//Calculate averages
       result[1] /= thisSize;
    
       cout << load << "\t...\t" << thisSize << "\t...\t";
       cout << std::setprecision(4) << result[0] << "\t...\t" << result[1] << "\n";
    }
    cout << "\n\n";    
}

void spellcheck(string textfile, Dictionary & dict){
    int mcSize;
    vector<string> misWords, misCands;
    misWords = getMisWords(textfile, dict); 

    cout << "Spellcheck for " << textfile << ":\n\n";
    for(int n = 0; n < misWords.size(); n++){
    	misCands = dict.getCandidates(misWords[n]);
    	mcSize = misCands.size();
    	cout << "Unrecognized word: " << misWords[n] << "\n";
    	cout << "Candidates: ";	
    	if(!mcSize)
    		cout << "(NONE)";
    	else{
    		for(int k = 0; k < mcSize; k++){
    			cout << misCands[k];
    			if(k < mcSize - 1)
    				cout << ", ";
    		}
    	}
    	cout << "\n\n";
    }        

}

vector<string> getMisWords(string textFile, Dictionary & dict){
	vector<string> result;
	string thisWord;
	std::ifstream istream(textFile.c_str());

	if(!istream.is_open())
		cout << "\nERROR: file \"" << textFile << "\" not found\n";
	else{ 
		while(!istream.eof()){ 		
			istream >> thisWord;	    		
						
			dict.format(thisWord);
			if(thisWord != ""){
				if(!dict.contains(thisWord)){
                    			if(std::find(result.begin(), result.end(), thisWord) == result.end())
						result.push_back(thisWord);
                		} 
			} 			 
		} 
	}
	istream.close();
	return result;
}
