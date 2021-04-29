#pragma once

#ifndef RNU_SI_HPP
#define RNU_SI_HPP

#include <bit>
#include <tuple>
#include <numeric>
#include <compare>

namespace rnu::units
{
    namespace base
    {
        template<size_t Nom, size_t Den = 1>
        struct fraction_t {
            using value_type = size_t;
            using inverse = fraction_t<Den, Nom>;

            constexpr static auto nominator = Nom;
            constexpr static auto denominator = Den;
            template<std::floating_point Float> constexpr static Float as = static_cast<Float>(Nom) / static_cast<Float>(Den);

            template<typename T> constexpr static auto mul(T value) {
              using float_type = std::conditional_t<std::greater<size_t>{}(sizeof(std::decay_t<T>), sizeof(float)), double, float>;

              return float_type(value) * float_type(Nom) / float_type(Den);
            }
        };

        template<size_t Nom, size_t Den, size_t Gcd = std::gcd(Nom, Den)>
        using reduced_fraction = fraction_t<Nom / Gcd, Den / Gcd>;

        template<typename F0, typename F1>
        using fraction_mul = reduced_fraction<F0::nominator * F1::nominator, F0::denominator* F1::denominator>;

        template<typename Front, typename... F>
        constexpr auto muls()
        {
            if constexpr (sizeof...(F) == 0)
                return Front{};
            else
                return fraction_mul<Front, decltype(muls<F...>())>{};
        }

        template<typename... Fs>
        using fraction_mul_var = decltype(muls<Fs...>());

        using nulltuple = std::tuple<>;

        template<typename Nominator, typename Denominator = nulltuple>
        struct descriptor_t
        {
            using nominator = Nominator;
            using denominator = Denominator;

            using inverse = descriptor_t<Denominator, Nominator>;
        };

        template<typename T>
        concept descriptor_type = requires {
            typename T::nominator;
            typename T::denominator;
        };

        namespace detail {
            template<typename Query>
            constexpr auto tuple_remove_one(std::tuple<>)
            {
                return std::pair<std::false_type, std::tuple<>>({}, {});
            }

            template<typename Query, typename T1, typename... T>
            constexpr auto tuple_remove_one(std::tuple<T1, T...>)
            {
                if constexpr (std::is_same_v<Query, T1>)
                {
                    return std::pair<std::true_type, std::tuple<T...>>({}, {});
                }
                else
                {
                    const auto p = tuple_remove_one<Query, T...>(std::tuple<T...>{});
                    using cat = decltype(std::tuple_cat(std::tuple<T1>{}, decltype(p.second){}));
                    return std::pair<decltype(p.first), cat>({}, {});
                }
            }
            template<typename Query, typename Tuple>
            struct tuple_remove_t
            {
                using pair_type = decltype(tuple_remove_one<Query>(std::declval<Tuple>()));
                constexpr static bool did_remove = pair_type::first_type::value;
                using reduced_tuple = typename pair_type::second_type;
            };

            template<typename... Tup2>
            constexpr auto fract_reduce(std::tuple<> t1, std::tuple<Tup2...> t2)
            {
                return std::pair<std::tuple<>, std::tuple<Tup2...>>{};
            }

            template<typename Tup1_0, typename... Tup1, typename... Tup2>
            constexpr auto fract_reduce(std::tuple<Tup1_0, Tup1...> t1, std::tuple<Tup2...> t2)
            {
                using reduced = tuple_remove_t<Tup1_0, std::tuple<Tup2...>>;
                using recurse = decltype(fract_reduce(std::tuple<Tup1...>{}, typename reduced::reduced_tuple{}));
                if constexpr (reduced::did_remove)
                {
                    return recurse{};
                }
                else
                {
                    using first = decltype(std::tuple_cat(std::tuple<Tup1_0>{}, typename recurse::first_type{}));
                    return std::pair<first, typename recurse::second_type>{};
                }
            }

            template<descriptor_type... descriptors>
            constexpr auto multiply_and_normalize() {
                using nom_cat = decltype(std::tuple_cat(typename descriptors::nominator{}...));
                using den_cat = decltype(std::tuple_cat(typename descriptors::denominator{}...));

                using reduced = decltype(fract_reduce(nom_cat{}, den_cat{}));
                return descriptor_t<typename reduced::first_type, typename reduced::second_type>{};
            }
        }

        template<descriptor_type Descriptor, typename Fraction, typename Scalar>
        struct unit_t;

