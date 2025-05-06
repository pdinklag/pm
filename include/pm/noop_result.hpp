/**
 * pm/noop_result.hpp
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

 #ifndef _PM_NOOP_RESULT_HPP
 #define _PM_NOOP_RESULT_HPP
 
 #include <algorithm>
 #include <iostream>
 #include <sstream>
 #include <string>
 #include <vector>
 
 #include <pm/concepts.hpp>
 #include <pm/json.hpp>
 
 namespace pm {
 
 /**
  * \brief Dummy replacement for \ref Result to be used for optimized builds
  * 
  * This class is intended to be used as a replacement of \ref Result for when all statistics measurement is to be optimized out at compile time.
  * All functions are implemented as no-ops and should thus be completely optimized away by a compiler with proper optimization set.
  * 
  * The following is an example usage, assuming the existence of a `constexpr` function `is_release_build`:
  * \code{cpp}
  * using Result = std::conditional_t<is_release_build(), pm::NoopResult, pm::Result>;
  * \endcode
  */
class NoopResult {
public:
    inline NoopResult() {}
    NoopResult(NoopResult&&) = default;
    NoopResult(NoopResult const&) = default;
    NoopResult& operator=(NoopResult&&) = default;
    NoopResult& operator=(NoopResult const&) = default;

   /**
    * \brief No-op
    */
    inline void add(std::string&& key, bool value) { }

   /**
    * \brief No-op
    */
    template<std::integral T>
    void add(std::string&& key, T value) { }

   /**
    * \brief No-op
    */
    template<std::floating_point T>
    void add(std::string&& key, T value) { }

   /**
    * \brief No-op
    */
    inline void add(std::string&& key, std::string&& value) { }

   /**
    * \brief No-op
    */
    inline void add(std::string&& key, std::string const& value) { }

   /**
    * \brief No-op
    */
    inline void add(std::string&& key, char const* value) { }

   /**
    * \brief No-op
    */
    inline void add(std::string&& key, nlohmann::json const& value) { }

   /**
    * \brief No-op
    */
    template<JSONMeasurementPhase T>
    inline void add(T const& phase) { }

   /**
    * \brief No-op
    */
    inline void sort() { }

    /**
    * \brief No-op
    */
    inline void print(std::ostream& out = std::cout, bool append_newline = true, std::string const& prefix = "RESULT") const { }

   /**
    * \brief Returns an empty string
    */
    inline std::string str(bool append_newline = false, std::string const& prefix = "RESULT") const { return std::string(); }
};
 
}

#endif
