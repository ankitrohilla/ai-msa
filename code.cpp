#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

float ttime;

// number of characters
int vocabNumber;

// vector containing all the characters in the vocabulary
vector<char> vocab;

// number of strings
int stringNumber;

// vector of strings
vector<string> strings;

// each character in strings has been replaced by its index in vocab vector
vector< vector<int> > stringInts;

// CC*no. of dashes is the conversion cost
int CC;

vector< vector<int> > MC;

// this is the index in MC for hyphen
int hyphen;

typedef vector<int>::iterator intIterator;

// a state is composed of the starting index of each string
// goal states is having the last index for each string
// we have to reach the goal state with the minimum possible cost
class state {

public:

//    we came to this state after giving this vector to findCost()
    vector<int> previousCostInput;

//    this is the cost of coming to this state from the previous state i.e. cost incurred by this state
    int costIncurred;

//    this is the cost so far to come to this state including this state's cost
    int costSoFar;

//    starting indices for each string
    vector<int> startingIndex;

//    vector of strings so far encountered upto this state including hyphen
    vector<string> stringsSoFar;

    int findCost() {

        vector<int> s;
        for( int i = 0; i < stringNumber; i++ ) {
//            string number i with character given at ith index of startingIndex
            s.push_back( (stringInts.at( i )).at( startingIndex.at(i) ) );
        }

        int cost = 0;

        while( s.size() > 1 ) {
//            start it from the next element in the list
            for( intIterator it = s.begin()+1; it != s.end(); it++ ) {
                cost += (MC.at(*(s.begin()))).at(*it);
            }
            s.erase(s.begin());
        }

        return cost;

    }

    state() {}

    state( vector<int> i ) {
        startingIndex = i;
        costIncurred = findCost();
    }

//    only used by exploreStates() and getState()
    vector<state> tempStates;
    vector<int> tempInts;

    void getState( int i ) {

//        if the whole tempInts has been created and ready
        if( i == strings.size() ) {
            tempStates.push_back(*(new state(tempInts)));
            return;
        }
//        i is not referring to the last element
        tempInts.push_back( startingIndex.at(i)+1 );
        getState( i+1 );

        tempInts.erase( tempInts.begin()+i, tempInts.end() );

        tempInts.push_back( startingIndex.at(i) );

        getState( i+1 );

    }

//    will return a vector of all next states
    vector<state> exploreStates() {
        tempStates.clear();
        getState( 0 );

//        to remove the last entry which indicates inserting hyphens for each string which is completely useless
        tempStates.pop_back();
        return tempStates;
    }

    bool isGoal() {

//        if the startingIndex of any string in the vector startingIndex is not its end
//        then this state is not a goal state
        for( int i = 0; i < stringNumber; i++ ) {
            if( startingIndex.at(i) < strings.at(i).size()-1 )
                return false;
        }
    }

    void viewStateInfo() {
        cout << "\nStarting index of each string - ";
        for_each( startingIndex.begin(), startingIndex.end(), [](int i){cout << i;} );
        cout << "\nCost to come to this state from the previous state - " << costIncurred;
    }

}currentState;

// input is the set of indices of vocab which represents character of string

main() {

    currentState.costIncurred = 0;
    currentState.costSoFar = 0;

//    this vector will hold all the states that will be explored by the currentState
    vector<state> exploredStates;

    cin >> ttime >> vocabNumber;
    hyphen = vocabNumber;

    char temp;
    for( int i = 0; i < vocabNumber; i++ ) {
        cin >> temp;
        vocab.push_back(temp);
//        to remove commas
        cin >> temp;
    }

//    check file input format
    stringNumber = temp-48;

    for( int i = 0; i < stringNumber; i++ ) {
        string temp;
        cin >> temp;
        strings.push_back(temp);
        currentState.startingIndex.push_back(0);
    }

    cin >> CC;

    for( int i = 0; i <= vocabNumber; i++ ) {
        vector<int> temp1;
        for( int j = 0; j <= vocabNumber; j++ ) {
            int temp;
            cin >> temp;
            temp1.push_back(temp);
        }
        MC.push_back(temp1);
        temp1.clear();
    }
    
//    check if input received properly or not
    cout << "\nInput strings\n";
    for_each(strings.begin(), strings.end(), [](string s){cout<<s<<endl;});

    cout << "\nMC\n";
    for_each(MC.begin(), MC.end(), [](vector<int> v){
        for_each(v.begin(), v.end(), [](int i){cout<<i;});
        cout << endl;
    });

//    preprocessing
//    tells the index number in vocab corresponding to strings vector
//    string "ABC" will be converted to {3,1,2} if vocab vector says CAB
    for_each( strings.begin(), strings.end(), [&](string s) {
        vector<int> temp;
        for_each( s.begin(), s.end(), [&](char c) {
            for( int i = 0; i < vocabNumber; i++ ) {
                if( c == vocab.at(i) ) {
                    temp.push_back( i );
                    break;
                }
            }
        } );
        stringInts.push_back( temp );
        temp.clear();
    });

//    check if the values have been given properly or not
    cout << "\nstringInts\n";
    for_each(stringInts.begin(), stringInts.end(), [](vector<int> v){
        for_each(v.begin(), v.end(), [](int i){cout<<i;});
        cout << endl;
    });

//    this vector comprising of one character from each stringInts will be fed to findCost()
    vector<int> costInput;

    currentState.viewStateInfo();

    exploredStates = currentState.exploreStates();

    cout << endl;

    for_each( exploredStates.begin(), exploredStates.end(), [](state s){s.viewStateInfo();});


//    do it till the goal state is reached
//    while( !currentState.isGoal() ) {
//    }

    cout << endl;
    return 0;
}
