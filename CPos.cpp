#include "CPos.hpp"

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
