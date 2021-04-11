#include <iostream>
#include <chrono>
#include <functional>
#include <rnu/math/math.hpp>
#include <rnu/si.hpp>

int main(int argc, char** argv)
{
  namespace si = rnu::units;

  rnu::vec3 vector;
  rnu::vec<rnu::vec3, 3> vecs{ rnu::vec3{1, -1, 1}, rnu::vec3{1, 3, 1}, rnu::vec3{-3, -65, 1} };
  rnu::mat4 wuw{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
  auto x = rnu::sqrt(wuw);

  auto ayy = si::kilo<si::t_kelvin<int>>{ 1 } < si::kelvin{ 5 };


  __debugbreak();
  return 0;
}