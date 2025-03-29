
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

CPos::CPos(std::string_view str)
{
    std::istringstream input{std::string(str)};
    parseCol(input);
    parseRow(input);
}

void CPos::shiftBy(int x, int y)
{
    if (!m_isAbsRow)
    {
        m_row += x;
    }
    if (!m_isAbsCol)
    {
        m_col += y;
    }
}

bool CPos::operator==(const CPos &other) const
{
    return m_col == other.m_col && m_row == other.m_row;
}

bool CPos::operator<(const CPos &other) const
{
    if (m_row <= other.m_row)
    {
        if (m_row == other.m_row)
        {
            return m_col < other.m_col;
        }
        return true;
    }
    else
    {
        return false;
    }
}

void CPos::parseCol(std::istringstream &input)
{
    if (input.peek() == '$')
    {
        m_isAbsCol = true;
        input.get();
    }
    char ch;
    std::vector<size_t> digits;
    while (std::isalpha(ch = input.peek()))
    {
        digits.push_back(letterToInt(ch));
        input.get();
    }
    if ((std::isdigit(ch) || ch == '$') && digits.size() > 0)
    {
        m_col = getValue(digits, 26) - 1;
    }
    else
    {
        throw std::invalid_argument("unknown char in COL");
    }
}

void CPos::parseRow(std::istringstream &input)
{
    if (input.peek() == '$')
    {
        m_isAbsRow = true;
        input.get();
    }
    input >> m_row;
    if (!input.eof())
    {
        throw std::invalid_argument("unknown char in ROW");
    }
}

size_t CPos::letterToInt(char ch) const
{
    return (std::tolower(ch) - 'a') + 1;
}

char CPos::intToLetter(int i) const
{
    return i + 'A' - 1;
}

size_t CPos::getValue(const std::vector<size_t> &digits, const size_t base) const
{
    size_t result = 0;
    for (size_t i = 0; i < digits.size(); i++)
    {
        result += digits.at(i) * pow(base, i);
    }
    return result;
}

