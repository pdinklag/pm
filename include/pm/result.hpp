/**
 * pm/result.hpp
 * part of pdinklag/pm
 * 
 * MIT License
 * 
 * Copyright (c) 2022 Patrick Dinklage
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

#ifndef _PM_RESULT_HPP
#define _PM_RESULT_HPP

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <pm/concepts.hpp>
#include <pm/json.hpp>

namespace pm {

/**
 * \brief Helper for generating structured output lines of key-value pairs for use with <a href="https://github.com/bingmann/sqlplot-tools">sqlplot-tools</a>
 * 
 * This class simply manages pairs of keys and values to be printed to the standard output.
 * These pairs are gathered via the various `add` overloads, may then be sorted by key and finally printed to some output.
 * 
 * The output is formatted with <a href="https://github.com/bingmann/sqlplot-tools">sqlplot-tools</a> in mind:
 * it begins with the fixed prefix `RESULT`, followed by space-separated strings of the format `key=value`.
 * 
 * The following is an example use case:
 * \code{cpp}
 * Result r;
 * r.add("algorithm", "test");
 * r.add("time", 3.142);
 * r.add("memory", 1337);
 * r.sort();
 * r.print(); // defaults to std::cout
 * \endcode
 * 
 * The output of the above looks like this:
 * \code
 * RESULT algorithm=test memory=1337 time=3.142
 * \endcode
 */
class Result {
private:
    inline static std::string append_key(std::string const& key_prefix, std::string const& key) {
        return key_prefix.empty() ? key : (key_prefix + "." + key);
    }

    struct KeyValuePair {
        std::string key;
        std::string value;
    };

    std::vector<KeyValuePair> pairs_;

    inline void add_pairs_recursive(std::string const& key_prefix, nlohmann::json const& data) {
        for(auto& item : data.items()) {
            auto key = append_key(key_prefix, item.key());
            auto& v = item.value();
            if(v.is_primitive()) {
                pairs_.emplace_back(key, v.dump());
            } else {
                add_pairs_recursive(key, v);
            }
        }
    }

    inline void add_phase_data(std::string const& key_prefix, nlohmann::json const& data) {
        if(data.contains(JSON_KEY_METRICS)) {
            add_pairs_recursive(append_key(key_prefix, JSON_KEY_METRICS), data[JSON_KEY_METRICS]);
        }

        if(data.contains(JSON_KEY_DATA)) {
            add_pairs_recursive(append_key(key_prefix, JSON_KEY_DATA), data[JSON_KEY_DATA]);
        }

        if(data.contains(JSON_KEY_CHILDREN)) {
            for(auto& c : data[JSON_KEY_CHILDREN]) {
                auto key = append_key(key_prefix, c[JSON_KEY_NAME]);
                add_phase_data(key, c);
            }
        }
    }

public:
    inline Result() {}
    Result(Result&&) = default;
    Result(Result const&) = default;
    Result& operator=(Result&&) = default;
    Result& operator=(Result const&) = default;

    /**
     * \brief Adds a key-value pair with a boolean value
     * 
     * The boolean value will be formatted to `true` or `false` (in all lower-case), respectively.
     * 
     * \param key the key
     * \param value the value
     */
    inline void add(std::string&& key, bool value) {
        pairs_.emplace_back(std::move(key), value ? "true" : "false");
    }

    /**
     * \brief Adds a key-value pair with an integral value
     * 
     * \tparam T the value type, required to be integral
     * \param key the key
     * \param value the value
     */
    template<std::integral T>
    void add(std::string&& key, T value) {
        pairs_.emplace_back(std::move(key), std::to_string(value));
    }

    /**
     * \brief Adds a key-value pair with a floating point value
     * 
     * \tparam T the value type, required to be floating point
     * \param key the key
     * \param value the value
     */
    template<std::floating_point T>
    void add(std::string&& key, T value) {
        pairs_.emplace_back(std::move(key), nlohmann::json(value).dump()); // nb: nlohmann/json provides neat float formatting
    }

