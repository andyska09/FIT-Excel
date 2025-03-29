#pragma once
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <variant>
#include <optional>
#include <compare>
#include <charconv>
#include <span>
#include <utility>

#include "expression.h"
#include "CPos.hpp"
#include "CContent.hpp"

using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;

constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
constexpr unsigned SPREADSHEET_SPEED = 0x08;
constexpr unsigned SPREADSHEET_PARSER = 0x10;

class CSpreadsheet;

// abstract class for a node in the AST
// all methods are called recursively on its descendants
class CExpr
{
public:
    virtual ~CExpr() = default;

    virtual void print(std::ostream &os) const = 0;

    // evaluates the expr tree and all trees that the this tree references
    virtual CContent eval(const CSpreadsheet &sheet) const = 0;
    virtual std::shared_ptr<CExpr> clone() const = 0;

    // fill dependencies with position of cell that are needed to eval this tree, used in checking for cyclic dependecies
    virtual void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const = 0;

    // shifts all references by i rows and j col, used for copying
    void virtual updateRef(int i, int j) = 0;
    friend std::ostream &operator<<(std::ostream &os, const CExpr &expr)
    {
        expr.print(os);
        return os;
    }
};

class Reference : public CExpr
{
public:
    Reference(const std::string &pos);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    CPos m_pos;
};

class Literal : public CExpr
{
public:
    Literal(CContent val);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    CContent m_value;
};

class Addition : public CExpr
{
public:
    Addition(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    ~Addition() override = default;
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;

private:
};

class Multiplication : public CExpr
{
public:
    Multiplication(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class Division : public CExpr
{
public:
    Division(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class Subtraction : public CExpr
{
public:
    Subtraction(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class Exponentiation : public CExpr
{
public:
    Exponentiation(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class Negation : public CExpr
{
public:
    Negation(std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Rhs;
};

class LessThan : public CExpr
{
public:
    LessThan(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class GreaterThan : public CExpr
{
public:
    GreaterThan(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class Equal : public CExpr
{
public:
    Equal(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class NotEqual : public CExpr
{
public:
    NotEqual(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class LessEqual : public CExpr
{
public:
    LessEqual(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class GreaterEqual : public CExpr
{
public:
    GreaterEqual(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs);
    std::shared_ptr<CExpr> clone() const override;
    CContent eval(const CSpreadsheet &sheet) const override;
    void getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const override;
    void updateRef(int i, int j) override;
    void print(std::ostream &os) const override;

private:
    std::shared_ptr<CExpr> m_Lhs;
    std::shared_ptr<CExpr> m_Rhs;
};

class CAstBuilder : public CExprBuilder
{
public:
    CAstBuilder(){};
    void opAdd() override;
    void opSub() override;
    void opMul() override;
    void opDiv() override;
    void opPow() override;
    void opNeg() override;
    void opEq() override;
    void opNe() override;
    void opLt() override;
    void opLe() override;
    void opGt() override;
    void opGe() override;
    void valNumber(double val) override;
    void valString(std::string val) override;
    void valNull();
    void valReference(std::string val) override; // TODO
    void valRange(std::string val) override;     // TODO
    void funcCall(std::string fnName,
                  int paramCount) override; // TODO

    std::shared_ptr<CExpr> getResult() const;

    std::stack<std::shared_ptr<CExpr>> m_stack;

private:
};

class CSpreadsheet
{
public:
    static unsigned capabilities()
    {
        return SPREADSHEET_CYCLIC_DEPS | SPREADSHEET_FILE_IO;
    }
    CSpreadsheet();
    bool load(std::istream &is);
    bool save(std::ostream &os) const;
    bool setCell(CPos pos,
                 std::string contents);
    CValue getValue(CPos pos);
    void copyRect(CPos dst,
                  CPos src,
                  int w = 1,
                  int h = 1);

    static constexpr char separator = '|'; // for IO operations

    // returns pointer to the expression stored at pos - doesnt evaluate the cell
    std::shared_ptr<CExpr> getCell(const CPos &pos) const;

private:
    std::unordered_map<CPos, std::shared_ptr<CExpr>, CPosHasher> m_table;

    // check if the expression at starts contains a cycle
    bool isCycle(const CPos &start) const;

    // creates an expression from input, if it cant -> exception
    std::shared_ptr<CExpr> setValue(std::string input);

    // overwrites cells in rectangle defined by dst, w, h in m_table by cellsToInsert.
    // If no cell exisits in cellsToInsert to replace it, the target cell is removed from m_table
    void insertCellsTo(const CPos &dst, const int w, const int h, const std::unordered_map<CPos, std::shared_ptr<CExpr>, CPosHasher> &cellsToInsert);

    // IO - all methods below return, true on success, false on fail, to read/write

    // saves pos to output stream os, in string form ie. row = 1, col = 1 => B1
    bool savePos(std::ostream &os, const CPos &pos) const;

    // saves expr to output stream os, the string begins with =
    bool saveExpr(std::ostream &os, const CExpr &expr) const;

    // first, loads count of char to save to out, then separator, then loads the amount of chars + seperator at end. the separator isnt included in the out string
    bool loadString(std::istream &is, std::string &out) const;

    // loads count chars from input stream and stores them to buffer
    bool loadChars(size_t count, std::string &buffer, std::istream &is) const;
};