std::string CPos::getCol() const
{
    std::string result;
    size_t num = m_col + 1;
    size_t base = 26;
    while (num > 0)
    {
        result += intToLetter(num % base);
        num /= base;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

void CPos::print(std::ostream &os) const
{
    if (m_isAbsCol)
        os << "$";
    os << getCol();
    if (m_isAbsRow)
        os << "$";
    os << m_row;
}

std::ostream &operator<<(std::ostream &os, const CPos &pos)
{
    pos.print(os);
    return os;
}

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

CContent::CContent() : m_value(CValue()) {} // default is monostate
CContent::CContent(const CValue &value) : m_value(value) {}

CContent CContent::toExp(const CContent &power) const
{
    if (this->isDouble() && power.isDouble())
    {
        return CContent(CValue(std::pow(std::get<double>(m_value), std::get<double>(power.m_value))));
    }
    return CContent();
}

CContent CContent::operator-() const
{
    if (this->isDouble())
    {
        return CContent(CValue(-(std::get<double>(m_value))));
    }
    return CContent();
}

// must be same type, otherwise UB
int CContent::compare(const CContent &other) const
{
    if (this->isDouble())
    {
        double x = std::get<double>(m_value);
        double y = std::get<double>(other.m_value);
        if (x < y)
        {
            return -1;
        }
        else if (x == y)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else if (this->isString())
    {
        std::string x = std::get<std::string>(m_value);
        std::string y = std::get<std::string>(other.m_value);
        return x.compare(y);
    }
    return 0;
}

bool CContent::isDouble() const
{
    return std::holds_alternative<double>(m_value);
}

bool CContent::isString() const
{
    return std::holds_alternative<std::string>(m_value);
}

bool CContent::isMonostate() const
{
    return std::holds_alternative<std::monostate>(m_value);
}

CContent operator+(const CContent &lhs, const CContent &rhs)
{
    if (lhs.isMonostate() || rhs.isMonostate())
    {
        return CContent();
    }
    else if (lhs.isString() && rhs.isString())
    {
        return CContent(CValue(std::get<std::string>(lhs.m_value) + std::get<std::string>(rhs.m_value)));
    }
    else if (lhs.isDouble() && rhs.isDouble())
    {
        return CContent(CValue(std::get<double>(lhs.m_value) + std::get<double>(rhs.m_value)));
    }
    else if (lhs.isString() && rhs.isDouble())
    {
        return CContent(CValue(std::get<std::string>(lhs.m_value) + std::to_string(std::get<double>(rhs.m_value))));
    }
    else if (lhs.isDouble() && rhs.isString())
    {
        return CContent(CValue(std::to_string(std::get<double>(lhs.m_value)) + std::get<std::string>(rhs.m_value)));
    }
    else
    {
        return CContent();
    }
}

CContent operator-(const CContent &lhs, const CContent &rhs)
{
    if (lhs.isDouble() && rhs.isDouble())
    {
        return lhs + (-rhs);
    }
    return CContent();
}

CContent operator*(const CContent &lhs, const CContent &rhs)
{
    if (lhs.isDouble() && rhs.isDouble())
    {
        return CContent(CValue(std::get<double>(lhs.m_value) * std::get<double>(rhs.m_value)));
    }
    return CContent();
}

CContent operator/(const CContent &lhs, const CContent &rhs)
{
    if (lhs.isDouble() && rhs.isDouble())
    {
        if (std::get<double>(rhs.m_value) == 0)
        {
            return CContent();
        }
        return CContent(CValue(std::get<double>(lhs.m_value) / std::get<double>(rhs.m_value)));
    }
    return CContent();
}

CContent operator<(const CContent &lhs, const CContent &rhs)
{
    if ((lhs.isDouble() && rhs.isDouble()) || (lhs.isString() && rhs.isString()))
    {
        return lhs.compare(rhs) < 0 ? CContent(CValue(1.)) : CContent(CValue(0.));
    }
    return CContent();
}

CContent operator>(const CContent &lhs, const CContent &rhs)
{
    if ((lhs.isDouble() && rhs.isDouble()) || (lhs.isString() && rhs.isString()))
    {
        return lhs.compare(rhs) > 0 ? CContent(CValue(1.)) : CContent(CValue(0.));
    }
    return CContent();
}

CContent operator<=(const CContent &lhs, const CContent &rhs)
{
    if ((lhs.isDouble() && rhs.isDouble()) || (lhs.isString() && rhs.isString()))
    {
        return lhs.compare(rhs) <= 0 ? CContent(CValue(1.)) : CContent(CValue(0.));
    }
    return CContent();
}

CContent operator>=(const CContent &lhs, const CContent &rhs)
{
    if ((lhs.isDouble() && rhs.isDouble()) || (lhs.isString() && rhs.isString()))
    {
        return lhs.compare(rhs) >= 0 ? CContent(CValue(1.)) : CContent(CValue(0.));
    }
    return CContent();
}

CContent operator==(const CContent &lhs, const CContent &rhs)
{
    if ((lhs.isDouble() && rhs.isDouble()) || (lhs.isString() && rhs.isString()))
    {
        return lhs.compare(rhs) == 0 ? CContent(CValue(1.)) : CContent(CValue(0.));
    }
    return CContent();
}

CContent operator!=(const CContent &lhs, const CContent &rhs)
{
    if ((lhs.isDouble() && rhs.isDouble()) || (lhs.isString() && rhs.isString()))
    {
        return lhs.compare(rhs) != 0 ? CContent(CValue(1.)) : CContent(CValue(0.));
    }
    return CContent();
}

void CContent::printString(std::ostream &os) const
{
    const std::string original = std::get<std::string>(m_value);
    for (size_t i = 0; i < original.size(); i++)
    {
        if (original.at(i) == '"')
        {
            os << original.at(i) << '"';
        }
        else
        {
            os << original.at(i);
        }
    }
}

std::ostream &operator<<(std::ostream &os, const CContent &content)
{
    if (content.isDouble())
    {
        os << std::get<double>(content.m_value);
    }
    else if (content.isString())
    {

        os << "\"";
        content.printString(os);
        os << "\"";
    }
    else
    {
        os << "std::monostate";
    }
    return os;
}


using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;


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

// Reference

Reference::Reference(const std::string &pos) : m_pos(pos) {}

std::shared_ptr<CExpr> Reference::clone() const
{
    return std::make_shared<Reference>(*this);
}

CContent Reference::eval(const CSpreadsheet &sheet) const
{
    return sheet.getCell(m_pos)->eval(sheet);
}

void Reference::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    dependencies.insert(m_pos);
}

void Reference::updateRef(int i, int j)
{
    m_pos.shiftBy(i, j);
}

void Reference::print(std::ostream &os) const
{
    os << m_pos;
}

// Literal

Literal::Literal(CContent val) : m_value(val) {}

std::shared_ptr<CExpr> Literal::clone() const
{
    return std::make_shared<Literal>(*this);
}

CContent Literal::eval(const CSpreadsheet &sheet) const
{
    sheet.capabilities(); // does nothing, however compiler doesnt complain about unused param
    return m_value;
}

void Literal::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    dependencies.begin(); // does nothing, however compiler doesnt complain about unused param
    return;
}

void Literal::updateRef(int i, int j)
{
    CPos("A0").shiftBy(i, j); // does nothing, however compiler doesnt complain about unused param
    return;
}

void Literal::print(std::ostream &os) const
{
    os << m_value;
}

// Addition

Addition::Addition(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : CExpr(), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> Addition::clone() const
{
    return std::make_shared<Addition>(Addition(m_Lhs->clone(), m_Rhs->clone()));
}

void Addition::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << "+";
    m_Rhs->print(os);
    os << ")";
}

CContent Addition::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) + m_Rhs->eval(sheet);
}

