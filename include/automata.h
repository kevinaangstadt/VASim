/**
 * @file
 */
#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "stack.h"
#include "ste.h"
#include "pdstate.h"
#include "specialElement.h"
#include "ANMLParser.h"
#include "MNRLAdapter.h"
#include "errors.h"
#include "util.h"

#include <cmath>
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <queue>
#include <deque>
#include <set>
#include <unordered_set>
#include <vector>
#include <list>
#include <fstream>
#include <algorithm>
#include <mnrl.hpp>



class Automata {

private:
    std::string filename;
    std::string id;
    bool profile;
    bool quiet;
    bool report;
    bool dump_state;
    bool endOfData;
    uint32_t dump_state_cycle;

    // our DPDA stack
    std::deque<uint8_t> pdstack;

    std::unordered_map<std::string, Element*> elements;
    std::vector<STE*> starts;
    std::vector<Element*> reports;
    std::unordered_map<std::string, SpecialElement*> specialElements;
    std::unordered_map<std::string, SpecialElement*> firstLayerSpecialElements;

    // Functional element stacks
    Stack<Element *> enabledSTEs;
    Stack<STE*> activatedSTEs;
    Stack<STE*> latchedSTEs;
    std::queue<Element*> enabledSpecialElements;
    std::queue<SpecialElement*> activatedSpecialElements;
    std::vector<SpecialElement*> latchedSpecialElements;
    std::vector<SpecialElement*> activateNoInputSpecialElements;


    // Simulation Statistics
    std::vector<std::pair<uint64_t, std::string>> reportVector;
    std::unordered_map<uint32_t, std::list<std::string>> activationVector;
    std::unordered_map<std::string, uint32_t> activationHist;
    std::vector<uint32_t> enabledHist;
    std::vector<uint32_t> activatedHist;
    uint32_t maxActivations;
    uint32_t maxStackSize = 0;
    std::unordered_map<Element*, uint32_t> enabledCount;
    std::unordered_map<Element*, uint32_t> activatedCount;
    std::queue<Element *> enabledLastCycle;
    std::queue<Element *> activatedLastCycle;
    std::queue<Element *> reportedLastCycle;

    // Misc
    vasim_err_t error;

    //
    uint64_t cycle;
    uint64_t clock;

public:

    // Constructors
    Automata();
    Automata(std::string fn);
    Automata(std::string fn, std::string filetype);
    void parseAutomataFile(std::string fn, std::string filetype);
    void finalizeAutomata();

    // Get/set
    std::vector<STE *> &getStarts();
    std::vector<Element *> &getReports();
    std::unordered_map<std::string, Element *> &getElements();
    std::unordered_map<std::string, SpecialElement *> &getSpecialElements();
    std::unordered_map<std::string, uint32_t> &getActivationHist();
    std::vector<std::pair<uint64_t, std::string>> &getReportVector();
    std::deque<uint8_t> &getPDStack() { return pdstack; }
    uint32_t getMaxActivations();
    uint32_t getMaxStackSize();
    void enableProfile();
    void enableReport();
    void enableQuiet();
    void enableDumpState(uint64_t);
    void disableProfile();
    Element *getElement(std::string);
    void setErrorCode(vasim_err_t err);
    vasim_err_t getErrorCode();
    void unmarkAllElements();
    uint64_t getClock() { return clock; }

    // I/O
    void print();
    void writeReportToFile(std::string fn);
    void printReportBatchSim();
    std::string activationHistogramToString();
    void automataToDotFile(std::string fn);
    void automataToNFAFile(std::string fn);
    void automataToANMLFile(std::string fn);
    void automataToMNRLFile(std::string fn);
    void automataToHDLFile(std::string fn);
    void automataToBLIFFile(std::string fn);
    void automataToGraphFile(std::string fn);

    // Simulation
    void initializeSimulation();
    void simulate(uint8_t *inputs, uint64_t start_index, uint64_t length, uint64_t end_index);
    bool simulate(uint8_t);
    bool simulate();
    bool simulate(uint8_t, std::vector<std::string> injects);
    void reset();
    void enableStartStates(bool enableStartOfData); // formerly stageOne
    bool computeStackMatches();
    void computeSTEMatches(uint8_t); // formerly stageTwo
    void performStackOperations();
    void enableSTEMatchingChildren(); // formerly stageThree
    void specialElementSimulation(); // formerly stageFour/Five
    uint64_t tick();
    uint64_t tock();


    // Statistics and Profiling
    void profileEnables();
    void profileActivations();
    std::unordered_map<Element*, uint32_t> &getEnabledCount();
    std::unordered_map<Element*, uint32_t> &getActivatedCount();
    std::queue<Element *> &getEnabledLastCycle();
    std::queue<Element *> &getActivatedLastCycle();
    std::queue<Element *> &getReportedLastCycle();
    void buildActivationHistogram(std::string fn);
    void calcEnableDistribution();
    void printGraphStats();
    void printSTEComplexity();
    void dumpSTEState(std::string fn);
    void dumpSpecelState(std::string fn);

    // Manipulation
    void addEdge(Element *from, Element *to);
    void addEdge(std::string from, std::string to);
    void removeEdge(Element *from, Element *to);
    void removeEdge(std::string from, std::string to);
    void updateElementId(Element *el, std::string newId);
    void addSTE(STE *, std::vector<std::string>&);
    void rawAddSTE(STE *);
    void rawAddSpecialElement(SpecialElement *);
    void leftMergeSTEs(STE*,STE*);
    void rightMergeSTEs(STE*,STE*);
    void removeElement(Element *);
    void removeOrGates();
    void removeCounters();
    void convertAllInputStarts();
    std::vector<Automata*> splitConnectedComponents();
    std::vector<Automata*> generateGNFAs();
    void unsafeMerge(Automata *);
    Automata *clone();
    void enforceFanIn(uint32_t fanin_max);
    void enforceFanOut(uint32_t fanout_max);

    // Optimization
    void leftMinimizeStartStates();
    uint32_t leftMinimize();
    uint32_t leftMinimizeChildren(STE*, int);
    Automata * generateDFA();
    void eliminateDeadStates();

    // Util
    std::set<STE*>* follow(uint32_t, std::set<STE*>*);
    std::string getElementColor(std::string);
    std::string getElementColorLog(std::string);
    std::string getLogElementColor(std::string);
    void validate();
    void validateStartElement(Element *);
    void validateReportElement(Element *);
    void validateElement(Element *);

};

#endif
