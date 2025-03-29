#include "CSpreadsheet.hpp"

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