        template<typename Unit>
        struct dissolve_unit
        {
          using type = Unit;
        };

        template<descriptor_type Descriptor, typename Fraction, typename Scalar>
        struct dissolve_unit<unit_t<Descriptor, Fraction, Scalar>>
        {
          using type = unit_t<Descriptor, Fraction, Scalar>;
        };

        template<typename Scalar>
        struct dissolve_unit<unit_t<descriptor_t<nulltuple, nulltuple>, fraction_t<1, 1>, Scalar>>
        {
          using type = Scalar;
        };

        template<typename T>
        using dissolve_unit_t = typename dissolve_unit<T>::type;

        template<descriptor_type Descriptor, typename Fraction, typename Scalar>
        struct unit_t
        {
            using fraction = Fraction;
            using scalar_type = Scalar;
            using descriptor = Descriptor;
            using inverse = unit_t<typename descriptor::inverse, typename fraction::inverse, Scalar>;
            scalar_type value;

            constexpr unit_t(scalar_type value = 0.0)
                : value(value) {}

            template<typename OtherFraction, std::convertible_to<scalar_type> OtherScalar>
            constexpr unit_t(unit_t<Descriptor, OtherFraction, OtherScalar> other) : value(OtherFraction::template mul<scalar_type>((typename Fraction::inverse)::template mul<scalar_type>(other.value))) {

            }

            template<typename OtherScalar>
            constexpr auto to() const {
                return unit_t<descriptor, fraction, OtherScalar>(*this);
            }

            template<descriptor_type OtherDescriptor, typename OtherFraction, std::convertible_to<scalar_type> OtherScalar>
            friend constexpr auto operator*(unit_t lhs, unit_t<OtherDescriptor, OtherFraction, OtherScalar> rhs)
            {
                using return_descriptor = decltype(detail::multiply_and_normalize<Descriptor, OtherDescriptor>());
                return dissolve_unit_t<unit_t<return_descriptor, reduced_fraction<Fraction::nominator* OtherFraction::nominator, Fraction::denominator* OtherFraction::denominator>, std::common_type_t<OtherScalar, scalar_type>>>{
                    lhs.value* rhs.value
                };
            }
            template<std::convertible_to<scalar_type> OtherScalar>
            friend constexpr auto operator*(unit_t lhs, OtherScalar rhs)
            {
                return unit_t<Descriptor, Fraction, std::common_type_t<OtherScalar, scalar_type>>{ lhs.value * rhs };
            }
            template<std::convertible_to<scalar_type> OtherScalar>
            friend constexpr auto operator*(OtherScalar rhs, unit_t lhs)
            {
                return unit_t<Descriptor, Fraction, std::common_type_t<OtherScalar, scalar_type>>{ rhs * lhs.value };
            }
            template<descriptor_type OtherDescriptor, typename OtherFraction, std::convertible_to<scalar_type> OtherScalar>
            friend constexpr auto operator/(unit_t lhs, unit_t<OtherDescriptor, OtherFraction, OtherScalar> rhs)
            {
                using return_descriptor = decltype(detail::multiply_and_normalize<Descriptor, typename OtherDescriptor::inverse>());
                return dissolve_unit_t<unit_t<return_descriptor, reduced_fraction<Fraction::nominator* OtherFraction::denominator, Fraction::denominator* OtherFraction::nominator>, std::common_type_t<OtherScalar, scalar_type>>>{
                    lhs.value / rhs.value
                };
            }
            template<std::convertible_to<scalar_type> OtherScalar>
            friend constexpr auto operator/(unit_t lhs, OtherScalar rhs)
            {
                return unit_t<Descriptor, Fraction, std::common_type_t<OtherScalar, scalar_type>>{ lhs.value / rhs };
            }
            template<std::convertible_to<scalar_type> OtherScalar>
            friend constexpr auto operator/(scalar_type rhs, unit_t lhs)
            {
                return unit_t<typename Descriptor::inverse, typename Fraction::inverse, std::common_type_t<OtherScalar, scalar_type>>{ rhs / lhs.value };
            }

