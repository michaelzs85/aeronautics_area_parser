#pragma once

#include <boost/spirit/home/x3.hpp>
#include <string>
#include "geoparser_config.h"

namespace geo::parser {

namespace x3 = boost::spirit::x3;

struct annotate_position
{
  template<typename T, typename Iterator, typename Context>
  inline void on_success(Iterator const& first, Iterator const& last, T& ast, Context const& context)
  {
    auto& position_cache = x3::get<position_cache_tag>(context).get();
    position_cache.annotate(ast, first, last);
  }
};

}
