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

  namespace x3 = boost::spirit::x3;

  // 46°45'00"N , 002°50'00"E

  std::string tst = "46°4500\"N 002°50'00.44\"E";
  iterator_type iter = tst.begin();
  iterator_type const end = tst.end();

  geo::ast::coord_data coord;
  bool r = x3::phrase_parse(iter, end, coord_(), x3::space, coord);

  if(r && iter == end)
  {
    fmt::print("parsed coord succesfully {} {}!\n", coord.lat.deg, coord.lon.deg);
  }
  else
  {
    fmt::print("failed parse!\n");
  }
  return 0;
}
