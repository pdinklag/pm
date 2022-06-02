#ifndef _PM_JSON_HPP
#define _PM_JSON_HPP

#include <nlohmann/json.hpp>

namespace pm {

/**
 * \brief The key for the child array in a \ref Phase JSON document
 */
constexpr char const* JSON_KEY_CHILDREN = "children";

/**
 * \brief The key for the data object in a \ref Phase JSON document
 */
constexpr char const* JSON_KEY_DATA = "data";

/**
 * \brief The key for the metrics object in a \ref Phase JSON document
 */
constexpr char const* JSON_KEY_METRICS = "metrics";

/**
 * \brief The key for the name in a \ref Phase JSON document
 */
constexpr char const* JSON_KEY_NAME = "name";

}

#endif