void Addition::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void Addition::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

// Multiplication

Multiplication::Multiplication(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> Multiplication::clone() const
{
    return std::make_shared<Multiplication>(Multiplication(m_Lhs->clone(), m_Rhs->clone()));
}

CContent Multiplication::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) * m_Rhs->eval(sheet);
}

void Multiplication::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void Multiplication::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void Multiplication::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << "*";
    m_Rhs->print(os);
    os << ")";
}

// Division

Division::Division(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> Division::clone() const
{
    return std::make_shared<Division>(Division(m_Lhs->clone(), m_Rhs->clone()));
}

CContent Division::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) / m_Rhs->eval(sheet);
}

void Division::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void Division::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void Division::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << "/";
    m_Rhs->print(os);
    os << ")";
}

// Subtraction

Subtraction::Subtraction(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> Subtraction::clone() const
{
    return std::make_shared<Subtraction>(Subtraction(m_Lhs->clone(), m_Rhs->clone()));
}

CContent Subtraction::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) - m_Rhs->eval(sheet);
}
void Subtraction::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void Subtraction::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void Subtraction::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << "-";
    m_Rhs->print(os);
    os << ")";
}

// Exponentiation

Exponentiation::Exponentiation(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> Exponentiation::clone() const
{
    return std::make_shared<Exponentiation>(Exponentiation(m_Lhs->clone(), m_Rhs->clone()));
}

CContent Exponentiation::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet).toExp(m_Rhs->eval(sheet));
}
void Exponentiation::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void Exponentiation::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void Exponentiation::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << "^";
    m_Rhs->print(os);
    os << ")";
}

// Negation

Negation::Negation(std::shared_ptr<CExpr> rhs) : m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> Negation::clone() const
{
    return std::make_shared<Negation>(Negation(m_Rhs->clone()));
}

