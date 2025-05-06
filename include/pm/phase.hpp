/**
 * pm/phase.hpp
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

#ifndef _PM_PHASE_HPP
#define _PM_PHASE_HPP

#include <string>
#include <tuple>

#include <pm/concepts.hpp>
#include <pm/json.hpp>

namespace pm {

/**
 * \brief A statistics measurement phase
 * 
 * This class satisfies the \ref pm::MeasurementPhase "MeasurementPhase" concept and provides a JSON data storage.
 * It consists of several measurements defined at compile time.
 * 
 * The format of the JSON document provided by \ref gather_data is described in the manual.
 * 
 * \tparam M the declared meters
 */
template<Meter<nlohmann::json>... M>
class Phase  {
private:
    std::tuple<M...> meters_;
    std::string name_;
    nlohmann::json data_;
    std::vector<nlohmann::json> children_;

public:
    /**
     * \brief Reports the phase's number of meters
     * 
     * \return the phase's number of meters
     */
    static constexpr size_t num_meters() { return std::tuple_size_v<decltype(meters_)>; }

    /**
     * \brief Returns true
     * 
     * This class provides a JSON data storage even if no meters are declared.
     */
    static constexpr bool has_data() { return true; }

    /**
     * \brief Tests whether any meters have been declared
     * 
     * \return true if at least one measurement has been declared
     * \return false if no measurement has been declared (`Phase<>`)
     */
    [[deprecated("direct access to meters is not recommended")]]
    static constexpr bool has_meters() { return num_meters() > 0; }

private:
    template<size_t I> auto& get() { return std::get<I>(meters_); }
    template<size_t I> auto const& get() const { return std::get<I>(meters_); }

    template<size_t I = 0>
    void start_meters() {
        if constexpr(I < num_meters()) {
            get<I>().start();
            start_meters<I+1>();
        }
    }

    template<size_t I = num_meters()>
    void pause_meters() {
        if constexpr(I > 0) {
            get<I-1>().pause();
            pause_meters<I-1>();
        }
    }

    template<size_t I = 0>
    void resume_meters() {
        if constexpr(I < num_meters()) {
            get<I>().resume();
            resume_meters<I+1>();
        }
    }

    template<size_t I = num_meters()>
    void stop_meters() {
        if constexpr(I > 0) {
            get<I-1>().stop();
            stop_meters<I-1>();
        }
    }

    template<size_t I = 0>
    void gather_metrics(nlohmann::json& metrics) const {
        if constexpr(I < num_meters()) {
            auto& m = get<I>();
            metrics[m.key()] = m.gather_metrics();
            gather_metrics<I+1>(metrics);
        }
    }

    template<Metric X, size_t I>
    X::MetricValue get_metric() const {
        if(I < num_meters()) {
            if constexpr(std::tuple_element_t<I, decltype(meters_)>::template has_metric<X>()) {
                return get<I>().template get_metric<X>();
            } else {
                return get_metric<X, I+1>();
            }
        } else {
            return typename X::MetricValue{};
        }
    }

public:
    /**
     * \brief Constructs a phase with the given name
     * 
     * \param name the name of the phase
     */
    Phase(std::string&& name) : name_(std::move(name)) {
    }

    Phase() {}
    Phase(Phase const& other) = delete;
    Phase(Phase&& other) = default;
    Phase& operator=(Phase const& other) = delete;
    Phase& operator=(Phase&& other) = default;

    /**
     * \brief Starts the phase's meters in order of their declaration
     */
    void start() {
        start_meters();
    }

    /**
     * \brief Pauses the phase's meters in reverse order of their declaration
     */
    void pause() {
        pause_meters();
    }

    /**
     * \brief Resumes the phase's meters in order of their declaration
     */
    void resume() {
        resume_meters();
    }

    /**
     * \brief Stops the phase's meters in reverse order of their declaration
     */
    void stop() {
        stop_meters();
    }

    /**
     * \brief Appends the given phase as a child of this phase
     * 
     * The child's data is gathered into this phase's JSON data storage.
     * 
     * \param child the child phase to append
     */
    template<JSONMeasurementPhase T>
    void append_child(T const& child) {
        children_.emplace_back(child.gather_data());
    }

    /**
     * \brief Provides access to the `I`-th meter as declared
     * 
     * \tparam I the number of the meter as declared
     * \return the `I`-th meter
     */
    template<size_t I>
    [[deprecated("direct access to meters is not recommended")]]
    auto const& meter() const { return std::get<I>(meters_); }

    /**
     * \brief Gets the given metric for this phase
     * 
     * The metric is retrieved from the first meter that reports to have it.
     * 
     * \tparam X the metric to retrieve
     * \return the value of the metric
     */
    template<Metric X>
    X::MetricValue get_metric() const { return get_metric<X, 0>(); }

    /**
     * \brief Provides write access to the JSON data storage
     * 
     * This is the part that will be contained in the application-defined `data` object of the JSON document
     * 
     * \return the JSON data storage
     */
    nlohmann::json& data() { return data_; }

    /**
     * \brief Gathers the phase's data in a JSON object
     * 
     * The format of the object is described in the manual.
     * 
     * \return the JSON data
     */
    nlohmann::json gather_data() const {
        nlohmann::json json;
        json[JSON_KEY_NAME] = name_;

        if(!children_.empty()) {
            auto children = nlohmann::json::array();
            for(auto& c : children_) children.push_back(c);
            json[JSON_KEY_CHILDREN] = std::move(children);
        }

        if constexpr(num_meters() > 0) {
            nlohmann::json metrics;
            gather_metrics(metrics);
            json[JSON_KEY_METRICS] = std::move(metrics);
        }

        if(!data_.empty()) {
            json[JSON_KEY_DATA] = data_;
        }

        return json;
    }

    /**
     * \brief The name of the phase
     * 
     * \return the name of the phase 
     */
    std::string const& name() const { return name_; }
};

}

#endif