    /**
     * \brief Adds a key-value pair with a string value
     * 
     * The string value will not be processed in any way and may cause the output line's format to be violated if it contains characters such as spaces, `=` or newlines.
     * 
     * \param key the key
     * \param value the value
     */
    inline void add(std::string&& key, std::string&& value) {
        pairs_.emplace_back(std::move(key), std::move(value));
    }

    /**
     * \brief Adds a key-value pair with a string value
     * 
     * The string value will not be processed in any way and may cause the output line's format to be violated if it contains characters such as spaces, `=` or newlines.
     * 
     * \param key the key
     * \param value the value
     */
    inline void add(std::string&& key, std::string const& value) {
        pairs_.emplace_back(std::move(key), std::string(value));
    }

    /**
     * \brief Adds a key-value pair with a string value
     * 
     * The string value will not be processed in any way and may cause the output line's format to be violated if it contains characters such as spaces, `=` or newlines.
     * 
     * \param key the key
     * \param value the value
     */
    inline void add(std::string&& key, char const* value) {
        pairs_.emplace_back(std::move(key), std::string(value));
    }

    /**
     * \brief Adds a key-value pair with a value gathered from a JSON object.
     * 
     * The value will be converted to a string using `dump` and will not be processed any further.
     * Using this function with a JSON object will add spaces and `=` characters to the output and is therefore not supported (but also not checked).
     * 
     * An example use case for this function would be adding data from a \ref Phase :
     * \code{cpp}
     * r.add("memory", data["metrics"]["memory"]["peak"]);
     * \endcode
     * 
     * \param key the key
     * \param value the value
     */
    inline void add(std::string&& key, nlohmann::json const& value) {
        if(value.is_string()) {
            std::string v = value;
            pairs_.emplace_back(std::move(key), std::move(v));
        } else {
            pairs_.emplace_back(std::move(key), value.dump());
        }
    }

    /**
     * \brief Adds all the data of a \ref Phase
     * 
     * This will add every data point of a data storage returned by \ref Phase::gather_data .
     * The data of child phases is added recursively,
     * Note that this will not simply unfold the JSON structure but does actually consider the JSON format provided by \ref Phase .
     * The `data` and `metrics` objects will be unfolded, every child phase will be treated as a child object with the corresponding \ref Phase::name "name".
     * 
     * Keys contain the dot (`.`) character to indicate the JSON structure.
     * The output may thus become very verbose and no longer human readable.
     * 
     * \tparam T the phase type; supports any \ref pm::JSONMeasurementPhase "JSON measurement phase"
     * \param data the data of a \ref Phase
     */
    template<JSONMeasurementPhase T>
    inline void add(T const& phase) {
        add_phase_data("", phase.gather_data());
    }

    /**
     * \brief Sorts the currently stored key-value pairs by their keys
     */
    inline void sort() {
        std::sort(pairs_.begin(), pairs_.end(), [](auto const& a, auto const& b){ return a.key < b.key; });
    }

    /**
     * \brief Prints a line starting with `RESULT`, followed by the stored key-value pairs in `key=value` format
     * 
     * The pairs are separated by spaces.
     * 
     * \param out the output stream to print to
     * \param append_newline whether or not to append a newline character
     * \param prefix the line prefix; `RESULT` by default
     */
    inline void print(std::ostream& out = std::cout, bool append_newline = true, std::string const& prefix = "RESULT") const {
        out << prefix;
        for(auto& pair : pairs_) {
            out << " " << pair.key << "=" << pair.value;
        }
        if(append_newline) out << std::endl;
    }

    /**
     * \brief Prints a result line into a string using \ref print
     * 
     * By default, the newline character is omitted.
     * 
     * \param append_newline whether or not to append a newline character
     * \param prefix the line prefix; `RESULT` by default
     * \return the result of \ref print in a string
     */
    inline std::string str(bool append_newline = false, std::string const& prefix = "RESULT") const {
        std::ostringstream ss;
        print(ss, append_newline, prefix);
        return ss.str();
    }
};

}

#endif
