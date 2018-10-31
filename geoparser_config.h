#pragma once

#include <boost/spirit/home/x3.hpp>

namespace geo::parser {

namespace x3 = boost::spirit::x3;

using iterator_type = std::string::const_iterator;
using context_type = x3::phrase_parse_context<x3::space_type>::type;
}
