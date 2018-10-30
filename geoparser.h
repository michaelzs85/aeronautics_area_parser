#pragma once

#include "geotypes.h"
#include "geovincenty.h"
#include "geoutils.h"
#include "fmt/format.h"
#include <boost/optional.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <vector>

BOOST_FUSION_ADAPT_STRUCT(
    geo::Location,
    lat, lon
    );

namespace geo {

namespace x3 = boost::spirit::x3;
namespace fusion = boost::fusion;

using x3::_attr;
using x3::_val;
using x3::lit;
using x3::rule;
using x3::double_;
using x3::omit;
using x3::no_case;
using x3::char_;
using fusion::at_c;

using two_digit_type = x3::int_parser<double, 10, 2, 2>;
two_digit_type const two_digit = {};

using three_digit_type = x3::int_parser<double, 10, 3, 3>;
three_digit_type const three_digit = {};

template <typename T>
struct no_exp_ureal_polcies : x3::ureal_policies<T>
{
  template <typename Iterator>
  static bool
  parse_exp(Iterator& /*first*/, Iterator const& /*last*/)
  {
    return false;
  }
};

using no_exp_double_type = x3::real_parser<double, no_exp_ureal_polcies<double>>;
no_exp_double_type const no_exp_double_ = {};


// Latitude parser ddmmssH -> geo::Lat

struct northsouth_ : x3::symbols<double>
{
  northsouth_()
  {
    add
        ("N", 1.)
        ("n", 1.)
        ("S", -1.)
        ("s", -1.)
        ;
  }
} const northsouth;

auto lat_from_dms =[](auto& ctx){
  double deg = at_c<0>(_attr(ctx));
  double min = at_c<1>(_attr(ctx));
  double sec = at_c<2>(_attr(ctx));
  double sig = at_c<3>(_attr(ctx));
  _val(ctx) = Lat{sig*(deg + min/60. + sec/3600.)};
};


rule<struct LatRuleId, Lat> const latitude = "latitude_parser";
auto const latitude_def = (two_digit > two_digit > no_exp_double_ > northsouth)[lat_from_dms];


// Longitude parser dddmmssH -> geo::Lon

struct eastwest_ : x3::symbols<double>
{
  eastwest_()
  {
    add
        ("E", 1.)
        ("e", 1.)
        ("W", -1.)
        ("w", -1.)
        ;
  }
} const eastwest;

auto lon_from_dms = [](auto& ctx){
  double deg = at_c<0>(_attr(ctx));
  double min = at_c<1>(_attr(ctx));
  double sec = at_c<2>(_attr(ctx));
  double sig = at_c<3>(_attr(ctx));
  _val(ctx) = Lon{sig*(deg + min/60. + sec/3600.)};
};

rule<struct LonRuleId, Lon> const longitude = "longitude_parser";
auto const longitude_def = (three_digit > two_digit > no_exp_double_ > eastwest)[lon_from_dms];

// Coord Parser ddmmssH dddmmssH -> geo::Location

rule<struct CoordRuleId, Location> const coord = "coordinate_parser";
auto const coord_def = latitude > longitude;

// icao circle description parser

struct unit_ : x3::symbols<double>
{
  unit_()
  {
    add
        ("ft", 0.3048)
        ("nm", 1852.)
        ("NM", 1852.)
        ("km", 1000.)
        ("m", 1.);
  }
} const unit;

auto circle_builder = [](auto& ctx){
  Meters radius{at_c<0>(_attr(ctx)) * at_c<1>(_attr(ctx))};
  Location center = at_c<2>(_attr(ctx));
  std::vector<Location> c = create_circle_points(center, radius, 2);
  _val(ctx).insert(std::end(_val(ctx)), std::begin(c), std::end(c));
};


rule<struct CircleDescId, std::vector<Location>> const circle = "circle_parser";
auto const circle_def = (x3::no_case[lit("A circle") > -lit(",")] > double_ > unit
                        > x3::no_case[lit("radius centred at")] > coord
                        > x3::omit[*x3::char_])[circle_builder];



// arcparser -> std::vector<geo::Locations>
struct direction_ : x3::symbols<double>
{
  direction_()
  {
    add
        ("clockwise", 1)
        ("anti-clockwise", -1.)
        ("anticlockwise", -1.)
        ;
  }
} const direction;

auto arc_builder = [](auto& ctx){
  Location const& arc_begin = _val(ctx).back();
  double direction = at_c<0>(_attr(ctx));
  //double radius = at_c<1>(_attr(ctx)) * at_c<2>(_attr(ctx));
  Location const& center = at_c<3>(_attr(ctx));
  Location const& arc_end = at_c<4>(_attr(ctx));

  std::vector<Location> arc = create_arc_points(center, arc_begin, arc_end, (direction > 0) ? geo::dir::clockwise : geo::dir::anticlockwise);
  _val(ctx).insert(std::end(_val(ctx)), std::begin(arc), std::end(arc));
};

rule<struct ArcDescId, std::vector<Location>> const arc = "arc_parser";
auto const arc_def = ((-lit("-") >> no_case[lit("thence")]) > direction >> no_case["by" > (lit("an")|lit("the")) > "arc of a circle radius"]
                     > double_ > unit > no_case[(lit("centred")|lit("centered")) > lit("on")]
                     > coord > no_case[lit("to")] > coord > -lit("-"))[arc_builder];


// Icao Area Parser -> std::vector<geo::Locations>

rule<struct AreaParserId, std::vector<Location>> const icao_area = "icao_area_parser";
auto const icao_area_def = +((coord % (lit("-")|lit(","))) | arc) | circle;

BOOST_SPIRIT_DEFINE(latitude, longitude, coord, icao_area, circle, arc);

} // namespace geo
