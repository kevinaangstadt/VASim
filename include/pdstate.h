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
            uint8_t pop,
            bool eps,
            std::string start);
    ~PDState();

    virtual ElementType getType();

    inline bool smatch(uint8_t top) {
        return stack_column.test(top);
    };

    bool getPush();
    uint8_t getPop();
    uint8_t getPushChar();
    std::bitset<256>& getStackSet();
    
    void setPushChar(uint8_t);
    void setPop(uint8_t);
    void setStackSet(std::bitset<256> &);

    bool isInputEpsilon();
    bool doesPeek();
    
    bool linearEpsilon(bool);
    
    virtual std::shared_ptr<MNRL::MNRLNode> toMNRLObj();

protected:
    std::string stack_set;
    uint8_t push_char;

    std::bitset<256> stack_column;

    bool push;
    uint8_t pop;
    bool eps;

};

#endif
