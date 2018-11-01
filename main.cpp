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
  using geo::circle_;
  using geo::area_;

  namespace x3 = boost::spirit::x3;

  // 46°45'00"N , 002°50'00"E

  // "arc clockwise de 4.86 NM de rayon centre sur 43°36'10\"N , 005°06'29\"E - 43°31'21\"N , 005°07'14\"E"
  std::string tst = "535955N 0014027W - 535348N 0013100W - 534904N 0012703W - thence clockwise by the arc of a circle radius 8 nm centered on 535157N 0013938W to 534359N 0013847W - 535312N 0015259W - thence clockwise by the arc of a circle radius 8 nm centered on 535157N 0013938W to 535955N 0014027W";
  iterator_type iter = tst.begin();
  iterator_type const end = tst.end();

  geo::ast_type area{};
  bool r = x3::phrase_parse(iter, end, area_(), x3::space, area);

  if(r && iter == end)
  {
    fmt::print("parsed area succesfully {}!\n", area.which());
  }
  else
  {
    fmt::print("failed parse!\n");
  }
  return 0;
}
