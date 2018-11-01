#include "fmt/format.h"
#include "geoast.h"
#include "geoast_adapted.h"
#include "geolocation.h"

int main()
{
  using iterator_type = std::string::const_iterator;

  using geo::lat_;
  using geo::lon_;
  using geo::coord_;
  using geo::arc_;

  namespace x3 = boost::spirit::x3;

  // 46°45'00"N , 002°50'00"E

  // "arc clockwise de 4.86 NM de rayon centre sur 43°36'10\"N , 005°06'29\"E - 43°31'21\"N , 005°07'14\"E"
  std::string tst = "arc de 4.86 NM de rayon centre sur 43°36'10\"N , 005°06'29\"E - ";
  iterator_type iter = tst.begin();
  iterator_type const end = tst.end();

  geo::ast::arc_data arc{};
  bool r = x3::phrase_parse(iter, end, arc_(), x3::space, arc);

  if(r && iter == end)
  {
    fmt::print("parsed arc succesfully {} {} {} {} {}!\n", arc.direction, arc.radius, arc.unit, arc.center.lon.sec.get_value_or(-1), arc.target.has_value());
  }
  else
  {
    fmt::print("failed parse!\n");
  }
  return 0;
}
