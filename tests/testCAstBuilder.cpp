#include "CContent.hpp"
#include "CAstBuilder.hpp"
#include <cassert>

int main()
{
    std::cout << "START" << std::endl;
    std::ostringstream oss;
    // CASTBUILDER
    CAstBuilder b;
    // 30^2 - (20 + 30) / (-40 * 50)
    parseExpression("=30^2 - (20 + 30) / (-40 * 50)", b);
    std::cout << *b.m_stack.top() << std::endl;
    std::cout << b.m_stack.top().get()->eval() << std::endl;
    CAstBuilder a;
    parseExpression("=(((30) ^ (2)) - ((20 + 30) / (( - 40) * 50)))", a);
    std::cout << *a.m_stack.top() << std::endl;
    CAstBuilder c;
    parseExpression("=owritjh", c);
    std::cout << *c.m_stack.top() << std::endl;
}