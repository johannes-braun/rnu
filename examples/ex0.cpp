#include <rnu/math/math.hpp>
#include <iostream>
#include <chrono>
#include <functional>
#include <rnu/math/math.hpp>
#include <rnu/si.hpp>


int main(int argc, char** argv)
{
  bool axxx = false;

  rnu::vec<rnu::units::meter, 2> len;
  rnu::vec2 a{ 23, 11 };

  auto eq = rnu::call_equal_to(len, len);

  auto b = !((len + len)/ sqrt(a) <= rnu::units::meter{ 10 });

  auto ay = normalize(a);

  //constexpr auto map = rnu::detail2::any_vector_type<rnu::mat3, rnu::vec3>;

  rnu::mat4 mup;
  mup = mup * mup;
  rnu::mat2 mep;
  rnu::vec2 ayxop = mep * ay;

  __debugbreak();
  return 0;
}