            template<typename OtherFraction, std::convertible_to<scalar_type> OtherScalar>
            friend constexpr auto operator+(unit_t lhs, unit_t<Descriptor, OtherFraction, OtherScalar> rhs)
            {
                constexpr auto factor = OtherFraction::template as<scalar_type> / Fraction::template as<scalar_type>;
                return unit_t<Descriptor, Fraction, std::common_type_t<OtherScalar, scalar_type>>{
                    lhs.value + rhs.value * factor
                };
            }
            template<typename OtherFraction, std::convertible_to<scalar_type> OtherScalar>
            friend constexpr auto operator-(unit_t lhs, unit_t<Descriptor, OtherFraction, OtherScalar> rhs)
            {
              using ty = std::common_type_t<OtherScalar, scalar_type>;
                constexpr auto factor = OtherFraction::template as<ty> / Fraction::template as<ty>;
                return unit_t<Descriptor, Fraction, ty>{
                    lhs.value - rhs.value * factor
                };
            }

            template<typename B> requires requires(unit_t& a, B b) { a = a + b; }
            friend constexpr auto operator+=(unit_t& a, B b)
            {
                a = a + b;
                return a;
            }
            template<typename B> requires requires(unit_t& a, B b) { a = a - b; }
            friend constexpr auto operator-=(unit_t& a, B b)
            {
                a = a - b;
                return a;
            }
            template<typename B> requires requires(unit_t& a, B b) { a = a * b; }
            friend constexpr auto operator*=(unit_t& a, B b)
            {
                a = a * b;
                return a;
            }
            template<typename B> requires requires(unit_t& a, B b) { a = a / b; }
            friend constexpr auto operator/=(unit_t& a, B b)
            {
                a = a / b;
                return a;
            }

            template<typename OtherFraction, std::convertible_to<scalar_type> OtherScalar>
            constexpr auto operator<=>(unit_t<Descriptor, OtherFraction, OtherScalar> const& other) const {
              
              const auto self_value = Fraction::mul(value);
              const auto other_value = OtherFraction::mul(other.value);
              using common = std::common_type_t<std::decay_t<decltype(self_value)>, std::decay_t<decltype(other_value)>>;

              return common(self_value) <=> common(other_value);
            };
        };
    }

    template<typename... Nom>
    using compound = base::unit_t<
        decltype(base::detail::multiply_and_normalize<typename Nom::descriptor...>()),
        base::fraction_mul_var<typename Nom::fraction...>,
        std::common_type_t<typename Nom::scalar_type...>>;

    template<typename... Nom>
    using inverse = typename compound<Nom...>::inverse;

    using fract_nano = base::fraction_t<1, 1000000000>;
    using fract_micro = base::fraction_t<1, 1000000>;
    using fract_milli = base::fraction_t<1, 1000>;
    using fract_centi = base::fraction_t<1, 100>;
    using fract_deci = base::fraction_t<1, 10>;
    using fract_unity = base::fraction_t<1>;
    using fract_deca = base::fraction_t<10>;
    using fract_hecto = base::fraction_t<100>;
    using fract_kilo = base::fraction_t<1000>;
    using fract_mega = base::fraction_t<1000000>;
    using fract_giga = base::fraction_t<1000000000>;

    namespace base_descriptors
    {
        enum class luminosity;
        enum class mass;
        enum class time;
        enum class distance;
        enum class current;
        enum class temperature;
        enum class substance_amount;
    }

    template<typename T, typename V>
    concept same_as = std::is_arithmetic_v<base::dissolve_unit_t<compound<T, inverse<V>>>>;

    template<typename Unit, typename Fraction> using as_fract = base::unit_t<typename Unit::descriptor, base::fraction_mul<typename Unit::fraction, Fraction>, typename Unit::scalar_type>;
    template<typename Unit> using nano = as_fract<Unit, fract_nano>;
    template<typename Unit> using micro = as_fract<Unit, fract_micro>;
    template<typename Unit> using milli = as_fract<Unit, fract_milli>;
    template<typename Unit> using centi = as_fract<Unit, fract_centi>;
    template<typename Unit> using deci = as_fract<Unit, fract_deci>;
    template<typename Unit> using unity = as_fract<Unit, fract_unity>;
    template<typename Unit> using deca = as_fract<Unit, fract_deca>;
    template<typename Unit> using hecto = as_fract<Unit, fract_hecto>;
    template<typename Unit> using kilo = as_fract<Unit, fract_kilo>;
    template<typename Unit> using mega = as_fract<Unit, fract_mega>;
    template<typename Unit> using giga = as_fract<Unit, fract_giga>;

