#include "fmt/format.h"
#include "geotypes.h"
#include "geoparser.h"
#include "boost/variant.hpp"
#include <fstream>
#include <filesystem>


struct xml_print_helper
{
  xml_print_helper(std::string name)
  {
    fmt::print(
          "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
          "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
          "<Document>\n"
          "  <name>{}</name>\n"
          "  <open>0</open>\n", name);
  }

  ~xml_print_helper()
  {
    fmt::print(
          "</Document>\n"
          "</kml>\n");
  }

};


struct xml_printer : public boost::static_visitor<void>
{

  xml_printer() : boost::static_visitor<void>()
  {
    fmt::print(
          "  <Placemark>\n"
          "    <name>REPLACEME</name>\n"
          "      <Polygon>\n"
          "        <extrude>0</extrude>\n"
          "        <altitudeMode>clampToGround</altitudeMode>\n"
          "        <outerBoundaryIs>\n"
          "          <LinearRing>\n"
          "            <coordinates>\n");
  }

  xml_printer(std::string_view area_name) : boost::static_visitor<void>()
  {
    fmt::print(
          "  <Placemark>\n"
          "    <name>{}</name>\n"
          "      <Polygon>\n"
          "        <extrude>0</extrude>\n"
          "        <altitudeMode>clampToGround</altitudeMode>\n"
          "        <outerBoundaryIs>\n"
          "          <LinearRing>\n"
          "            <coordinates>\n", area_name);
  }

  ~xml_printer()
  {
    fmt::print(
          "          </coordinates>\n"
          "        </LinearRing>\n"
          "      </outerBoundaryIs>\n"
          "    </Polygon>\n"
          "  </Placemark>\n");
  }


  void operator()(geo::Location l)
  {
    fmt::print("            {:.6f},{:.6f}\n", l.lon, l.lat);
  }

  void operator()(std::string const& text)
  {
    fmt::print("            <!--{}-->\n", text);
  }
};



int main(int argc, char* argv[])
{
  if(argc != 2)
  {
    fmt::print(stderr, "use with path to file as argument!\n");
    return 1;
  }

  namespace x3 = boost::spirit::x3;
  namespace fs = std::filesystem;
  fs::path infile(argv[1]);
  if(!fs::exists(infile))
  {
    fmt::print(stderr, "The given file path doesn't seem to exist!\n");
    return 2;
  }

  std::ifstream in(infile);
  std::string line;
  std::string last_line("");
  {
    xml_print_helper _(infile.filename().string());
    while(std::getline(in, line))
    {

      if(!geo::starts_with_coord(line))
      {
        fmt::print("<!--{}-->\n",line);
        last_line = std::move(line);
        std::transform(last_line.begin(), last_line.end(), last_line.begin(), [](auto c) -> decltype (c) {
          if(c == ' ') return '_';
          return std::tolower(c);
        });
        continue;
      }

      std::vector<boost::variant<geo::Location, std::string>> area = geo::parse_area_description(line);
      {
        xml_printer visitor(last_line);
        for(auto const& loc : area)
        {
          boost::apply_visitor(visitor, loc);
        }
      }
    }
  }
  return 0;
}
