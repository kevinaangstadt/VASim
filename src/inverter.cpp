/**
 * @file
 */
#include "inverter.h"

using namespace std;
using namespace MNRL;

/*
 *
 */
Inverter::Inverter(string id) : Gate(id) {

}

/*
 *
 */
Inverter::~Inverter() {

}


/*
 *
 */
bool Inverter::calculate() {

    bool result = false;
    
    for(auto ins: inputs) {

        if(ins.second == false) {
            result = true;
        }

    }

    return result;
}

/*
 *
 */
string Inverter::toString() {

    string str("Inverter:");
    str.append("\tid=");
    str.append(id);
    return str;
}

/*
 *
 */
string Inverter::toANML() {

    string str("<inverter ");

    str += Gate::toANML();

    str.append("</inverter>");

    return str;
}

MNRLNode *Inverter::toMNRLObj() {
    return toMNRLBool(MNRLDefs::BooleanMode::NOT);
}

/*
 *
 */
bool Inverter::canActivateNoEnable() {

    return true;
}

/*
 *
 */
string Inverter::toHDL(unordered_map<string, string> id_reg_map) {
    
    string inv = "";

    // header
    inv += "\t////////////////\n";
    inv += "\t// INVERTER: " + getId() + "\n";
    inv += "\t////////////////\n";

    // print input enable wire
    inv += "\t// Input enable OR gate\n";
    string enable_name = getId() + "_IN";
    inv += "\twire\t" + enable_name + ";\n";

    // attach input signals to enable wire
    inv += "\tassign " + enable_name + " = ";
    // for all inputs    
    bool first = true;
    for(auto in : getInputs()){
        if(first){
            inv += id_reg_map[in.first];
            first = false;
        }else{
            inv += " | " + id_reg_map[in.first];
        }
    }

    inv += ";\n";

    //
    inv += "\t// Inverter logic\n";
    //inv += "\t(*dont_touch = \"true\"*) always @(posedge Clk) // should not be optimized\n";
    string wire_name = id_reg_map[getId()];
    //inv += "\tbegin\n";
    inv += "\t\tassign " + wire_name + " = ~" + enable_name + ";\n";
    //inv += "\tend\n\n";


    return inv;
}

/*
 *
 */
ElementType Inverter::getType() {

    return ElementType::INVERTER_T;
}
