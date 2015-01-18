/*
Jordan Francia
CSCI335
Fall 2013

Dictionary Class: uses a hash table to store words, can also check for misspellings. 

CONSTRUCTION: a dictionary of words constructed from dict_file, an optional max word capacity, and an optional flag to make the probe type linear, rather than quadratic  

******************PUBLIC OPERATIONS*********************
Dictionary(string dict_file, int max_size = 0, bool linear = false) --> Creates a dictionary from file dict_file, with no more than max_size words. Linear is flag to indicate linear probing, rather than quadratic
int contains(string word) 		  			    --> returns 0 if the word x does not exist, or otherwise the # of probes necessary to find said word. (If probe type is quadratic, returns 1).
vector<string> getCandidates(string word) 			    --> if word is not in the dictionary, returns a list of candidates for mis-spellings 
void empty() 				  			    --> empties dictionary
int addFile(string word, int max_size = 0)  			    --> adds a file of words to existing dictionary (up to max_size), returns number of probes if linear
int addWord(string word) 		  		    	    --> adds a word to the dictionary, returns number of probes if linear
int addWordList(vector<string> wordlist)  		    	    --> adds a vector of words to the dictionary, returns number of probes if linear
int getSize() 			          			    --> returns size
void format(string & word)					    --> formats a word to make it lower case and remove non-alpha values
        
*/

#ifndef Dictionary_H
#define Dictionary_H

#include "QuadraticProbing.h" 
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

class Dictionary{
    public:
        Dictionary(string dict_file, int max_size = 0, bool linear = false);
        int contains(string word);
        vector<string> getCandidates(string word);   
        void empty();
        int addFile(string word, int max_size = 0);
        int addWord(string word);
        int addWordList(vector<string> wordlist);
        int getSize();
        string getProbeType();
	void format(string & word);
        ~Dictionary(); 
        
    private:
        int size;
        bool dictLoaded,
             linearProbe;
        string dFile; 
        HashTable<string> * dict;
        
        int loadDict(string dict_file, int max_words = 0, bool concat = false);  
};

Dictionary::Dictionary(string dict_file, int max_size, bool linear) : size(0), dictLoaded(false), dFile(dict_file), linearProbe(linear){  
    if(linear)
        dict = new HashTable<string>(109,"LINEAR");
    else
        dict = new HashTable<string>();
    if(max_size < 0)
        max_size = 0;
    loadDict(dict_file, max_size);  
}

string Dictionary::getProbeType(){
    if(linearProbe)
        return "LINEAR";
    else 
        return "QUADRATIC";    
}

int Dictionary::contains(string word){
	return dict->contains(word);
}

int Dictionary::getSize(){
    return size;    
}

int Dictionary::loadDict(string dict_file, int max_words, bool concat){  
    int result = 0,
        totalWords = 0,
        probes;
    string thisLine;
    if(!concat)
        dict->makeEmpty();
    std::ifstream istream(dict_file.c_str());    
    if(!istream.is_open())
        cout << "\nERROR: file \"" << dict_file << "\" not found. Cannot load dictionary.\n";     
    else{
        while(!istream.eof()){
            getline(istream,thisLine);
	    format(thisLine);
            if(thisLine != ""){  
                probes = dict->insert(thisLine);
                if(probes != -1){ 
                    result += probes;
                    totalWords++;
                    if(max_words && totalWords == max_words)
                        break;
                }
            }    
        }
    }
    if(!concat)
        size = totalWords;
    else
        size += totalWords;
    if(!linearProbe)
        result = 1; 
    if(result != -1){
        dictLoaded = true;
        cout << "Dictionary successfully " << (concat ? "added" : "loaded") << " from " << dict_file << "\n";
        cout << size << " words added\n";
        if(linearProbe){
            cout << "Total probes required: " << result << "\n";
            cout << "Average probes required: " << (static_cast<float>(result)/static_cast<float>(totalWords)) << "\n";
        }            
        cout << "\n\n"; 
    }    
    return result;
}

void Dictionary::format(string & word){
	bool done = false;
	for(int n = 0; n < word.size(); n++)
		if(isupper(word[n]))
			word[n] = tolower(word[n]);
	while(!done){
		done = true;
		for(int n = 0; n < word.size(); n++){
			if(!isalpha(word[n])){
				word.erase(n,1);
				done = false;
			}
		}	
	}	
}

vector<string> Dictionary::getCandidates(string word){
    vector<string> result,
                   allPerms; 
    string perm;
    if(dict->contains(word))
        result.push_back(word);
    else{
        for(int n = 0; n < word.length()+1; n++){ //insert each letter of alphabet to every position of word
            for(char ch = 'a'; ch < 'z'; ch++){
                perm = word; 
                perm.insert(n,1,ch);   
                allPerms.push_back(perm);
            }
        }
        for(int n = 0; n < word.length(); n++){ //remove one of each letter
            perm = word; 
            perm.erase(n,1);   
            allPerms.push_back(perm);
         }
        for(int n = 0; n < (word.length()-1); n++){ // swap all adjacent letters
            perm = word;    
            std::swap(perm[n],perm[n+1]);
            allPerms.push_back(perm);
        }
        for(int n = 0; n < allPerms.size(); n++){
            if(dict->contains(allPerms[n]))
                result.push_back(allPerms[n]);
        } 
    }
    return result;  
}

void Dictionary::empty(){
    dict->makeEmpty();    
    dictLoaded = false;
    size = 0;
}

int Dictionary::addFile(string filename, int max_size){
    return loadDict(filename,max_size,true);    
}

int Dictionary::addWord(string word){
    int result = dict->insert(word);        
    if(result)
        size++;
    return result;
}

int Dictionary::addWordList(vector<string> wordlist){
    int probes;
    int result = 0;
    for(int n = 0; n < wordlist.size(); n++){
        probes = dict->insert(wordlist[n]);
        if(probes){
            size++;
            result += dict->insert(wordlist[n]);
        }
    }
    return result;    
}

Dictionary::~Dictionary(){
    delete dict;    
}

#endif
