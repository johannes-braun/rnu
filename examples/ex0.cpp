#include <rnu/math/math.hpp>
#include <iostream>
#include <chrono>
#include <functional>
#include <rnu/math/operators.hpp>

int main(int argc, char** argv)
{
  bool axxx = false;

  rnu::vec2 a{ 23, 11 };

  auto b = a + a - a;
  auto iy = a < 16;
  b += a;

  rnu::call_plus_assign(a, a);

  ++a;
  a++;

  auto ay1 = rnu::step(a, 0);
  auto ay2 = rnu::smoothstep(0, 10, a);
  auto ay3 = rnu::clamp(a, 0, 1);
  auto ay4 = rnu::min(a, 0);
  auto ay5 = rnu::max(a, 2);
  auto ay6 = rnu::pow(a, 4);
  auto ay7 = rnu::step(a, 0);
  auto ay13 = rnu::sin(a);
  auto ay55 = rnu::cos(a);
  auto ay71 = rnu::tan(a);
  auto ay72 = rnu::asin(a);
  auto ay75 = rnu::acos(a);
  auto ay734 = rnu::atan(a);
  auto ay7122 = rnu::atan2(a.y, a.x);

  auto ay7122y = rnu::isinf(a);
  auto ay7222 = rnu::isnan(a);
  auto ay75422 = rnu::abs(a);
  auto ay7124 = rnu::sqrt(a);
  auto ay71412 = rnu::sign(a);
  auto ay71212 = rnu::degrees(a);
  auto ay715512 = rnu::radians(a);

  return 0;
}