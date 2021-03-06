#include <boost/spirit/home/x3.hpp>
#include <boost/variant.hpp>
#include "geoparser.h"
#include "geolocation.h"
#include "geoutils.h"
#include "geoparser_config.h"
#include "fmt/format.h"

namespace geo {

namespace _private{

//template<typename out_type, typename in_type>
//out_type translate(in_type val);

Lat translate(ast::lat_data val)
{
  return Lat{val.sign*(val.deg + val.min/60. + val.sec.get_value_or(0.)/(60*60))};
}

Lon translate(ast::lon_data val)
{
  return Lon{val.sign*(val.deg + val.min/60. + val.sec.get_value_or(0.)/(60*60))};
}

Location translate(ast::coord_data val)
{
  return Location{translate(val.lat), translate(val.lon)};
}

struct ast_visitor : public boost::static_visitor<void> {

  std::vector<boost::variant<Location, std::string>> area{};

  struct coord_arc_visitor : public boost::static_visitor<void> {
    std::vector<boost::variant<Location, std::string>>& area;
    coord_arc_visitor(std::vector<boost::variant<Location, std::string>>& area) : area(area) {}

    void operator()(ast::coord_data cd)
    {
      area.emplace_back(Location{translate(cd.lat), translate(cd.lon)});
    }

    void operator()(ast::arc_data ad)
    {
      Location target;
      if(ad.target.has_value())
        target = translate(ad.target.get());
      else
        target = boost::get<Location>(area.front());
      std::vector<Location> arc = create_arc_points(translate(ad.center), boost::get<Location>(area.back()), target, ad.direction);
      area.insert(area.end(), arc.begin(), arc.end());
    }

    void operator()(std::string const& s)
    {
      area.push_back(s);
    }

  };

  void operator()(ast::circle_data cd)
  {
    assert(area.size() == 0);
    std::vector<Location> circle = create_circle_points(translate(cd.center), Meters{cd.radius*cd.unit});
    area.insert(area.end(), circle.begin(), circle.end());
  }

  void operator()(std::vector<boost::variant<ast::coord_data, ast::arc_data, std::string>> vd)
  {
    coord_arc_visitor v(area);
    std::for_each(vd.begin(), vd.end(), [&](auto const& el){boost::apply_visitor(v, el);});
  }

};
using namespace _private;

std::vector<boost::variant<Location, std::string>> translate(ast_type const& ast)
{
  ast_visitor v;
  boost::apply_visitor(v, ast);
  return v.area;
}

}

std::vector<boost::variant<Location, std::string>> parse_area_description(std::string const& input)
{
  namespace x3 = boost::spirit::x3;
  geo::parser::iterator_type iter = input.begin();
  geo::parser::iterator_type const end = input.end();

  std::vector<boost::variant<Location, std::string>> retval;

  //while(iter != end)
  {
    ast_type ast;

    bool r{};
    try {
      r = x3::phrase_parse(iter, end, area_(), x3::space, ast);
    }
    catch(std::exception e)
    {
      fmt::print("!!! expectation error!\n");
      /*...*/
    }
    if(!r)
    {
      fmt::print(stderr, "ERROR! Could not parse a vaild area from '{}'\n", input);
      fmt::print(stderr, "dist(begin, iter): {}\n", std::distance(input.begin(), iter));
      //return {};
    }

    std::vector<boost::variant<Location, std::string>> parsed_points = _private::translate(ast);
    retval.insert(retval.end(), parsed_points.begin(), parsed_points.end());

    if(iter==end)
    {

      //break;
    }

    /*std::string words;
    r = x3::phrase_parse(iter, end, -x3::lit("-") >> x3::lexeme[*(~x3::char_("-"))] >> -x3::lit("-"), x3::space, words);
    if(!r)
    {
      fmt::print(stderr, "ERROR: Problem during skipping string: '{}'\n", std::string_view(&*iter, std::distance(iter, end)));
      return {};
    }
    retval.emplace_back(words);*/
  }
  return retval;
}

bool starts_with_coord(std::string const& input)
{
  namespace x3 = boost::spirit::x3;
  geo::parser::iterator_type iter = input.begin();
  geo::parser::iterator_type const end = input.end();
  geo::ast::coord_data c;
  bool retval{};
  try {
    retval = x3::phrase_parse(iter, end, coord_(),x3::space, c);
  } catch (std::exception) {
    return false;
  }
  return retval;
}

}
