#include "fmt/format.h"
#include <boost/optional.hpp>
#include <boost/convert/lexical_cast.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "geotypes.h"
#include "old_geoparser.h"

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

  std::string locstr("5520N 00655W, 542500N 0081000W, 5355N 00530W, 522000N 0053000W, 5100N 00800W, 5100N 01500W, 5400N 01500W, 5434N 01000W, 5445N 00900W, 5520N 00815W, 5525N 00720W, 5520N 00655W,");

  fmt::print(
"  <Placemark>\n"
"    <name>REPLACEME</name>\n"
"      <Polygon>\n"
"        <extrude>0</extrude>\n"
"        <altitudeMode>clampToGround</altitudeMode>\n"
"        <outerBoundaryIs>\n"
"          <LinearRing>\n"
"            <coordinates>\n");

  auto begin = locstr.begin();
  do
  {
    std::vector<Location> area;
    bool r = x3::phrase_parse(begin, locstr.end(), icao_area, x3::space, area);

    if(r)
      for(Location const&  l : area)
        fmt::print("            {}\n", l);
    else
      fmt::print("FUCK\n");

    if(begin != locstr.end())
    {
      std::string words;
      bool r = x3::phrase_parse(begin, locstr.end(), -x3::lit("-") >> x3::lexeme[*(~x3::char_("-"))] >> -x3::lit("-"), x3::space, words);
      if(r)
        fmt::print("<!--Couldn't understand: '{}'-->\n", words);
      else
        fmt::print("FUCK\n");
    }
  }
  while(begin != locstr.end());

fmt::print(
"          </coordinates>\n"
"        </LinearRing>\n"
"      </outerBoundaryIs>\n"
"    </Polygon>\n"
"  </Placemark>\n");

  return 0;
}
