#include "geolocation_def.h"
#include "geoparser_config.h"

namespace geo::parser {

namespace _private {
BOOST_SPIRIT_INSTANTIATE(arcinit_type, iterator_type, context_type);
BOOST_SPIRIT_INSTANTIATE(center_type, iterator_type, context_type);
BOOST_SPIRIT_INSTANTIATE(circleinit_type, iterator_type, context_type);
}

BOOST_SPIRIT_INSTANTIATE(lat_type, iterator_type, context_type);
BOOST_SPIRIT_INSTANTIATE(lon_type, iterator_type, context_type);
BOOST_SPIRIT_INSTANTIATE(coord_type, iterator_type, context_type);
BOOST_SPIRIT_INSTANTIATE(arc_type, iterator_type, context_type);
BOOST_SPIRIT_INSTANTIATE(circle_type, iterator_type, context_type);

}
