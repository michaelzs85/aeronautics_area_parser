#include "fmt/format.h"
#include "geoparser.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <boost/spirit/home/x3.hpp>

std::string parse_area(std::string const& input, std::string const& name)
{
  using geo::Location;
  using geo::icao_area;
  namespace x3 = boost::spirit::x3;

  std::string retval =
      fmt::format(
"  <Placemark>\n"
"    <name>{}</name>\n"
"      <Polygon>\n"
"        <extrude>0</extrude>\n"
"        <altitudeMode>clampToGround</altitudeMode>\n"
"        <outerBoundaryIs>\n"
"          <LinearRing>\n"
"            <coordinates>\n", name);

  auto begin = input.begin();
  do
  {
    std::vector<Location> area;
    bool r = x3::phrase_parse(begin, input.end(), icao_area, x3::space, area);
    if(r)
      for(Location const&  l : area)
      {
        retval += fmt::format("            {}\n", l);
      }
    else
      fmt::print("FUCK\n");

    if(begin != input.end())
    {
      std::string words;
      bool r = x3::phrase_parse(begin, input.end(), -x3::lit("-") >> x3::lexeme[*(~x3::char_("-"))] >> -x3::lit("-"), x3::space, words);
      if(r)
        retval += fmt::format("<!--Couldn't understand: '{}'-->\n", words);
      else
        fmt::print("FUCK\n");
    }
  }
  while(begin != input.end());
  retval +=
"          </coordinates>\n"
"        </LinearRing>\n"
"      </outerBoundaryIs>\n"
"    </Polygon>\n"
"  </Placemark>\n";

  return retval;
}



int main(int argc, char *argv[])
{
  if(argc != 2)
    return 1;

  std::ifstream in(argv[1]);
  if(!in.is_open())
    return 2;
  int mode = 0;
  std::string line, name;

  fmt::print(
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
    "<Document>\n"
    "  <name>APP_LOWI</name>\n"
    "  <open>0</open>\n");

  while(std::getline(in, line))
  {
    if(line.empty())
      continue;
    if(mode == 0)
    {
      name = "";
      std::transform(line.begin(), line.end(), std::back_inserter(name), [](auto cit) -> char {
        if(cit == ' ')
          return '_';
        return std::tolower(cit);
      });
      mode++;
    }
    else if(mode == 1)
    {
      std::string area = parse_area(line, name);
      fmt::print(area);
      mode = 2;
    }
    else if(mode >= 2)
    {
      if(mode >= 2)
        mode = 0;
      else
        mode++;
      continue;
    }

  }

  fmt::print(
    "</Document>\n"
    "</kml>\n");
}
