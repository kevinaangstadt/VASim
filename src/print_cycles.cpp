#include "automata.h"

#include <iostream>

using namespace std;

void printElements(queue<Element *> q) {
  while(!q.empty()) {
    Element *e = q.front();
    q.pop();

    cout << e->getId() << endl;
  }
}

void printCycle(uint64_t i, queue<Element *> en, queue<Element *> ac) {
  cout << "==================================" << endl <<
    "cycle:" << i << endl;

  cout << "activated:" << endl;

  printElements(ac);

  cout << "enabled for next cycle:" << endl;

  printElements(en);


}

int main(int argc, char* argv[]) {
  // we'll assume two inputs
  if (argc != 3) {
    cerr << "Expecting two arguments" << endl;
    exit(1);
  }

  // read in the input
  // open the file:
  std::ifstream file(argv[2], ios::binary);

  // get its size:
  std::streampos fileSize;

  file.seekg(0, ios::end);
  fileSize = file.tellg();
  file.seekg(0, ios::beg);

  // Stop eating new lines in binary mode!!!
  file.unsetf(std::ios::skipws);

  // reserve capacity
  std::vector<unsigned char> vec;
  vec.reserve(fileSize);

  // read the data:
  vec.insert(vec.begin(),
             std::istream_iterator<unsigned char>(file),
             std::istream_iterator<unsigned char>());

  uint64_t length = vec.size();
  uint8_t *inputs = (uint8_t*)malloc(sizeof(uint8_t) * length);

  uint64_t counter = 0;
  for(unsigned char val : vec) {
    inputs[counter] = (uint8_t) val;
    counter++;
  }

  Automata ap;

  ap.parseAutomataFile(argv[1], "mnrl");

  ap.finalizeAutomata();

  ap.enableReport();
  ap.enableProfile();

  ap.initializeSimulation();

  cout << "==================================" << endl <<
    "start:" << endl << "enabled:" << endl;

  printElements(ap.getEnabledLastCycle());

  uint64_t i;
  for(i = 0; i < length; i++) {
    while(!ap.simulate(inputs[i])) {
      printCycle(i, ap.getEnabledLastCycle(), ap.getActivatedLastCycle());
    }

    printCycle(i, ap.getEnabledLastCycle(), ap.getActivatedLastCycle());

  }

  while(ap.getEnabledLastCycle().size() > 0 && !ap.simulate()) {
    printCycle(i, ap.getEnabledLastCycle(), ap.getActivatedLastCycle());
  }

  free(inputs);
}