CContent Negation::eval(const CSpreadsheet &sheet) const
{
    return -(m_Rhs->eval(sheet));
}

void Negation::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Rhs->getDependencies(dependencies);
}

void Negation::updateRef(int i, int j)
{
    m_Rhs->updateRef(i, j);
}

void Negation::print(std::ostream &os) const
{
    os << "(";
    os << "-";
    m_Rhs->print(os);
    os << ")";
}

// LessThan

LessThan::LessThan(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> LessThan::clone() const
{
    return std::make_shared<LessThan>(LessThan(m_Lhs->clone(), m_Rhs->clone()));
}

CContent LessThan::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) < m_Rhs->eval(sheet);
}

void LessThan::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void LessThan::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void LessThan::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << "<";
    m_Rhs->print(os);
    os << ")";
}

// GreaterThan

GreaterThan::GreaterThan(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> GreaterThan::clone() const
{
    return std::make_shared<GreaterThan>(GreaterThan(m_Lhs->clone(), m_Rhs->clone()));
}

CContent GreaterThan::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) > m_Rhs->eval(sheet);
}

void GreaterThan::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void GreaterThan::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void GreaterThan::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << ">";
    m_Rhs->print(os);
    os << ")";
}

// Equal

Equal::Equal(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> Equal::clone() const
{
    return std::make_shared<Equal>(Equal(m_Lhs->clone(), m_Rhs->clone()));
}

CContent Equal::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) == m_Rhs->eval(sheet);
}

void Equal::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void Equal::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void Equal::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << "=";
    m_Rhs->print(os);
    os << ")";
}

// NotEqual

NotEqual::NotEqual(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> NotEqual::clone() const
{
    return std::make_shared<NotEqual>(NotEqual(m_Lhs->clone(), m_Rhs->clone()));
}

CContent NotEqual::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) != m_Rhs->eval(sheet);
}

void NotEqual::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void NotEqual::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void NotEqual::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << "<>";
    m_Rhs->print(os);
    os << ")";
}

// LessEqual

LessEqual::LessEqual(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> LessEqual::clone() const
{
    return std::make_shared<LessEqual>(LessEqual(m_Lhs->clone(), m_Rhs->clone()));
}

CContent LessEqual::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) <= m_Rhs->eval(sheet);
}

void LessEqual::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void LessEqual::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void LessEqual::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << "<=";
    m_Rhs->print(os);
    os << ")";
}

// GreaterEqual

GreaterEqual::GreaterEqual(std::shared_ptr<CExpr> lhs, std::shared_ptr<CExpr> rhs)
    : m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {}

std::shared_ptr<CExpr> GreaterEqual::clone() const
{
    return std::make_shared<GreaterEqual>(GreaterEqual(m_Lhs->clone(), m_Rhs->clone()));
}

CContent GreaterEqual::eval(const CSpreadsheet &sheet) const
{
    return m_Lhs->eval(sheet) >= m_Rhs->eval(sheet);
}

void GreaterEqual::getDependencies(std::unordered_set<CPos, CPosHasher> &dependencies) const
{
    m_Lhs->getDependencies(dependencies);
    m_Rhs->getDependencies(dependencies);
}

void GreaterEqual::updateRef(int i, int j)
{
    m_Lhs->updateRef(i, j);
    m_Rhs->updateRef(i, j);
}

void GreaterEqual::print(std::ostream &os) const
{
    os << "(";
    m_Lhs->print(os);
    os << ">=";
    m_Rhs->print(os);
    os << ")";
}

// CSpreadsheet

CSpreadsheet::CSpreadsheet() {}