    template<typename Scalar> using t_kilogram = base::unit_t<base::descriptor_t<std::tuple<base_descriptors::mass>>, fract_kilo, Scalar>;
    template<typename Scalar> using t_gram = milli<t_kilogram<Scalar>>;
    template<typename Scalar> using t_ton = kilo<t_kilogram<Scalar>>;
    template<typename Scalar> using t_candela = base::unit_t<base::descriptor_t<std::tuple<base_descriptors::luminosity>>, fract_unity, Scalar>;
    template<typename Scalar> using t_second = base::unit_t<base::descriptor_t<std::tuple<base_descriptors::time>>, fract_unity, Scalar>;
    template<typename Scalar> using t_minute = as_fract<t_second<Scalar>, base::fraction_t<60>>;
    template<typename Scalar> using t_hour = as_fract<t_second<Scalar>, base::fraction_t<60 * 60>>;
    template<typename Scalar> using t_meter = base::unit_t<base::descriptor_t<std::tuple<base_descriptors::distance>>, fract_unity, Scalar>;
    template<typename Scalar> using t_ampere = base::unit_t<base::descriptor_t<std::tuple<base_descriptors::current>>, fract_unity, Scalar>;
    template<typename Scalar> using t_kelvin = base::unit_t<base::descriptor_t<std::tuple<base_descriptors::temperature>>, fract_unity, Scalar>;
    template<typename Scalar> using t_mol = base::unit_t<base::descriptor_t<std::tuple<base_descriptors::substance_amount>>, fract_unity, Scalar>;

    template<typename Scalar> using t_newton = compound<t_kilogram<Scalar>, t_meter<Scalar>, inverse<t_second<Scalar>, t_second<Scalar>>>;
    template<typename Scalar> using t_pascal = compound<t_newton<Scalar>, inverse<t_meter<Scalar>, t_meter<Scalar>>>;
    template<typename Scalar> using t_bar = hecto<kilo<t_pascal<Scalar>>>;

    template<typename Scalar> using t_joule = compound<t_newton<Scalar>, t_meter<Scalar>>;
    template<typename Scalar> using t_watt = compound<t_joule<Scalar>, inverse<t_second<Scalar>>>;
    template<typename Scalar> using t_volt = compound<t_watt<Scalar>, inverse<t_ampere<Scalar>>>;
    template<typename Scalar> using t_ohm = compound<t_volt<Scalar>, inverse<t_ampere<Scalar>>>;
    template<typename Scalar> using t_coulomb = compound<t_ampere<Scalar>, t_second<Scalar>>;
    template<typename Scalar> using t_farad = compound<t_coulomb<Scalar>, inverse<t_volt<Scalar>>>;
    template<typename Scalar> using t_henry = compound<t_ohm<Scalar>, t_second<Scalar>>;
    template<typename Scalar> using t_tesla = compound<t_newton<Scalar>, inverse<t_ampere<Scalar>, t_meter<Scalar>>>;
    template<typename Scalar> using t_weber = compound<t_volt<Scalar>, t_second<Scalar>>;
    template<typename Scalar> using t_gauss = milli<deci<t_tesla<Scalar>>>;
    template<typename Scalar> using t_maxwell = compound<t_gauss<Scalar>, centi<t_meter<Scalar>>, centi<t_meter<Scalar>>>;
    template<typename Scalar> using t_biot = deca<t_ampere<Scalar>>;

    template<typename Scalar> using t_becquerel = inverse<t_second<Scalar>>;
    template<typename Scalar> using t_rutherford = mega<t_becquerel<Scalar>>;
    template<typename Scalar> using t_curie = as_fract<giga<t_becquerel<Scalar>>, base::fraction_t<37>>;
    template<typename Scalar> using t_roentgen = as_fract<compound<t_coulomb<Scalar>, inverse<t_kilogram<Scalar>>>, base::reduced_fraction<258, 1000000>>;

    namespace imperial
    {
        template<typename Scalar> using t_inch = as_fract<centi<t_meter<Scalar>>, base::reduced_fraction<254, 100>>;
        template<typename Scalar> using t_foot = as_fract<t_inch<Scalar>, base::fraction_t<12>>;
        template<typename Scalar> using t_yard = as_fract<t_foot<Scalar>, base::fraction_t<3>>;
        template<typename Scalar> using t_mile = as_fract<t_yard<Scalar>, base::fraction_t<1760>>;
        template<typename Scalar> using t_nautical_mile = as_fract<t_yard<Scalar>, base::reduced_fraction<20254, 10>>;

