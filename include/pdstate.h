/**
 * @file
 */
//
#ifndef PDSTATE_H
#define PDSTATE_H

#include "ste.h"

class PDState : public STE {

public:
    PDState(std::string id,
            std::string symbol_set,
            std::string stack_set,
            uint8_t push_char,
            bool push,
            bool pop,
            bool eps,
            std::string start);
    ~PDState();

    virtual ElementType getType();

    inline bool smatch(uint8_t top) {
        return stack_column.test(top);
    };

    bool getPush();
    bool getPop();
    uint8_t getPushChar();

    bool isInputEpsilon();
    bool doesPeek();

protected:
    std::string stack_set;
    uint8_t push_char;

    std::bitset<256> stack_column;

    bool push;
    bool pop;
    bool eps;

};

#endif