bool CSpreadsheet::load(std::istream &is)
{
    if (!is.good())
        return false;

    m_table.clear();
    size_t cellCount = 0;
    if (!(is >> cellCount))
        return false;

    if (is.get() != separator)
        return false;

    for (size_t i = 0; i < cellCount; i++)
    {
        std::string posInput;
        if (!loadString(is, posInput))
            return false;

        try
        {
            CPos x(posInput);
        }
        catch (std::invalid_argument &e)
        {
            return false;
        }

        std::string exprInput;
        if (!loadString(is, exprInput))
            return false;

        if (!setCell(CPos(posInput), exprInput))
            return false;
    }
    if (is.get() != EOF)
        return false;

    return true;
}

bool CSpreadsheet::loadChars(size_t count, std::string &buffer, std::istream &is) const
{
    char ch;
    for (size_t i = 0; i < count; i++)
    {
        if (!is.get(ch))
        {
            return false;
        }
        else
        {
            buffer += ch;
        }
    }
    return true;
}

bool CSpreadsheet::loadString(std::istream &is, std::string &out) const
{
    size_t size = 0;
    if (!(is >> size))
        return false;
    if (is.get() != separator)
        return false;
    if (!loadChars(size, out, is))
        return false;
    if (is.get() != separator)
        return false;
    return true;
}

bool CSpreadsheet::save(std::ostream &os) const
{
    //[cellCount]|[posSize]|[pos]|[exprSize]=[expr]|...|

    if (!(os << m_table.size()))
        return false;
    if (!(os << separator))
        return false;
    for (auto it = m_table.begin(); it != m_table.end(); it++)
    {
        if (!savePos(os, it->first))
            return false;
        if (!saveExpr(os, *(it->second.get())))
            return false;
    }
    return os.good();
}

bool CSpreadsheet::savePos(std::ostream &os, const CPos &pos) const
{
    std::ostringstream oss;
    oss << pos;
    std::string posString = oss.str();
    return ((os << posString.size()) && (os << separator) && (os << posString) && (os << separator));
}

bool CSpreadsheet::saveExpr(std::ostream &os, const CExpr &expr) const
{
    std::ostringstream oss;
    oss << expr;
    std::string resultExpr = oss.str();
    return ((os << resultExpr.size() + 1) && (os << separator) && (os << "=") && (os << resultExpr) && (os << separator));
}

bool CSpreadsheet::setCell(CPos pos, std::string contents)
{
    std::shared_ptr<CExpr> cell;
    try
    {
        cell = setValue(contents);
    }
    catch (std::invalid_argument &e)
    {
        return false;
    }
    if (m_table.contains(pos))
    {
        m_table.find(pos)->second = cell;
    }
    m_table.insert({pos, cell});
    return true;
}

bool CSpreadsheet::isCycle(const CPos &start) const
{
    const int ENTER = 0;
    const int EXIT = 1;
    std::vector<std::pair<CPos, int>> stack;
    std::unordered_set<CPos, CPosHasher> visiting;
    std::unordered_set<CPos, CPosHasher> visited;
    stack.push_back({start, ENTER});
    while (!stack.empty())
    {
        std::pair<CPos, int> current = stack.back();
        stack.pop_back();
        if (current.second == EXIT)
        {
            visiting.erase(current.first);
            visited.insert(current.first);
        }
        else
        {
            visiting.insert(current.first);
            stack.push_back({current.first, EXIT});
            std::unordered_set<CPos, CPosHasher> dependencies;
            getCell(current.first)->getDependencies(dependencies);
            for (const auto &nodeTopush : dependencies)
            {
                if (visiting.contains(nodeTopush))
                {
                    return true;
                }
                else if (!visiting.contains(nodeTopush) && !visited.contains(nodeTopush))
                {
                    stack.push_back({nodeTopush, ENTER});
                }
            }
        }
    }
    return false;
}

std::shared_ptr<CExpr> CSpreadsheet::setValue(std::string input)
{
    CAstBuilder builder;
    parseExpression(input, builder);
    return builder.getResult();
}

