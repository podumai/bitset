#include <iostream>
#include <bitset>
#include <print>
#include "bitset.hpp"

std::int32_t main()
{
  bit::bitset<16> test (0xff);
  
  std::cout << (test >> 20) << std::endl;

  return 0;
}