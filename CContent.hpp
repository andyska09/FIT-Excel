#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <variant>
#include <cmath>

using CValue = std::variant<std::monostate, double, std::string>;

// encapsulates CValue so it is usable in AST
class CContent
{
public:
    CContent();
    CContent(const CValue &value);
    // arithmetic
    friend CContent operator+(const CContent &lhs, const CContent &rhs);
    friend CContent operator-(const CContent &lhs, const CContent &rhs);
    friend CContent operator*(const CContent &lhs, const CContent &rhs);
    friend CContent operator/(const CContent &lhs, const CContent &rhs);
    CContent toExp(const CContent &power) const; // exponential
    CContent operator-() const;                  // unary

    // comparison
    //< <=, > >=, == a !=
    friend CContent operator<(const CContent &lhs, const CContent &rhs);
    friend CContent operator>(const CContent &lhs, const CContent &rhs);
    friend CContent operator<=(const CContent &lhs, const CContent &rhs);
    friend CContent operator>=(const CContent &lhs, const CContent &rhs);
    friend CContent operator==(const CContent &lhs, const CContent &rhs);
    friend CContent operator!=(const CContent &lhs, const CContent &rhs);
    int compare(const CContent &other) const;
    bool isDouble() const;
    bool isString() const;
    bool isMonostate() const;
    // IO
    friend std::ostream &operator<<(std::ostream &os, const CContent &content);
    CValue m_value;

private:
    // saving string as string literal => all " must be doubled
    void printString(std::ostream &os) const;
};