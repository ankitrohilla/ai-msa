#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <algorithm>

#include <ctime>

#define timeTaken float(endt - begt)/CLOCKS_PER_SEC

class state;

using namespace std;

float ttime;

time_t begt, endt;

long c = 0 , d = 0;

// name justifies
float mcAvg = 0;

//this factor decides the heuristic value
float divideFactor = 2.0;

// final cost
long minCost = 9999999999;

// they will be used by center star method where edit distance will return them as the output strings
// purely temporary
string s3, s4;

// for analysis
long statesProcessed = 0;
long statesEncountered = 0;
long nodeSaved = 0;

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


state* minState;

// CC*no. of dashes is the conversion cost
int CC;

vector< vector<int> > MC;

// true for ith string if that string is the largest
vector<bool> isMaxLength;

// this is the index in MC for hyphen
int hyphen;

typedef vector<int>::iterator intIterator;
typedef string::iterator stringIterator;
typedef vector<string>::iterator vecStringIterator;
typedef vector<state>::iterator stateIterator;


// finds the editDistance between the strings so far and returns the number of matches
// apply DP here
long editDistance( string s1, string s2, vector<int> v1, vector<int> v2, int depth ) {

    s3.clear();
    s4.clear();

//    if( depth == 1 )
//        cout << "Strings are " << s1 << " " << s2;

    int rows = s1.size()+1;
    int cols = s2.size()+1;

    int** A = new int*[rows];
    char** B = new char*[rows];
    for( int i = 0; i < rows; i++ ) {
        A[i] = new int[cols];
        B[i] = new char[cols];
    }

//    initialize

    A[0][0] = 0;
    B[0][0] = 'O';

    for( int i = 1; i < rows; i++ ) {
        A[i][0] = A[i-1][0] + CC + MC[v1.at(i-1)][vocabNumber];
        B[i][0] = '|';
    }
    for( int i = 1; i < cols; i++ ) {
        A[0][i] = A[0][i-1] + CC + MC[v2.at(i-1)][vocabNumber];
        B[0][i] = '-';
    }

//    do it
    for( int i = 1; i < rows; i++ ) {
        for( int j = 1; j < cols; j++ ) {
            int choice1, choice2, choice3;
            choice1 = A[i-1][j-1] + MC[v1[i-1]][  v2[j-1]   ]     ; // '\'
            choice2 = A[i-1][j]   + MC[v1[i-1]][vocabNumber] + CC; // '|'
            choice3 = A[i][j-1]   + MC[v2[j-1]][vocabNumber] + CC; // '-'

//            cout << "\ni j choice1,2,3 " << i << " " << j << " " << choice1 << " " << choice2 << " " << choice3;

            if( choice1 <= choice2 && choice1 <= choice3 ) {
                A[i][j] = choice1;
                B[i][j] = '\\';
            } else if( choice2 <= choice1 && choice2 <= choice3 ) {
                A[i][j] = choice2;
                B[i][j] = '|';
            } else {
                A[i][j] = choice3;
                B[i][j] = '-';
            }

        }
    }

//    for( int i = 0; i < rows; i++ ) {

//        cout << "\n";
//        for( int j = 0; j < cols; j++ ) {
//            cout << A[i][j] << " ";
//        }
//    }

//    for( int i = 0; i < rows; i++ ) {
//        cout << "\n";

//        for( int j = 0; j < cols; j++ ) {
//            cout << B[i][j] << " ";
//        }
//    }

    for( int i = rows-1, j = cols-1; (i > 0)||(j > 0);) {
        if( B[i][j] == '\\' ) {
            s3.push_back( s1[i-1] );
            s4.push_back( s2[j-1] );
            i--;
            j--;
        } else if( B[i][j] == '|' ) {
            s3.push_back( s1[i-1] );
            s4.push_back( '-' );
            i--;
        } else {
            s3.push_back( '-' );
            s4.push_back( s2[j-1] );
            j--;
        }
    }

    reverse( s3.begin(), s3.end() );
    reverse( s4.begin(), s4.end() );

    long result = A[rows-1][cols-1];

//    if( depth == 1 ) {
//        cout << " - values returned -> " << s3 << " " << s4;
//        cout << " " << result;
//    }

    for( int i = 0; i < rows; i++ ) {
        delete [] A[i];
        delete [] B[i];
    }
    delete [] A;
    delete [] B;

    s1.clear();
    s2.clear();

    return result;

}

