#include "fmt/format.h"
#include <boost/optional.hpp>
#include <boost/convert/lexical_cast.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "geotypes.h"
#include "geoparser.h"

struct MyClass
{
  double val;
};

//BOOST_FUSION_ADAPT_STRUCT(
//    MyClass,
//    (double, val)
//)

namespace x3 = boost::spirit::x3;
namespace fusion = boost::fusion;

struct sign_ : x3::symbols<double>
{
  sign_()
  {
    add
        ("P", 1.)
        ("M", -1.)
        ;
  }
} sign;


using x3::rule;
using x3::_attr;
using fusion::at_c;
using boost::lexical_cast;
using std::string;

auto f = [](auto& ctx){
  double signval = at_c<1>(_attr(ctx));
  auto str = at_c<0>(_attr(ctx));
  double precomma = lexical_cast<double>(string(str.begin(), str.begin()+2));
  double postcomma = lexical_cast<double>(string(str.begin()+2, str.end()));
  _val(ctx) = MyClass{signval * (precomma + postcomma/100.)};
};

rule<struct LatParserID, MyClass> const latparser = "lat_parser";
auto const latparser_def = (x3::repeat(4)[x3::digit] > sign)[f];

BOOST_SPIRIT_DEFINE(latparser);

int main()
{
//  std::string input("5535P");
//  MyClass myval_;

//  bool r = x3::parse(input.begin(), input.end(), latparser, myval_);

//  if(r)
//    fmt::print("myval: {}\n", myval_.val);
//  else
//    fmt::print("oh!\n");

  using geo::Lon;
  using geo::longitude;
  using geo::Lat;
  using geo::latitude;
  using geo::Location;
  using geo::coord;

  /*std::string latstr("553734N");
  Lat lat;
  bool r = x3::parse(latstr.begin(), latstr.end(), latitude, lat);

  if(r)
    fmt::print("lat: {}\n", lat);
  else
    fmt::print("FUCK\n");

  std::string lonstr("0041639W");
  Lon lon;
  r = x3::parse(lonstr.begin(), lonstr.end(), longitude, lon);

  if(r)
    fmt::print("lon: {}\n", lon);
  else
    fmt::print("FUCK\n");*/


  using geo::icao_area;

  //std::string locstr("553124N 0045830W - 552040N 0042722W - 551848N 0044702W - 553124N 0045830W");
  std::string locstr("A circle, 2.5 nm radius centred at 553034N 0043540W on longest notified runway (12/30)");
  std::vector<Location> area;
  bool r = x3::phrase_parse(locstr.begin(), locstr.end(), icao_area, x3::space, area);

  if(r)
    for(Location const&  l : area)
      fmt::print("{}\n", l);
  else
    fmt::print("FUCK\n");


  return 0;
}
