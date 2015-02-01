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

// a state is composed of
struct state {

};

// input is the set of indices of vocab which represents character of string
int findCost( vector<int> s ) {

    cout << "\nInput to findCost - ";
    for_each(s.begin(), s.end(), [](int i){cout<<i;});

    int cost = 0;

    while( s.size() > 1 ) {
//        start it from the next element in the list
        for( intIterator it = s.begin()+1; it != s.end(); it++ ) {
            cost += (MC.at(*(s.begin()))).at(*it);
        }
        s.erase(s.begin());
    }

    return cost;

}

main() {
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

    for( int i = 0; i < )




    cout << endl;
    return 0;
}
