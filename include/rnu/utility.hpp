#pragma once

namespace rnu
{
  template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
}