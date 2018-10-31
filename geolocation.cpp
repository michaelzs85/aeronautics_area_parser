#include "geolocation_def.h"
#include "geoparser_config.h"

namespace geo::parser {

BOOST_SPIRIT_INSTANTIATE(lat_type, iterator_type, context_type);
BOOST_SPIRIT_INSTANTIATE(lon_type, iterator_type, context_type);
BOOST_SPIRIT_INSTANTIATE(coord_type, iterator_type, context_type);

}
