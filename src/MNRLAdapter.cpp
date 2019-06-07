/**
 * @file
 */
#include "MNRLAdapter.h"
#include <iostream>

using namespace std;
using namespace MNRL;

static string convertStart(MNRLDefs::EnableType st) {
    switch(st) {
        case MNRLDefs::EnableType::ENABLE_ALWAYS:
            return "all-input";
        case MNRLDefs::EnableType::ENABLE_ON_ACTIVATE_IN:
            return "none";
        case MNRLDefs::EnableType::ENABLE_ON_START_AND_ACTIVATE_IN:
            return "start-of-data";
        // FIXME VASim does not currently support "onLast" enable type
        //case MNRLDefs::EnableType::ENABLE_ON_LAST:
        //    return "on-last";
        default:
            cerr << "Non-ANML-like enable signal. Exiting." << endl;
            exit(1);
    }
}

static bool reportEOF(MNRLDefs::ReportEnableType r) {
  switch(r) {
    case MNRLDefs::ReportEnableType::ENABLE_ON_LAST:
      return true;
    default:
      return false;
  }
}

static string convertThreshold(MNRLDefs::CounterMode m) {
    switch(m) {
        case MNRLDefs::CounterMode::TRIGGER_ON_THRESHOLD:
            return "pulse";
        case MNRLDefs::CounterMode::HIGH_ON_THRESHOLD:
            return "latch";
        case MNRLDefs::CounterMode::ROLLOVER_ON_THRESHOLD:
            return "roll";
    }
}

static void addOutputs(MNRLNode *n, Element *e) {    
    for(auto &p : n->getOutputConnections()) {
        for(auto &c : p.second.getConnections()) {
            const string &id = c.first;
            const string &p_id = c.second;
            
            if(
               p_id.compare( MNRLDefs::UP_COUNTER_COUNT ) == 0 ||
               p_id.compare( MNRLDefs::UP_COUNTER_RESET ) == 0 ) {
                
                // we want to keep the port
                e->addOutput(id + ":" + p_id);
            } else {
                // we just drop the port
                e->addOutput(id);
            }
            
        }
    }
}

MNRLAdapter::MNRLAdapter(string &filename) : filename(filename) {}

STE *MNRLAdapter::parseSTE(MNRLHState *hState) {
    
    string id = hState->getId();
    string symbol_set = hState->getSymbolSet();
    string start = convertStart(hState->getEnable());
    
    // create new STE
    STE *s = new STE(id, symbol_set, start);
    s->setIntId(unique_ids++);
    
    s->setReporting(hState->getReport());
    
    // check to see if report is EOD
    bool is_eod = reportEOF(hState->getReportEnable());
    s->setEod(is_eod);
    
    addOutputs(hState, s);
    
    s->setReportCode(hState->getReportId()->toString());
    
    
    
    return s;
}

Gate *MNRLAdapter::parseGate(MNRLBoolean *a) {
    string id = a->getId();
    
    Gate *s;
    
    switch(a->getMode()) {
        case MNRLDefs::BooleanMode::AND:
            s = new AND(id);
            break;
        case MNRLDefs::BooleanMode::OR:
            s = new OR(id);
            break;
        case MNRLDefs::BooleanMode::NOR:
            s = new NOR(id);
            break;
        case MNRLDefs::BooleanMode::NOT:
            s = new Inverter(id);
            break;
        default:
            cerr << "Unknown Gate Type. Exiting." << endl;
            exit(1);
    }
    
    s->setIntId(unique_ids++);
    
    addOutputs(a, s);
    
    s->setReporting(a->getReport());
    
    // check to see if report is EOD
    bool is_eod = reportEOF(a->getReportEnable());
    s->setEod(is_eod);
    
    s->setReportCode(a->getReportId()->toString());
    
    return s;
}

Counter *MNRLAdapter::parseCounter(MNRLUpCounter *cnt) {
    string id = cnt->getId();
    uint32_t target = cnt->getThreshold();
    string at_target = convertThreshold(cnt->getMode());
    
    // create new Counter gate
    Counter *c = new Counter(id, target, at_target);
    c->setIntId(unique_ids++);
    
    addOutputs(cnt, c);
    
    c->setReporting(cnt->getReport());
    
    // check to see if report is EOD
    bool is_eod = reportEOF(cnt->getReportEnable());
    c->setEod(is_eod);
    
    c->setReportCode(cnt->getReportId()->toString());
    
    return c;
}

void MNRLAdapter::parse(unordered_map<string, Element*> &elements, 
                       vector<STE*> &starts, 
                       vector<Element*> &reports, 
                       unordered_map<string, SpecialElement*> &specialElements,
                       string * id,
                       vector<SpecialElement*> &activateNoInputSpecialElements)
{
    
    try {
        // load the MNRL file
        MNRLNetwork net = loadMNRL(filename);
        
        *id = net.getId();
        
        for(auto &node : net.getNodes()) {
            Element *s;
            switch(node.second->getNodeType()) {
                case MNRLDefs::NodeType::HSTATE:
                    s = parseSTE(dynamic_cast<MNRLHState*>(node.second));
                    if(dynamic_cast<STE *>(s)->isStart()) {
                        starts.push_back(dynamic_cast<STE *>(s));
                    }
                    break;
                case MNRLDefs::NodeType::BOOLEAN:
                    s = parseGate(dynamic_cast<MNRLBoolean*>(node.second));
                    specialElements[s->getId()] = dynamic_cast<Gate *>(s);
                    
                    // If this is a NOR or Inverter, we need to add it ot another vector
                    switch(dynamic_cast<MNRLBoolean*>(node.second)->getMode()) {
                        case MNRLDefs::BooleanMode::NOR:
                        case MNRLDefs::BooleanMode::NOT:
                            activateNoInputSpecialElements.push_back(dynamic_cast<SpecialElement*>(s));
                            break;
                        default:
                            break;
                    }
                    
                    break;
                case MNRLDefs::NodeType::UPCOUNTER:
                    s = parseCounter(dynamic_cast<MNRLUpCounter*>(node.second));
                    specialElements[s->getId()] = dynamic_cast<Gate *>(s);
                    break;
                default:
                    cerr << "Unsupported Node: " << node.second->getId() << endl;
                    exit(1);
            }
            
            elements[s->getId()] = dynamic_cast<Element*>(s);
            
            
            if(s->isReporting()){
                reports.push_back(s);
            }
        }
        
    } catch (...) {
        cerr << "Unexpected Parsing Error. Exiting." << endl;
        exit(1);
    }
    
}
