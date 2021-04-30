#include <rnu/math/math.hpp>
#include <rnu/camera.hpp>
#include <tuple>

int main(int argc, char** argv)
{ 
  rnu::vec<float, 4> vector4(-3, -2, -1, 0);
  rnu::vec<float, 8> vector8(vector4, 2, 3, 4, 0.3);
  rnu::vec<int, 2> vector2(vector4);

  auto y = vector4 + vector4;

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

  rnu::mat<int, 2, 2> m2(1, 2, 3, 4);
  rnu::mat4 m4(m2);

  rnu::mat<float, 4, 4> m(vector4, vector4, vector4, vector4);
  rnu::mat<int, 4, 4> mi(0.5f, 0.5f, 0.5f, 1.0f);

    
  constexpr bool x = rnu::equivalent_types<rnu::mat<float, 4, 4>, rnu::vec<float, 4>>;

  auto res = m * vector4;
  auto res2 = vector4 * m;

  rnu::mat<double, 4, 4> p(m);

  m += 89;
  m += m;

  auto asd = !(m != 3);

  m *= 89;
  m -= 89;
  m /= 89;
  mi %= 89;
  mi >>= 89;
  mi <<= 89;
  mi |= 89;
  mi ^= 89;
  mi = 8 - m;
  mi = mi >> 8;
  mi = mi % 8;
  mi = mi << 8;
  mi = mi | 8;
  mi = mi ^ 8;
  mi = mi + 8;

  auto yay = rnu::apply_2d([&](float val, float val2) { return val * val2; }, 4, 4);

  return 0;
}