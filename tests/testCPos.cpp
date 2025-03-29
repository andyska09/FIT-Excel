#include "CPos.hpp"
#include <cassert>

int main()
{
    CPos x0("A23");
    assert(x0.m_col == 1);
    assert(!x0.m_isAbsCol);
    // row
    assert(x0.m_row == 23);
    assert(!x0.m_isAbsRow);
    CPos x1("ABC23");
    assert(x1.m_col == 731);
    assert(!x1.m_isAbsCol);

    assert(x1.m_row == 23);
    assert(!x1.m_isAbsRow);
    CPos x2("AAA23");
    assert(x2.m_col == 703);

    assert(x2.m_row == 23);
    assert(!x2.m_isAbsRow);
    CPos x3("$B23");
    assert(x3.m_col == 2);
    assert(x3.m_isAbsCol);

    assert(x3.m_row == 23);
    assert(!x3.m_isAbsRow);
    CPos x4("B$23");
    assert(x4.m_col == 2);
    assert(!x4.m_isAbsCol);

    assert(x4.m_row == 23);
    assert(x4.m_isAbsRow);

    CPos x5("$B$1234");
    assert(x5.m_col == 2);
    assert(x5.m_isAbsCol);

    assert(x5.m_row == 1234);
    assert(x5.m_isAbsRow);
    std::cout << "PASSED CORRECT INPUTS" << std::endl;
    try
    {
        CPos x6("$B$1234.;[]");
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << e.what() << std::endl;
    }
    try
    {
        CPos x10("B[]");
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << e.what() << std::endl;
    }
    try
    {
        CPos x11("0314985");
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << e.what() << std::endl;
    }
    try
    {
        CPos x11("[][][]][]");
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}