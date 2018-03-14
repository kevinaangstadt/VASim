#include "pdstate.h"

using namespace std;

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

bool PDState::isInputEpsilon() { return eps; }

bool PDState::doesPeek() { return stack_column.count() != 256;  }
