//
#include "and.h"

using namespace std;

/*
 *
 */
AND::AND(string id) : Gate(id) {

}

/*
 *
 */
AND::~AND() {

}

/*
 * DOES AND OPERATION OVER INPUTS
 */
bool AND::calculate() {

    if(DEBUG)
        cout << "AND: id=" << id << " CALCULATING" << endl;

    bool result = true;
    
    for(auto e : inputs) {
    
        if(DEBUG)
            cout << e.second << endl;
        result = result && e.second;
    }

    if(DEBUG)
        cout << "RESULT: " << result << endl;

    if(inputs.size() > 0)
        return result;
    else{
        if(DEBUG)
            cout << "ONLY ONE INPUT TO AND GATE! CALC FALSE" << endl;
        return false;
    }
}

/*
 *
 */
string AND::toString() {

    string str(id + ", ");
    if (activated)
        return str + "1";
    else
        return str + "0";
    
    //string str("AND:");
    //str.append("\tid=");
    //str.append(id);
    //
    //return str;
}

/*
 *
 */
string AND::toANML() {

    string str("AND: NOT IMPLEMENTED YET\n");
    return str;
}