vector<int> getStringInts( string s ) {
    vector<int> temp;
    for_each( s.begin(), s.end(), [&](char c) {
        for( int i = 0; i < vocabNumber; i++ ) {
            if( c == vocab.at(i) ) {
                temp.push_back( i );
                break;
            }
        }
        if( c == '-' )
            temp.push_back( vocabNumber );
    } );
    return temp;
}

// a state is composed of the starting index of each string
// goal states is having the last index for each string
// we have to reach the goal state with the minimum possible cost
class state {

public:

//    this will be 1 when largest string will introduced hyphen
    int extraHyphens = 0;

//    time when this state was discovered
    float discoveredAt;

//    depth of state in the tree
    int depth = 0;

//    estimated cost that will be incurred from this state to goal
    float heuristicMinCost = 0.0;

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

//    only used by Center Star Method
    vector<string> globalAlignment;

    int findCost(vector<int> s) {

        int cost = 0;

        while( s.size() > 1 ) {
//            start it from the next element in the list
            for( intIterator it = s.begin()+1; it != s.end(); it++ ) {
                cost += (MC.at(*(s.begin()))).at(*it);
            }
            if( *(s.begin()) == vocabNumber )
                cost += CC;
            s.erase(s.begin());
        }

        if( *(s.begin()) == vocabNumber )
            cost += CC;

        return cost;

    }

