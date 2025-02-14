/**
 * pm/json.hpp
 * part of pdinklag/pm
 * 
 * MIT License
 * 
 * Patrick Dinklage
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
