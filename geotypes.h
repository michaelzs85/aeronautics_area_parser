#pragma once

#include "fmt/format.h"

namespace geo {

template<typename T, class Tag>
struct Type
{
  using type = T;
  Type() = default;
  explicit constexpr Type(T v) : val(v) {}
  constexpr operator T() const {return val;}

  T val{};
};

using Lat = Type<double, struct LatTag>;
using Lon = Type<double, struct LonTag>;
using Azimuth = Type<double, struct AzimuthTag>;
using NM = Type<double, struct NMTag>;
using Km = Type<double, struct KmTag>;

struct Meters : Type<double, struct MetersTag>
{
  explicit Meters(double v) : Type<double, struct MetersTag>(v) {}
  Meters(NM v) : Type<double, struct MetersTag>(0.) {val = 1852. * v;}
  Meters(Km v) : Type<double, struct MetersTag>(0.) {val = 1000. * v;}
};

struct Location
{
  Lat lat;
  Lon lon;
};

struct BearingDistance
{
  Azimuth bear;
  Meters dist;
};

} // namespace geo


namespace fmt {
using geo::Location;
using geo::BearingDistance;

template <>
struct formatter<Location > {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

//  template <typename FormatContext>
//  auto format(const Location &loc, FormatContext &ctx) {
//    return format_to(ctx.begin(), "{:02.6f} {:03.6f}", loc.lat, loc.lon);
//  }
  template <typename FormatContext>
  auto format(const Location &loc, FormatContext &ctx) {
    return format_to(ctx.begin(), "{:02.6f},{:03.6f}", loc.lon, loc.lat);
  }

};

template <>
struct formatter<BearingDistance > {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const BearingDistance &bd, FormatContext &ctx) {
    return format_to(ctx.begin(), "{:03.2f}° {:.2f}m", bd.bear, bd.dist);
  }
};
} // namespace fmt