CValue CSpreadsheet::getValue(CPos pos)
{

    std::shared_ptr<CExpr> cellToEval = getCell(pos);
    if (isCycle(pos))
    {
        return CValue();
    }
    return cellToEval->eval(*this).m_value;
}

void CSpreadsheet::copyRect(CPos dst, CPos src, int w, int h)
{
    std::unordered_map<CPos, std::shared_ptr<CExpr>, CPosHasher> cellsToInsert; // copies of cells to be inserted
    std::pair<int, int> shift = {dst.m_row - src.m_row, dst.m_col - src.m_col};
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            // old pos
            CPos from = src;
            from.shiftBy(i, j);

            // new pos
            CPos to = dst;
            to.shiftBy(i, j);

            if (m_table.contains(from))
            {
                std::shared_ptr<CExpr> copyOfExpr = m_table.find(from)->second->clone();
                copyOfExpr->updateRef(shift.first, shift.second);
                cellsToInsert.insert({to, copyOfExpr});
            }
        }
    }
    insertCellsTo(dst, w, h, cellsToInsert);
}

void CSpreadsheet::insertCellsTo(const CPos &dst, const int w, const int h, const std::unordered_map<CPos, std::shared_ptr<CExpr>, CPosHasher> &cellsToInsert)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            CPos to = dst;
            to.shiftBy(i, j);
            if (cellsToInsert.contains(to))
            {
                m_table[to] = (cellsToInsert.find(to))->second; // insert/rewrite to
            }
            else
            {
                m_table.erase(to); // delete = paste empty cell
            }
        }
    }
}

std::shared_ptr<CExpr> CSpreadsheet::getCell(const CPos &pos) const
{
    auto it = m_table.find(pos);
    if (it == m_table.end())
    {
        CAstBuilder builder;
        builder.valNull();
        return builder.getResult();
    }
    return it->second;
}

// CAstBuilder
void CAstBuilder::opAdd()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<Addition>(lhs, rhs));
}

void CAstBuilder::opSub()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<Subtraction>(lhs, rhs));
}

void CAstBuilder::opMul()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<Multiplication>(lhs, rhs));
}

void CAstBuilder::opDiv()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<Division>(lhs, rhs));
}

void CAstBuilder::opPow()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<Exponentiation>(lhs, rhs));
}

void CAstBuilder::opNeg()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<Negation>(rhs));
}

void CAstBuilder::opEq()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<Equal>(lhs, rhs));
}

void CAstBuilder::opNe()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<NotEqual>(lhs, rhs));
}

void CAstBuilder::opLt()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<LessThan>(lhs, rhs));
}

void CAstBuilder::opLe()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<LessEqual>(lhs, rhs));
}

void CAstBuilder::opGt()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<GreaterThan>(lhs, rhs));
}

void CAstBuilder::opGe()
{
    std::shared_ptr<CExpr> rhs = m_stack.top();
    m_stack.pop();
    std::shared_ptr<CExpr> lhs = m_stack.top();
    m_stack.pop();
    m_stack.push(std::make_shared<GreaterEqual>(lhs, rhs));
}

void CAstBuilder::valNumber(double val)
{
    m_stack.push(std::make_shared<Literal>(CContent(CValue(val))));
}

void CAstBuilder::valString(std::string val)
{
    m_stack.push(std::make_shared<Literal>(CContent(CValue(val))));
}

void CAstBuilder::valNull()
{
    m_stack.push(std::make_shared<Literal>(CContent()));
}

void CAstBuilder::valReference(std::string val)
{
    std::shared_ptr<Reference> x = std::make_shared<Reference>(val);
    m_stack.push(x);
}

void CAstBuilder::valRange(std::string val)
{
    val.size();
}

void CAstBuilder::funcCall(std::string fnName, int paramCount)
{
    fnName.size();
    fnName.append(std::to_string(paramCount));
}

std::shared_ptr<CExpr> CAstBuilder::getResult() const
{
    return m_stack.top();
}
