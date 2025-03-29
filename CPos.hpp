#pragma once
#include <string>
#include <iostream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <functional>
#include <unordered_map>

class CPos
{
public:
    CPos(std::string_view str);
    size_t m_row;
    size_t m_col;
    bool m_isAbsRow = false;
    bool m_isAbsCol = false;

    void shiftBy(int x, int y);
    bool operator==(const CPos &other) const;
    bool operator<(const CPos &other) const;
    friend std::ostream &operator<<(std::ostream &os, const CPos &pos);
    void print(std::ostream &os) const;

private:
    void parseCol(std::istringstream &input);
    void parseRow(std::istringstream &input);
    size_t letterToInt(char ch) const;
    char intToLetter(int i) const;
    size_t getValue(const std::vector<size_t> &digits, const size_t base) const; // calculates the numerical value of col id
    std::string getCol() const;
};

struct CPosHasher
{
    std::size_t operator()(const CPos &pos) const
    {
        using std::hash;
        return ((hash<size_t>()(pos.m_row) ^ (hash<size_t>()(pos.m_col) << 1)) >> 1);
    }
};