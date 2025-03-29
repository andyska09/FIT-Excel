#!/bin/bash
#ignores all includes, pragma, and constexpr unsigned for symbolic constants in CSpreadsheet.hpp, which are already defined on progtest
grep -vEh '^(#include|#pragma|constexpr unsigned)' CPos.hpp CPos.cpp CContent.hpp CContent.cpp CSpreadsheet.hpp CSpreadsheet.cpp > submission/all_in_one.cpp