        template<typename Scalar> using t_ounce = as_fract<t_gram<Scalar>, base::reduced_fraction<2835, 100>>;
        template<typename Scalar> using t_pound = as_fract<t_ounce<Scalar>, base::fraction_t<16>>;
        template<typename Scalar> using t_stone = as_fract<t_pound<Scalar>, base::fraction_t<14>>;
        template<typename Scalar> using t_hundredweight = as_fract<t_pound<Scalar>, base::fraction_t<112>>;
        template<typename Scalar> using t_long_ton = as_fract<t_hundredweight<Scalar>, base::fraction_t<20>>;

        namespace detail {
            template<typename Scalar> using t_slug = as_fract<t_pound<Scalar>, base::reduced_fraction<3217405, 100000>>;
        }
        template<typename Scalar> using t_pound_force = compound<detail::t_slug<Scalar>, t_foot<Scalar>, inverse<t_second<Scalar>, t_second<Scalar>>>;
        template<typename Scalar> using t_psi = compound<t_pound_force<Scalar>, inverse<t_inch<Scalar>, t_inch<Scalar>>>;

        using inch = t_inch<double>;
        using foot = t_foot<double>;
        using yard = t_yard<double>;
        using mile = t_mile<double>;
        using nautical_mile = t_nautical_mile<double>;
              
        using ounce = t_ounce<double>;
        using pound = t_pound<double>;
        using stone = t_stone<double>;
        using hundredweight = t_hundredweight<double>;
        using long_ton = t_long_ton<double>;

        using pound_force = t_pound_force<double>;
        using psi = t_psi<double>;

        using finch = t_inch<float>;
        using ffoot = t_foot<float>;
        using fyard = t_yard<float>;
        using fmile = t_mile<float>;
        using fnautical_mile = t_nautical_mile<float>;

        using founce = t_ounce<float>;
        using fpound = t_pound<float>;
        using fstone = t_stone<float>;
        using fhundredweight = t_hundredweight<float>;
        using flong_ton = t_long_ton<float>;

        using fpound_force = t_pound_force<float>;
        using fpsi = t_psi<float>;
    }

    using default_scalar = double;
    using gram = t_gram<double>;
    using kilogram = t_kilogram<double>;
    using ton = t_ton<double>;
    using candela = t_candela<double>;
    using second = t_second<double>;
    using meter = t_meter<double>;
    using ampere = t_ampere<double>;
    using kelvin = t_kelvin<double>;
    using mol = t_mol<double>;
    using minute = t_minute<double>;
    using hour = t_hour<double>;
    using newton = t_newton<double>;
    using pasc = t_pascal<double>;
    using bar = t_bar<double>;
    using joule = t_joule<double>;
    using watt = t_watt<double>;
    using volt = t_volt<double>;
    using ohm = t_ohm<double>;
    using coulomb = t_coulomb<double>;
    using farad = t_farad<double>;
    using henry = t_henry<double>;
    using tesla = t_tesla<double>;
    using weber = t_weber<double>;
    using gauss = t_gauss<double>;
    using maxwell = t_maxwell<double>;
    using biot = t_biot<double>;
    using becquerel = t_becquerel<double>;
    using rutherford = t_rutherford<double>;
    using curie = t_curie<double>;
    using roentgen = t_roentgen<double>;

    using fgram = t_gram<float>;
    using fkilogram = t_kilogram<float>;
    using fton = t_ton<float>;
    using fcandela = t_candela<float>;
    using fsecond = t_second<float>;
    using fmeter = t_meter<float>;
    using fampere = t_ampere<float>;
    using fkelvin = t_kelvin<float>;
    using fmol = t_mol<float>;
    using fminute = t_minute<float>;
    using fhour = t_hour<float>;
    using fnewton = t_newton<float>;
    using fpascal = t_pascal<float>;
    using fbar = t_bar<float>;
    using fjoule = t_joule<float>;
    using fwatt = t_watt<float>;
    using fvolt = t_volt<float>;
    using fohm = t_ohm<float>;
    using fcoulomb = t_coulomb<float>;
    using ffarad = t_farad<float>;
    using fhenry = t_henry<float>;
    using ftesla = t_tesla<float>;
    using fweber = t_weber<float>;
    using fgauss = t_gauss<float>;
    using fmaxwell = t_maxwell<float>;
    using fbiot = t_biot<float>;
    using fbecquerel = t_becquerel<float>;
    using frutherford = t_rutherford<float>;
    using fcurie = t_curie<float>;
    using froentgen = t_roentgen<float>;

    namespace constants {
      template<typename Scalar> constexpr inverse<t_mol<Scalar>> avogadro{ 6.02214076e23 };
    }
}

#endif // RNU_SI_HPP