    state() {}

//    1st argument is the startingIndex of this state
//    2nd argument is the startingIndex of the previous state
//    3rd argument is the stringsSoFar of the previous state
//    4th argument is costSoFar of the previous state
//    5th argument is depth of the previous state
    state( vector<int> startingIndex, vector<int> previousStartingIndex, vector<string> previousStringsSoFar, int previousCostSoFar, int previousDepth ) {

        endt = clock();

        c++;
//        if( !(c % 1000000) )
//            cout << "Constructed " << c << endl;
        this->startingIndex = startingIndex;
        this->depth = previousDepth + 1;
        this->costIncurred = 0;

        this->discoveredAt = timeTaken;

        for( int i = 0; i < stringNumber; i++ ) {
            string temp = previousStringsSoFar.at(i);

            if( startingIndex.at(i) > previousStartingIndex.at(i) ) {
                temp.push_back( (strings.at(i)).at(previousStartingIndex.at(i)) );
                currentConcern.push_back( (stringInts.at(i)).at(previousStartingIndex.at(i)) );
            } else {
                temp.push_back( '-' );
                currentConcern.push_back( vocabNumber );
                hyphens++;
            }

            stringsSoFar.push_back( temp );
            temp.clear();
        }

//        find cost incurred after matching
        this->costIncurred = findCost( currentConcern );
        this->costSoFar = costIncurred + previousCostSoFar;
        if( depth == 1 ) {
//            viewStateInfo();
//            cout << "GONNA check heuristic\n";
        }

        heuristic();

//        if( depth == 1 ) {
//            cout << "Heuristic returned is " << this->heuristicMinCost;
//        }
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

        endt = clock();

        if( ttime - timeTaken < 20 )
            return;

//        if the whole tempInts has been created and ready
        if( i == strings.size() ) {

            tempState = new state(tempInts, this->startingIndex, this->stringsSoFar, this->costSoFar, this->depth);

//            if the new state can be better than the solution obtained yet, use it
            if( tempState->costSoFar + tempState->heuristicMinCost < minCost ) {
//                for_each( tempState->startingIndex.begin(), tempState->startingIndex.end(), [](int i) {cout<<i;} );
//                cout << " ";
                tempStates.push_back(*tempState);
            }
            else {
//                for_each( tempState->startingIndex.begin(), tempState->startingIndex.end(), [](int i) {cout<<i;} );
//                cout << "(ignored[" << tempState->costSoFar << "]) ";
            }

            delete tempState;

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

//    only used by center star method
    void addHyphen( int k ) {
        transform( globalAlignment.begin(), globalAlignment.end(), globalAlignment.begin(), [&](string s){
            s.insert( s.begin() + k, '-' );
            return s;
//            cout << s << endl;
        });
    }


// desirability of coming to this states is determined
    void heuristic() {

        heuristicMinCost = 0;

//        going to implement Center star approximation method
//        create a matrix of size nXn where n is the number of strings

        long *stringCostSum = new long[ stringNumber ];

        int **M = new int*[ stringNumber ];
        for( int i = 0; i < stringNumber; i++ ) {
            stringCostSum[i] = 0;
            M[i] = new int[ stringNumber ];
        }

//        (n^2)(k^2) time
//        for all nC2 combination of strings, find edit distance
        for( int i = 0; i < stringNumber; i++ ) {

            M[i][i] = 0;

            for( int j = i+1; j < stringNumber; j++ ) {
                stringIterator s1Start = strings.at(i).begin() + this->startingIndex.at(i);
                stringIterator s2Start = strings.at(j).begin() + this->startingIndex.at(j);
                stringIterator s1End = strings.at(i).end();
                stringIterator s2End = strings.at(j).end();

                intIterator i1Start = stringInts.at(i).begin() + this->startingIndex.at(i);
                intIterator i2Start = stringInts.at(j).begin() + this->startingIndex.at(j);
                intIterator i1End = stringInts.at(i).end();
                intIterator i2End = stringInts.at(j).end();

                string* s1 = new string( s1Start, s1End);
                string* s2 = new string( s2Start, s2End);
                vector<int> v1( i1Start, i1End);
                vector<int> v2( i2Start, i2End);

//                if( depth == 1 )
//                    cout << "\nStrings for editDistance " << *s1 << " " << *s2;


                float temp = editDistance( *s1, *s2, v1, v2, depth );

//                if( depth == 1 )
//                    cout << " " << s3 << " " << s4;

                M[i][j] = temp;
                M[j][i] = temp;
                stringCostSum[i] += temp;
                stringCostSum[j] += temp;

                delete s1;
                delete s2;
                v1.clear();
                v2.clear();
            }
        }

        long tempMinCost = 99999999;
        int tempMinIndex;

        for( int i = 0; i < stringNumber; i++ ) {
            if( tempMinCost > stringCostSum[i] ) {
                tempMinCost = stringCostSum[i];
                tempMinIndex = i;
            }
            delete [] M[i];
        }

//        if( depth == 1 )
//            cout << endl;

//        these will hold the aligned strings, Sc[i] being center string aligned with Si[i] string
//        if tempMinIndex is 3, Sc[3] and Si[3] will be useless
        vector<string> Sc;
        vector<string> Si;

//        find all the aligned strings aligned with the most similar string
        for( int i = 0; i < stringNumber; i++ ) {
            if( i != tempMinIndex ) {
                stringIterator s1Start = strings.at(tempMinIndex).begin() + this->startingIndex.at(tempMinIndex);
                stringIterator s2Start = strings.at(i).begin() + this->startingIndex.at(i);
                stringIterator s1End = strings.at(tempMinIndex).end();
                stringIterator s2End = strings.at(i).end();

                intIterator i1Start = stringInts.at(tempMinIndex).begin() + this->startingIndex.at(tempMinIndex);
                intIterator i2Start = stringInts.at(i).begin() + this->startingIndex.at(i);
                intIterator i1End = stringInts.at(tempMinIndex).end();
                intIterator i2End = stringInts.at(i).end();

                string* s1 = new string( s1Start, s1End);
                string* s2 = new string( s2Start, s2End);
                vector<int> v1( i1Start, i1End);
                vector<int> v2( i2Start, i2End);

                editDistance( *s1, *s2, v1, v2, depth );

//                now I have s3 and s4 with me which are the aligned

                Sc.push_back( s3 );
                Si.push_back( s4 );

                delete s1;
                delete s2;
                v1.clear();
                v2.clear();

            } else {
                Sc.push_back( "" );
                Si.push_back( "" );
            }
//            if( depth == 1 ) {
//                cout << Sc.at(i) << " " << Si.at(i) << endl;
//            }
        }

//        now I have Sc and Si, now I have to find the global alignment

        if( tempMinIndex != 0 ){
//            cout << "\nPushing Sc " << Sc[0] << endl;
            globalAlignment.push_back( Sc[0] );
        }else{
//            cout << "\nPushing Sc " << Sc[1] << endl;
            globalAlignment.push_back( Sc[1] );
        }

        for( int i = 0; i < stringNumber; i++ ) {
            if( tempMinIndex != i ) {
                int max = ((globalAlignment[0]).size() > (Sc[i]).size() ? (globalAlignment[0]).size() : (Sc[i]).size());
                int j = 0;
                while( j != max ) {

                     max = ((globalAlignment[0]).size() > (Sc[i]).size() ? (globalAlignment[0]).size() : (Sc[i]).size());

                    if( j >= (globalAlignment[0]).size() ) {
//                        cout << "1.Adding hyphen\n";
                        addHyphen( j );
                    } else if( j >= (Sc[i]).size() ) {
                        (Sc[i]).insert( Sc[i].begin() + j, '-' );
                        (Si[i]).insert( Si[i].begin() + j, '-' );
                    }

                    if( (globalAlignment[0]).at(j) != (Sc[i]).at(j) ) {
                        if( (globalAlignment[0]).at(j) == '-' ) {
                            (Sc[i]).insert( Sc[i].begin() + j, '-' );
                            (Si[i]).insert( Si[i].begin() + j, '-' );
                        } else if( (Sc[i]).at(j) == '-' ) {
//                            cout << "2.Adding hyphen\n";
                            addHyphen( j );
                        }
                    }
                    j++;
                }
//                cout << "Pushing Si " << Si[i] << endl;
                globalAlignment.push_back( Si[i] );
            }
        }

//        if( depth == 3 ) {
//            cout << "Final alignment by Center Star\n";
//            for_each( globalAlignment.begin(), globalAlignment.end(), [](string s) {
//                cout << s << endl;
//            });
//        }

//        now I have a full alignment, I have to calculate the cost now which will be my estimate

        string temp;
        for( int i = 0; i < (globalAlignment[0]).size(); i++ ) {
            temp.clear();
            for( int j = 0; j < globalAlignment.size(); j++ ) {
                temp.push_back( globalAlignment[j][i] );
            }

//            cout << "\nSending this to findCost " << temp << " ";
//            vector<int> temp1 = getStringInts(temp);
//            for_each( temp1.begin(), temp1.end(), [](int i){cout << i;});

            heuristicMinCost += findCost( getStringInts(temp) );
        }

        if( minCost > heuristicMinCost + costSoFar ) {

            cout << "String so far\n";
            for_each( stringsSoFar.begin(), stringsSoFar.end(), [](string s) {
                cout << s << endl;
            });

            cout << "Final alignment by Center Star\n";
            for_each( globalAlignment.begin(), globalAlignment.end(), [](string s) {
                cout << s << endl;
            });

            minCost = heuristicMinCost + costSoFar;
            cout << "\n\nCenter Star found minimum cost goal - " << minCost;
            state tempGoal;
            string temp;
            for( int i = 0; i < stringNumber; i++ ) {

                if( i == tempMinIndex ) {
                    temp = *(new string(stringsSoFar[i]));
                    (tempGoal.stringsSoFar).push_back( temp.append(globalAlignment[0]) );
                } else if( i < tempMinIndex ){
                    temp = *(new string(stringsSoFar[i]));
                    (tempGoal.stringsSoFar).push_back( temp.append(globalAlignment[i+1]) );
                } else {
                    temp = *(new string(stringsSoFar[i]));
                    (tempGoal.stringsSoFar).push_back( temp.append(globalAlignment[i]) );
                }

            }
            tempGoal.costSoFar = minCost;
            tempGoal.discoveredAt = timeTaken;
            temp.clear();
            tempGoal.viewStateInfo();
            minState = new state(tempGoal);
        }
//        ideally, it should be divided by 2
        heuristicMinCost /= divideFactor;

//        if( depth == 3 )
//            cout << "\nHeuristic min cost - " << heuristicMinCost << endl;

        delete [] M;
        delete [] stringCostSum;

        Sc.clear();
        Si.clear();
        globalAlignment.clear();
        temp.clear();

//        if( depth == 1 )
//            cout << "heursitc value - " << heuristicMinCost;
    }


//    will return a vector of all next states
    vector<state> exploreStates() {
        tempStates.clear();
        getState( 0 );

//        to remove the last entry which indicates inserting hyphens for each string which is completely useless
//        remove the last entry only if the number of hyphens there are equal to number of strings
        if( tempStates.size()>1 && tempStates.back().hyphens == stringNumber )
            tempStates.pop_back();

//        apply heuristic and sort appropriately and return appropriate vector

//        sort according to heuristic
        sort( tempStates.begin(), tempStates.end(), [&](state s1, state s2 ){
//            higher the heuristic value returned, lower the desirability of the state
//            sooner the state will be enstacked and later will be popped
            if( s1.costIncurred == 0 )
                return false;
            else if( s2.costIncurred == 0 )
                return true;
            else if( s1.heuristicMinCost + s1.costIncurred > s2.heuristicMinCost + s2.costIncurred )
                return true;
            else
                return false;
        });

//        cout << "Checking the nodes to be returned\n";
//        fflush(stdout);
//        for_each( tempStates.begin(), tempStates.end(), [](state s){
//            cout << s.heuristicMinCost + s.costIncurred << endl;
//        });
//        cout << "Checked\n";
        fflush(stdout);

        float minEstimate = 0;
        if( tempStates.size() > 1 ) {
                minEstimate = (tempStates.back()).heuristicMinCost + (tempStates.back()).costIncurred;

//            attempt to prune worse nodes
            while( true ) {
                if( minEstimate < (tempStates.at(0)).costIncurred + (tempStates.at(0)).heuristicMinCost ) {
//                    cout << " Pruning nodes";
                    tempStates.erase( tempStates.begin() );
                } else
                    break;
            }
        }
        fflush(stdout);
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
        endt = clock();
        cout << "\n\nStarting index of each string - ";
        for_each( startingIndex.begin(), startingIndex.end(), [](int i){cout << i;} );
        cout << "\nCost to come to this state from the previous state - " << costIncurred;
        cout << "\nTotal cost                                         - " << costSoFar;
        cout << "\nDepth - " << depth;
        cout << "\nDivide factor - " << divideFactor;
        cout << "\nHeuristic min estimated cost of this state - " << heuristicMinCost;
        cout << "\nGoal cost till now - " << minCost;
        cout << "\nNodes processed till now - " << statesProcessed;
        cout << "\nTime this node was discovered - " << discoveredAt;
        cout << "\nCurrent time - " << timeTaken;
        cout << "\nMaximum time - " << ttime;
        cout << "\nString upto this state including this state - \n";
        for_each( stringsSoFar.begin(), stringsSoFar.end(), [](string s){cout << s << endl;} );

    }

}startState, currentState;


// input is the set of indices of vocab which represents character of string

main() {
    divideFactor = 0.9;
    begt = clock();

    vector<state> pendingStates;

    currentState.costIncurred = 0;
    currentState.costSoFar = 0;
    currentState.depth = 0;

//    this vector will hold all the states that will be explored by the currentState
    vector<state> exploredStates;

    cin >> ttime >> vocabNumber;
    hyphen = vocabNumber;

    ttime *= 60;

    char temp;
    for( int i = 0; i < vocabNumber; i++ ) {
        cin >> temp;
        vocab.push_back(temp);
//        to remove commas
        if( i != vocabNumber-1 )
            cin >> temp;
    }

//    check file input format
    cin >> stringNumber;

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


////    setting the maximum length of strings
//    vecStringIterator tempvsIt= max_element( strings.begin(), strings.end(), []( string s1, string s2 ){
//       return s1.size() < s2.size();
//    });
//    for_each( strings.begin(), strings.end(), [&](string s) {
//        if( (*tempvsIt).size() == s.size() )
//            isMaxLength.push_back( true );
//        else
//            isMaxLength.push_back( false );
//    });

//    for_each( isMaxLength.begin(), isMaxLength.end(), [](bool b){cout<<b;});

////    compute average of MC's component
//    float mcSum = 0;
//    for( int i = 0; i < vocabNumber; i++ ) {
//        for( int j = 0; j < i; j++ ) {
//            mcSum += MC[i][j];
//        }
//    }
//    mcAvg = (mcSum) / ((vocabNumber*vocabNumber-vocabNumber) / 2);

//    check if input received properly or not
    cout << "\nInput strings\n";
    for_each(strings.begin(), strings.end(), [](string s){cout<<s<<endl;});

    cout << "\nCC " << CC;
    cout << "\nMC\n";
    for_each(MC.begin(), MC.end(), [](vector<int> v){
        for_each(v.begin(), v.end(), [](int i){cout<<i;});
        cout << endl;
    });

//    cout << "MCsum and MCavg are - " << mcSum << " " << mcAvg << endl;

//    preprocessing
//    tells the index number in vocab corresponding to strings vector
//    string "ABC" will be converted to {3,1,2} if vocab vector says CAB
    for_each( strings.begin(), strings.end(), [&](string s) {

        vector<int> temp = getStringInts( s );
        stringInts.push_back( temp );
        temp.clear();
    });

//    check if the values have been given properly or not
    cout << "\nstringInts\n";
    for_each(stringInts.begin(), stringInts.end(), [](vector<int> v){
        for_each(v.begin(), v.end(), [](int i){cout<<i;});
        cout << endl;
    });

    startState = *(new state( currentState ));

//    STARTING AI
//    ITS IDA*

    start:
    divideFactor += 0.1;
    pendingStates.push_back( startState );


//    do it till the pendingStates is not empty
    do {

//        return the result obtained, no time is left
        if( ttime - timeTaken < 10 )
            goto done;

        static int timer = 0;
        timer++;

//        currentState = pendingStates.back();
//        pendingStates.pop_back();

//        if part is lowest cost search
//        else part is A*
        if( timer % (500/stringNumber) ) {
            currentState = pendingStates.back();
            pendingStates.pop_back();
        } else {
            cout << "Stack size is " <<    pendingStates.size() << endl;
            stateIterator sIt = min_element( pendingStates.begin(), pendingStates.end(), [](state a, state b){
                    if( a.costSoFar + a.heuristicMinCost < b.costSoFar + b.heuristicMinCost )
                       return true;
                    else
                       return false;
                });
            currentState = *sIt;
            currentState.viewStateInfo();
            pendingStates.erase( sIt );
        }

        statesEncountered++;

//        currentState.viewStateInfo();
//        goal solution so far is better than any way this state can lead
        if( currentState.costSoFar + currentState.heuristicMinCost >= minCost ) {
            if( currentState.costSoFar < minCost ) {
                nodeSaved++;
//                cout << "Node saved by heuristic\n";
            }
            goto afterProcess;
        }

//        if( currentState.depth == 4 )
//            currentState.viewStateInfo();

        if( !currentState.isGoal() ) {
            vector<state> temp = currentState.exploreStates();
            if( temp.size() > 1 && temp.back().costIncurred == 0 ) {
//                cout << "\nZERO COST FOUND";
                pendingStates.push_back(temp.back());
                temp.clear();
            } else {
            for_each( temp.begin(), temp.end(), [&](state s){
//                cout << "Adding state";
//                s.viewStateInfo();
                pendingStates.push_back(s);
                } );
            }
        } else {
            if( currentState.costSoFar < minCost ) {
                minCost = currentState.costSoFar;
                minState = &currentState;
                cout << "\nGoal reached\n";
                currentState.viewStateInfo();
            }
        }
        statesProcessed++;
        afterProcess:;
    } while( !pendingStates.empty() );

    if( ttime - timeTaken > 30 )
        goto start;

    done:

    cout << "\n\n\n\n\n\nAnd the winner is - \n\n";
    minState->viewStateInfo();
    endt = clock();

    cout << "\n\nTime taken is            - " << timeTaken;
    cout << "\nStates processed         - " << statesProcessed;
    cout << "\nStates encountered       - " << statesEncountered;
    cout << "\nNodes saved by heuristic - " << nodeSaved;
    cout << endl;

    return 0;
}


/*

Strings for editDistance T TGA T-- TGA
Strings for editDistance T GTG -T- GTG
Strings for editDistance T TCGA T--- TCGA
Strings for editDistance T G T G
Strings for editDistance T  T -
Strings for editDistance TGA GTG TGA GTG
Strings for editDistance TGA TCGA T-GA TCGA
Strings for editDistance TGA G TGA -G-
Strings for editDistance TGA  TGA ---
Strings for editDistance GTG TCGA GTG- TCGA
Strings for editDistance GTG G GTG --G
Strings for editDistance GTG  GTG ---
Strings for editDistance TCGA G TCGA --G-
Strings for editDistance TCGA  TCGA ----
Strings for editDistance G  G -

T-- TGA
-T- GTG
T--- TCGA
T G
T -
Final alignment by Center Star
-T--
-TGA
GTG
-TCGA
-G--
----

-T--
-TGA
-TG
-TCGA

Heuristic min cost - 37

Strings for editDistance T TGA T-- TGA
Strings for editDistance T GTG -T- GTG
Strings for editDistance T TCGA T--- TCGA
Strings for editDistance T G T G
Strings for editDistance T A T A
Strings for editDistance TGA GTG TGA GTG
Strings for editDistance TGA TCGA T-GA TCGA
Strings for editDistance TGA G TGA -G-
Strings for editDistance TGA A TGA --A
Strings for editDistance GTG TCGA GTG- TCGA
Strings for editDistance GTG G GTG --G
Strings for editDistance GTG A GTG -A-
Strings for editDistance TCGA G TCGA --G-
Strings for editDistance TCGA A TCGA ---A
Strings for editDistance G A G A

T-- TGA
-T- GTG
T--- TCGA
T G
T A
Final alignment by Center Star
-T--
-TGA
GTG
-TCGA
-G--
-A--

Heuristic min cost - 33.5

Strings for editDistance T TGA T-- TGA
Strings for editDistance T GTG -T- GTG
Strings for editDistance T GTCGA -T--- GTCGA
Strings for editDistance T  T -
Strings for editDistance T  T -
Strings for editDistance TGA GTG TGA GTG
Strings for editDistance TGA GTCGA -T-GA GTCGA
Strings for editDistance TGA  TGA ---
Strings for editDistance TGA  TGA ---
Strings for editDistance GTG GTCGA GT-G- GTCGA
Strings for editDistance GTG  GTG ---
Strings for editDistance GTG  GTG ---
Strings for editDistance GTCGA  GTCGA -----
Strings for editDistance GTCGA  GTCGA -----
Strings for editDistance

T-- TGA
-T- GTG
-T--- GTCGA
T -
T -

-T---
-TGA-
GTG--
GTCGA







*/
