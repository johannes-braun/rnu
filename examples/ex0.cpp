#include "vec.hpp"

#include <tuple>

int main(int argc, char** argv)
{ 
  rnu2::vec<float, 4> vector4(-3, -2, -1, 0);
  rnu2::vec<float, 8> vector8(vector4, 2, 3, 4, 0.3);
  rnu2::vec<int, 2> vector2(vector4);

  vector2 += 89 & vector2;
  vector2 *= 89 & -!~vector2;
  vector2 -= 89;
  vector2 /= 89;
  vector2 %= 89;
  vector2 >>= 89;
  vector2 <<= 89;
  vector2 |= 89;
  vector2 ^= 89;

  ++vector2;
  vector2++;

  auto a = apply(sqrt, vector2);

  return 0;
}