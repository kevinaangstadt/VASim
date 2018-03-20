#include "pdstate.h"

using namespace std;
using namespace MNRL;

PDState::PDState(string id,
                 string symbol_set,
                 string stack_set,
                 uint8_t push_char,
                 bool push,
                 uint8_t pop,
                 bool eps,
                 string start) :
            STE(id,symbol_set,start),
            stack_set(stack_set),
            push_char(push_char),
            push(push),
            pop(pop),
            eps(eps)
{
    for(uint32_t i = 0; i < 256; i++) {
        stack_column.set(i,0);
    }

    parseSymbolSet(stack_column, stack_set);
}

PDState::~PDState() {}

ElementType PDState::getType() { return PDSTATE_T; }

bool PDState::getPush() { return push; }
uint8_t PDState::getPop() { return pop; }
uint8_t PDState::getPushChar() { return push_char; }
std::bitset<256>& PDState::getStackSet() { return stack_column; }

void PDState::setPushChar(uint8_t p) { push_char = p; push = true; }
void PDState::setPop(uint8_t cnt) { pop = cnt; }
void PDState::setStackSet(std::bitset<256> &ss) { stack_column = ss; }

bool PDState::isInputEpsilon() { return eps; }

bool PDState::doesPeek() { return stack_column.count() != 256;  }

bool PDState::linearEpsilon(bool multi) {
  // we defiinitely can't if we have something other than one child
  if(outputSTEPointers.size() != 1) {
    return false;
  }
  
  PDState *child = dynamic_cast<PDState *>(outputSTEPointers.begin()->first);
  
  // we definitely can't if our single child is not an Input Epsilon state
  if(!child->isInputEpsilon()) {
    return false;
  }
  
  // we definitely can't (for now) if we have multiple inputs
  if(child->getInputs().size() > 1) {
    return false;
  }
  
  // now check various failing conditions
  if(
    (!multi && pop && child->getPop()) // if multi-pop is disabled and both pop
    || (push && child->getPush()) // both push
    || (
      (doesPeek() && doesPeek()) && // if both do a peek AND
        (                           // either
          (pop || push) ||          // we perform a push or pop OR
          (stack_column != child->getStackSet()) // the peeks don't match
        )
      ) 
  ) {
    return false;
  }
  
  return true;
}

/**
 * Returns a MNRL object corresponding to this PDState.
 * Note: that this doesn't contain the connections
 */
shared_ptr<MNRLNode> PDState::toMNRLObj() {
    
    MNRLDefs::EnableType en;
    switch(start) {
    case NONE:
        en = MNRLDefs::EnableType::ENABLE_ON_ACTIVATE_IN;
        break;
    case START_OF_DATA:
        en = MNRLDefs::EnableType::ENABLE_ON_START_AND_ACTIVATE_IN;
        break;
    case ALL_INPUT:
        en = MNRLDefs::EnableType::ENABLE_ALWAYS;
        break;
    }
    
    if(push) {
      if(!isInputEpsilon()) {
        return shared_ptr<MNRLHPDState>(new MNRLHPDState(
          symbol_set,
          stack_set,
          pop,
          to_string(push_char),
          en,
          id,
          reporting,
          report_code,
          shared_ptr<map<string,string>>(new map<string,string>())
        ));
      } else {
        return shared_ptr<MNRLHPDState>(new MNRLHPDState(
          stack_set,
          pop,
          to_string(push_char),
          en,
          id,
          reporting,
          report_code,
          shared_ptr<map<string,string>>(new map<string,string>())
        ));
      }
    } else {
      // there is no push
      if(!isInputEpsilon()) {
        return shared_ptr<MNRLHPDState>(new MNRLHPDState(
          symbol_set,
          stack_set,
          pop,
          en,
          id,
          reporting,
          report_code,
          shared_ptr<map<string,string>>(new map<string,string>())
        ));
      } else {
        return shared_ptr<MNRLHPDState>(new MNRLHPDState(
          stack_set,
          pop,
          en,
          id,
          reporting,
          report_code,
          shared_ptr<map<string,string>>(new map<string,string>())
        ));
      }
    }
    
}
