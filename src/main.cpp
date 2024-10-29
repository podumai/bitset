#include <iostream>
#include <bitset>
#include <print>
#include "bitset.hpp"

std::int32_t main()
{
  bit::bitset<16> test (0xff);
  
  for (auto iterator : test)
    std::cout << iterator;
  std::cout << '\n';
  return 0;
}