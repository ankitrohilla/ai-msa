#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <algorithm>

#include <ctime>

class state;

using namespace std;

float ttime;

long c = 0 , d = 0;

//final cost
long minCost = 9999999999;

// for analysis
long statesProcessed = 0;
long statesEncountered = 0;

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

//    pointer to the previous state
    state *previousState;

//    we came to this state after giving this vector to findCost()
    vector<int> previousCostInput;

//    this is the cost of coming to this state from the previous state i.e. cost incurred by this state
    int costIncurred;

//    this is the cost so far to come to this state including this state's cost
    int costSoFar;

//    starting indices for each string
    vector<int> startingIndex;

//    this is to be fed to findCost to find costIncurred, this will include hyphens
    vector<int> currentConcern;

//    vector of strings so far encountered upto this state including hyphen
    vector<string> stringsSoFar;

//    number of hyphens added in this state
    int hyphens = 0;

    int findCost() {

        vector<int> s(currentConcern);

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

//    1st argument is the startingIndex of this state
//    2nd argument is the startingIndex of the previous state
//    3rd argument is the stringsSoFar of the previous state
//    4th argument is costSoFar of the previous state
    state( vector<int> startingIndex, vector<int> previousStartingIndex, vector<string> previousStringsSoFar, int previousCostSoFar ) {

        c++;
//        if( !(c % 1000000) )
//            cout << "Constructed " << c << endl;
        this->startingIndex = startingIndex;

        this->costIncurred = 0;

        for( int i = 0; i < stringNumber; i++ ) {
            string temp = previousStringsSoFar.at(i);

            if( startingIndex.at(i) > previousStartingIndex.at(i) ) {
                temp.push_back( (strings.at(i)).at(previousStartingIndex.at(i)) );
                currentConcern.push_back( (stringInts.at(i)).at(previousStartingIndex.at(i)) );
            } else {
                temp.push_back( '-' );
                currentConcern.push_back( vocabNumber );
                costIncurred += CC;
                hyphens++;
            }

            stringsSoFar.push_back( temp );

        }

//        find cost incurred after matching
        costIncurred += findCost();
        costSoFar = costIncurred + previousCostSoFar;
    }

    ~state() {
        d++;
//        if( !(d % 1000000) )
//            cout << "Destructed  " << d << endl;
        this->currentConcern.clear();
        this->previousCostInput.clear();
        this->startingIndex.clear();
        this->stringsSoFar.clear();
        this->tempInts.clear();
        this->tempStates.clear();
    }

//    only used by exploreStates() and getState()
    vector<state> tempStates;
    vector<int> tempInts;
    state *tempState;

    void getState( int i ) {

//        if the whole tempInts has been created and ready
        if( i == strings.size() ) {

            tempState = new state(tempInts, this->startingIndex, this->stringsSoFar, this->costSoFar);

//            if the new state can be better than the solution obtained yet, use it
            if( tempState->costSoFar < minCost ) {
//                for_each( tempState->startingIndex.begin(), tempState->startingIndex.end(), [](int i) {cout<<i;} );
//                cout << " ";
                tempStates.push_back(*tempState);
            }
            else {
//                for_each( tempState->startingIndex.begin(), tempState->startingIndex.end(), [](int i) {cout<<i;} );
//                cout << "(ignored[" << tempState->costSoFar << "]) ";
                delete tempState;
            }
            return;
        }
//        i is not referring to the last element

        if( strings.at(i).size() > startingIndex.at(i) ) {
            tempInts.push_back( startingIndex.at(i)+1 );
            getState( i+1 );
            tempInts.erase( tempInts.begin()+i, tempInts.end() );
        }

        tempInts.push_back( startingIndex.at(i) );

        getState( i+1 );

    }

//    will return a vector of all next states
    vector<state> exploreStates() {
        tempStates.clear();
//        cout << "\nminCost - " << minCost;
//        cout << "\nAdding to stack the states - ";
        getState( 0 );

//        to remove the last entry which indicates inserting hyphens for each string which is completely useless
//        remove the last entry only if the number of hyphens there are equal to number of strings
        if( tempStates.size()>1 && tempStates.back().hyphens == stringNumber )
            tempStates.pop_back();
        return tempStates;
    }

    bool isGoal() {

//        if the startingIndex of any string in the vector startingIndex is not its end
//        then this state is not a goal state
        for( int i = 0; i < stringNumber; i++ ) {
            if( startingIndex.at(i) < strings.at(i).size() )
                return false;
        }
        return true;
    }

    void viewStateInfo() {
        cout << "\n\nStarting index of each string - ";
        for_each( startingIndex.begin(), startingIndex.end(), [](int i){cout << i;} );
        cout << "\nCost to come to this state from the previous state - " << costIncurred;
        cout << "\nTotal cost                                         - " << costSoFar;
        cout << "\nString upto this state including this state - \n";
        for_each( stringsSoFar.begin(), stringsSoFar.end(), [](string s){cout << s << endl;} );

    }

}currentState, minState;


// input is the set of indices of vocab which represents character of string

main() {

    time_t beg = clock();

    stack<state> pendingStates;

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
        currentState.stringsSoFar.push_back( *(new string()) );
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





//    STARTING AI
    pendingStates.push( currentState );


//    DFS WORKED A BIT BETTER THAN BFS BECAUSE ONE GOAL IS REACHED INSTANTLY

//    do it till the pendingStates is not empty
    do {

//        cout << "Stack size is " << pendingStates.size() << endl;
        currentState = pendingStates.top();
        pendingStates.pop();


        statesEncountered++;

//        currentState.viewStateInfo();

//        goal solution so far is better than any way this state can lead
        if( currentState.costSoFar >= minCost )
            goto afterProcess;

//        currentState.viewStateInfo();

        if( !currentState.isGoal() ) {
            vector<state> temp = currentState.exploreStates();

//            enstack the highest cost element first so that it will go deeper into the stack
            sort( temp.begin(), temp.end(), [&](state s1, state s2){
                    if( s1.costSoFar > s2.costSoFar )
                        return true;
                    else
                        return false;
                  });

            if( temp.size() > 1 && temp.back().costIncurred == 0 ) {
                pendingStates.push(temp.back());
                temp.clear();
            } else {
                for_each( temp.begin(), temp.end(), [&](state s){
                    pendingStates.push(s); }
                );
            }
        } else {
            if( currentState.costSoFar < minCost ) {
                minCost = currentState.costSoFar;
                minState = currentState;
                cout << "\nGoal reached\n";
                currentState.viewStateInfo();
            }
        }
        statesProcessed++;

        afterProcess:;

    } while( !pendingStates.empty() );

    cout << "\n\n\n\n\n\nAnd the winner is - \n\n";

    minState.viewStateInfo();

    time_t end = clock();

    cout << "\n\nTime taken is - " << float(end - beg)/CLOCKS_PER_SEC;
    cout << "\nStates processed - " << statesProcessed;
    cout << "\nStates encountered - " << statesEncountered;

    cout << endl;
    return 0;
}
