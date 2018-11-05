#pragma once

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <functional>

namespace geo::parser {

namespace x3 = boost::spirit::x3;

class position_cache_tag;

using iterator_type = std::string::const_iterator;
using phrase_context_type = x3::phrase_parse_context<x3::space_type>::type;
using position_cache = x3::position_cache<std::vector<iterator_type>>;
using context_type = x3::context<position_cache_tag, std::reference_wrapper<position_cache> const, phrase_context_type>;

}
