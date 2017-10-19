//
#ifndef COUNTER_H
#define COUNTER_H

#include "specialElement.h"
#include <unordered_map>

class Counter : public SpecialElement {

    enum Mode { LATCH, ROLL, PULSE };

protected:
    uint32_t target;
    uint32_t value;
    Mode mode;
    bool dormant;
    bool latched;

public:
    Counter(std::string, uint32_t, std::string);
    ~Counter();


    virtual bool calculate();
    void setMode(std::string);
    void setTarget(uint32_t);
    uint32_t getTarget();
    uint32_t getValue();
    virtual bool deactivate();
    virtual std::string toString();
    virtual std::string toANML();
    virtual std::string toHDL(std::unordered_map<std::string, std::string>);

    virtual void stageOneHooks();

protected:
    bool cnt_trig, rst_trig;
};

#endif
