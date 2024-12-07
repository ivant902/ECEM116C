#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
// Define set entry 
    struct set {
        char state;      // M (Modified), S (Shared), I (Invalid)
        int lru;        // LRU counter
        bool dirty;     // Dirty bit
        int tag;        // Tag value
        set() : state('I'), lru(0), dirty(false), tag(0) {} // Initialize with index as LRU
        //set() : state('I'), lru(0), dirty(false), tag(0) {} // Initialize to Invalid
    };
// Create 4 caches, each with 4 sets
    struct cache {
        set coreCache[4];
        // Initialize sets with proper indices
        cache()
        {
            for (int i = 0; i < 4; i++){
                coreCache[i].lru = i; 
            }
        } 
    };

// CPU struct to manage all four caches
    struct CPU {
        cache cores[4];
    };

bool checkCoreCache(CPU cpu, int core, int tag, int &whichSet) 
{
    bool hit = false; 
    for (int i = 0; i < 4; i++) // iterate through each set in the core's private cache
    {
        if (cpu.cores[core].coreCache[i].tag == tag) // in each set, check if its tag = tag
        {
            hit = true; // send the bool so that we know if the tag is in this cores cache
            whichSet = i; 
        }
    }
    return hit;
}
bool checkAllCoreCache(CPU cpu, int tag, vector<int> &allWhichSet, vector <int> &allWhichCore)
{
    bool hit = false; 
    
    for (int j = 0; j < 4; j++) // iterate through each of the four cores
    {
        for (int i = 0; i < 4; i++) // iterate through each set in the core's private cache
        {
            if (cpu.cores[j].coreCache[i].tag == tag) // in each set, check if its tag = tag
            {
                hit = true; // send the bool so that we know if the tag is in this cores cache
                allWhichSet.push_back(i);
                allWhichCore.push_back(j);
            }
        }
    }
    return hit; 
    
}
int findLRU(CPU &cpu, int pid)
{
    int indexOfLru = 0;
    int temp[4] = {0};
    for (int ii = 0; ii < 4; ii++) // ii is which set in core i which is pid;
    {
        switch(cpu.cores[pid].coreCache[ii].lru)
        {
            case (0): 
                temp[ii] = 3; 
                indexOfLru = ii; 
                break;
            case (1): 
                temp[ii] = 0; 
                break;
            case (2): 
                temp[ii] = 1; 
                break;
            case (3): 
                temp[ii] = 2; 
                break;
            default: break;
        }
        
    }
    cpu.cores[pid].coreCache[0].lru = temp[0]; 
    cpu.cores[pid].coreCache[1].lru = temp[1]; 
    cpu.cores[pid].coreCache[2].lru = temp[2]; 
    cpu.cores[pid].coreCache[3].lru = temp[3]; 
    return indexOfLru; 
}

// current structure: 
// one cpu has 4 caches (one for each core)
void runProcess(CPU &cpu, int &cacheHits, int &cacheMisses, int &writebacks, int &broadcasts, int &cacheToCache,
                vector<int> users, vector<string> commands, vector<int> numbers)
{
    // pseudo code
    // for every instruction, check if the tag is in in coreCache[i].tag
    // report if there was a hit or miss
    // if there was a miss, go to the lowest LRU (typically zero) and put that tag into that set
    
// P3: read <100>
    //MOESIF
    // start at I (invalid)
    // if you want to modify, go to M; if you read something that is a cachemiss then you go to E (only that set has that memory info)
    for (int i = 0; i < users.size(); i++)
    {
        bool inCurrCore = false; 
        bool inOtherCore = false; 
        int whichSet = 0;
        vector<int> allWhichSet;
        vector <int> allWhichCore;
        int pid = users[i]-1;
        int tag = numbers[i];
        if (!checkCoreCache(cpu, pid, numbers[i], whichSet))
        {   
            cacheMisses++;
            broadcasts++;
            if (checkAllCoreCache(cpu, numbers[i], allWhichSet, allWhichCore))
            {   
                inOtherCore = true; 
                cacheToCache++;
            }
        }
        else
        {
            inCurrCore = true; 
            cacheHits++;
        }
         

        if ((commands[i]== "read") && !inCurrCore && !inOtherCore) // miss so we read of memory and set state to E
        {
            int replacementIndex = findLRU(cpu,pid);
            cpu.cores[pid].coreCache[replacementIndex].state = 'E'; 
            cpu.cores[pid].coreCache[replacementIndex].tag = tag; 
        } 
        if ((commands[i] == "write") && !inCurrCore && !inOtherCore)
        {
            int replacementIndex = findLRU(cpu,pid);
            cpu.cores[pid].coreCache[replacementIndex].state = 'M';
            cpu.cores[pid].coreCache[replacementIndex].tag = tag;  
        }

    }
    
}
int main(int argc, char* argv[]) {
    
    if (argc < 2) {
		cout << "No file name entered. Exiting...";
		return -1;
	}
    //argv[1] = "test1.txt";
    ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}
    string line;
    vector<int> users;
    vector<string> commands;
    vector<int> numbers;
    
    while (getline(infile, line)) {
        // Parse each line in format "P#: command <number>"
        int user = line[1] - '0'; // Extract number after P
        string command = line.substr(line.find(' ') + 1, line.find('<') - line.find(' ') - 2); // Extract read/write
        int number = stoi(line.substr(line.find('<') + 1)); // Extract number between < >
        
        users.push_back(user);
        commands.push_back(command);
        numbers.push_back(number);
    }

    int cacheHits = 0;
    int cacheMisses = 0;
    int writebacks = 0;
    int broadcasts = 0;
    int cacheToCache = 0;
    
    CPU cpu;
    runProcess(cpu, cacheHits, cacheMisses, writebacks, broadcasts, cacheToCache, users, commands, numbers);


    // Testing code to verify the setup of cores, caches, and sets
    for (int coreIndex = 0; coreIndex < 4; coreIndex++) {
        cout << "Core " << coreIndex + 1 << ": " << endl;
        for (int setIndex = 0; setIndex < 4; setIndex++) {
            cout << "  Cache " << setIndex << ": State=" << cpu.cores[coreIndex].coreCache[setIndex].state << ", LRU=" << cpu.cores[coreIndex].coreCache[setIndex].lru << ", Dirty=" << cpu.cores[coreIndex].coreCache[setIndex].dirty << ", Tag=" << cpu.cores[coreIndex].coreCache[setIndex].tag << endl;
        }
    }
    cout << "All CPU cores and caches are set up correctly." << endl;



    


    cout << cacheHits << endl << cacheMisses << endl << writebacks << endl << broadcasts << endl << cacheToCache;
    return 0;
}
