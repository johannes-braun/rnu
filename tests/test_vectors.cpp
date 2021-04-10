#include "catch_amalgamated.hpp"
#include <rnu/math/math.hpp>
#include <rnu/si.hpp>

using namespace rnu;

TEST_CASE("Basic Vector Functions")
{
    SECTION("all_of, none_of, any_of")
    {
        vec3b all_true(true, true, true);
        vec3b all_false(false, false, false);
        vec3b one_true(false, true, false);
        bool implicit = all_true;

        REQUIRE(implicit); // Implicit bool conversion, equivalent to next expression.
        REQUIRE(all_of(all_true));
        REQUIRE(!all_of(one_true));
        REQUIRE(none_of(all_false));
        REQUIRE(!none_of(one_true));
        REQUIRE(any_of(one_true));
        REQUIRE(any_of(all_true));
        REQUIRE(!any_of(all_false));
    }

    SECTION("Initialization tests and equality comparators") {
        {
            vec2 a;
            REQUIRE((a[0] == 0 && a[1] == 0));
            vec4 b;
            REQUIRE((b[0] == 0 && b[1] == 0 && b[2] == 0 && b[3] == 0));
        }
        {
            vec3 a;
            REQUIRE((a[0] == 0 && a[1] == 0 && a[2] == 0));
            REQUIRE(all_of(a == 0));
        }
        {
            vec3 a(10);
            REQUIRE((a[0] == 10 && a[1] == 10 && a[2] == 10));
            REQUIRE(all_of(a == 10));
        }
        {
            vec3 a(1, 2, 3);
            REQUIRE((a[0] == 1, a[1] == 2, a[2] == 3));
            REQUIRE(all_of(a == vec3(1, 2, 3)));
        }
        {
            double init[4]{ 0.3, 0.4, 0.5, 0.6 };
            vec4d a(init);
            REQUIRE((a[0] == init[0] && a[1] == init[1] && a[2] == init[2] && a[3] == init[3]));
            vec4d b = a;
            REQUIRE(all_of(a == b));
        }
    }

    SECTION("Arithmetic operators")
    {
        vec3i a(1, 2, 3);
        vec3i b(3, 2, 1);
        vec3i c(1, 1, 1);

        REQUIRE(all_of(a + b == vec3i(4, 4, 4)));
        REQUIRE(all_of(a - b == vec3i(-2, 0, 2)));
        REQUIRE(all_of(a * b == vec3i(3, 4, 3)));
        REQUIRE(all_of(a / b == vec3i(0, 1, 3)));
        REQUIRE(all_of(a % b == vec3i(1, 0, 0)));
        REQUIRE(all_of(c << 2 == 4));
        c <<= 2;
        REQUIRE(all_of(c == 4));
        REQUIRE(all_of(c >> 1 == 2));
        REQUIRE(all_of(c++ == 4));
        REQUIRE(all_of(c == 5));
        REQUIRE(all_of(++c == 6));
        REQUIRE(all_of(c == 6));
        REQUIRE(all_of(-c == -6));
        REQUIRE(all_of(c < 7));
        REQUIRE(all_of(c > 5));
        REQUIRE(all_of(c <= 7));
        REQUIRE(all_of(c >= 5));
        REQUIRE(all_of(c <= 6));
        REQUIRE(all_of(c >= 6));
        REQUIRE(all_of(c != 42));
        REQUIRE(all_of(c-- == 6));
        REQUIRE(all_of(c == 5));
        REQUIRE(all_of(--c == 4));
        REQUIRE(all_of((c | 1) == 5));
        REQUIRE(all_of((c & 1) == 0));
        REQUIRE(all_of((c & 4) == 4));
        REQUIRE(all_of((c ^ 5) == 1));
        REQUIRE(all_of(~c == ~4));

        vec3b d(true, true, false);
        vec3b e(false, true, false);
        REQUIRE(all_of((d && e) == vec3b(false, true, false)));
        REQUIRE(all_of((d || e) == vec3b(true, true, false)));
        REQUIRE(all_of(!d == vec3b(false, false, true)));
        REQUIRE(all_of((d != e) == vec3b(true, false, false)));
    }

    SECTION("Element-wise")
    {
        vec3i a(1, 2, 3);
        vec3 b(1, 2, 3);

        REQUIRE(all_of(element_wise([](float f) {return static_cast<int>(f); }, b) == a));
        element_wise([](int& i) { i++; }, a);
        REQUIRE(all_of(a == vec3i(2, 3, 4)));
    }
}

namespace rnu::detail2 {
  template<typename A, typename B, typename C>
    struct value_type_impl<rnu::units::base::unit_t<A, B, C>> {
      using type = rnu::units::base::unit_t<A, B, C>;
    };
}

TEST_CASE("More of the same")
{
  rnu::vec<rnu::units::meter, 2> size;

  auto aax = size / rnu::units::meter{ 10 };

    const float error = 1e-6f;
    const auto axyco = 1.f;
    rnu::max(axyco, 2.f);

    vec3 a(1, 2, 3);
    vec3 b(4, 5, 6);
    vec3 c(5, 0, 0);
    vec3 d(-1, -2, 3);

    REQUIRE(bool(abs(d) == vec3(1, 2, 3)));

    REQUIRE(bool(dot(a, b) == 1 * 4 + 2 * 5 + 3 * 6));
    REQUIRE(bool(abs(norm(a) - std::sqrtf(1 * 1 + 2 * 2 + 3 * 3)) < error));
    //REQUIRE(bool(abs(normalize(c) - vec3(1, 0, 0)) < error));
}