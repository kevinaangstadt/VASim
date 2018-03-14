/**
 * @file
 */
//
#ifndef UTIL_H
#define UTIL_H

#include <bitset>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <string>
#include <ios>
#include <iterator>

uint32_t fileSize(std::string fn);
void inputFileCheck();
std::vector<unsigned char> file2CharVector(std::string fn);
uint8_t * parseInputStream(bool simulate, bool input_string, uint64_t *size, char ** argv, uint32_t optind);
std::string bitsetToCharset(std::bitset<256> column);
void writeStringToFile(std::string str, std::string fn);
void appendStringToFile(std::string str, std::string fn);
void writeIntVectorToFile(std::vector<uint32_t> &vec, std::string fn);
std::string getFileExt(const std::string& s);
void setRange(std::bitset<256> &column, int start, int end, int value);
void parseSymbolSet(std::bitset<256> &column, std::string symbol_set);


/*
 * Quine-Mcklusky Algorithm for calculating character-set complexity
 */
int count1s(size_t x);

struct Implicant {

    int implicant;
    std::string minterms;
    std::vector<int> mints;
    int mask;
    std::string bits;
    int ones;
    bool used;

    Implicant(int i = 0, std::vector<int> min = std::vector<int>(), std::string t = "", int m = 0, bool
              u = false)
        : implicant(i), mask(m), ones(0), used(u)
    {
        if (t == "") {
            std::stringstream ss;
            ss << 'm' << i;
            minterms = ss.str();
        }
        else minterms = t;
        if (min.empty()) mints.push_back(i);
        else mints = min;
        int vars = 8;
        int bit = 1 << vars;
        while (bit >>= 1)
            if (m & bit) bits += '-';
            else if (i & bit) { bits += '1'; ++ones; }
            else bits += '0';
    }

    bool operator<(const Implicant& b) const { return ones < b.ones; }

    std::vector<int> cat(const Implicant &b) {
        std::vector<int> v = mints;
        v.insert(v.end(), b.mints.begin(), b.mints.end());
        return v;
    }

    //friend std::ostream &operator<<(std::ostream &out, const Implicant &im);
};

void mul(std::vector<size_t> &a, const std::vector<size_t> &b); 

int QMScore(std::bitset<256> column); 

void find_and_replace(std::string& source,
                      std::string const& find,
                      std::string const& replace);


#endif
