#include "CContent.hpp"

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
