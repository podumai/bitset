#include <iostream>
#include <bitset>
#include <print>
#include "bitset.hpp"

std::int32_t main()
{
  bit::bitset<16> test (0x0f);
  std::println("{}", test.to_string());
  return 0;
}