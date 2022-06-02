#ifndef _PM_CONCEPTS
#define _PM_CONCEPTS

#include <concepts>
#include <string>

#include <nlohmann/json.hpp>

namespace pm {

/**
 * \brief Concept for string-keyed dictionaries that allow read access
 * 
 * In order to satisfy this concept, a type must override the index access operator for strings,
 * and the return type must be convertible to the specified value type.
 * 
 * \tparam T the type in question
 * \tparam V the value type
 */
template<typename T, typename V>
concept ReadableStringDict =
    requires(T const x, std::string const& key) {
        { x[key] } -> std::convertible_to<V>;
    };

/**
 * \brief Concept for readable data storages
 * 
 * In order to satisfy this concept, a type must satisfy the \ref pm::ReadableStringDict "ReadableStringDict" concept for
 * signed and unsigned integer types, as well for strings and JSON objects.
 * 
 * \tparam T the type in question
 */
template<typename T>
concept ReadableDataStorage =
    ReadableStringDict<T, intmax_t> &&
    ReadableStringDict<T, uintmax_t> &&
    ReadableStringDict<T, std::string> &&
    ReadableStringDict<T, nlohmann::json>;

/**
 * \brief Concept for types that allow being move-assigned a value
 * 
 * In order satisfy this concept, a type must provide a move assignment operator accepting the specified value type.
 * 
 * \tparam T the type in question
 * \tparam V the value type
 */
template<typename T, typename V>
concept MoveAssignableFrom =
    requires(T x, V&& value) {
        { x = std::move(value) };
    };

/**
 * \brief Concept for string-keyed dictionaries that allow write access
 * 
 * In order to satisfy this concept, a type must override the index access operator for rvalue strings,
 * and the return type must satisfy the \ref pm::MoveAssignableFrom "MoveAssignableFrom" concept for the given value type.
 * 
 * \tparam T the type in question
 * \tparam V the value type
 */
template<typename T, typename V>
concept WritableStringDict =
    requires(T x, std::string&& key) {
        { x[std::move(key)] } -> MoveAssignableFrom<V>;
    };

/**
 * \brief Concept for data storages
 * 
 * In order to satisfy this concept, a type must satisfy the \ref pm::ReadableDataStorage "ReadableDataStorage" concept as well as the
 * \ref pm::WritableStringDict "WritableStringDict" concept for
 * signed and unsigned integer types, strings and JSON objects. 
 * 
 * \tparam T 
 */
template<typename T>
concept DataStorage =
    ReadableDataStorage<T> &&
    WritableStringDict<T, intmax_t> &&
    WritableStringDict<T, uintmax_t> &&
    WritableStringDict<T, std::string> &&
    WritableStringDict<T, nlohmann::json>;

/**
 * \brief Concept for statistics measurement phases
 * 
 * A measurement phase can be started, paused, resumed and stopped using correspondingly named operations.
 * It must provide data write access via a \ref pm::DataStorage "DataStorage" returned by a function named `data`.
 * The function `gather_data` is meant for gathering final measurement and auxiliary data and return a \ref pm::ReadableDataStorage "ReadableDataStorage".
 * 
 * The `constexpr` function `has_data` can be used to test out whether the phase can actually hold data.
 * This may be `false` to indicate that data gathering has been disabled at compile time, e.g., for optimizing performance.
 * Similary, `has_meters` can be used whether any \ref pm::Meter "meters" are actually installed.
 * 
 * Phases may also be put in a parent-child relationship using `append_child`.
 * This is meant to be done \em after the child phase has completed such that it can literally be moved into the parent phase, which
 * absorbs the child's data correspondingly.
 * 
 * \tparam T the type in question
 */
template<typename T>
concept MeasurementPhase =
    std::constructible_from<T> &&
    std::constructible_from<T, std::string&&> &&
    std::movable<T> &&
    requires {
        { T::has_data() } -> std::same_as<bool>;
        { T::has_meters() } -> std::same_as<bool>;
    } &&
    requires(T x) {
        { x.start() };
        { x.pause() };
        { x.resume() };
        { x.stop() };
        { x.data() } -> DataStorage;
    } &&
    requires(T x, T&& child) {
        { x.append_child(std::move(child)) };
    } &&
    requires(T const x) {
        { x.gather_data() } -> ReadableDataStorage;
    };

/**
 * \brief Concept for statistics measuring devices
 * 
 * A meter can be started, paused, resumed and stopped using correspondingly named operations.
 * Furthermore, data can be gathered into a \ref pm::DataStorage "DataStorage" of the specified type using `gather_metrics`.
 * The function `key` must provide a string that identifies a type of measurement uniquely in a data storage.
 * 
 * When a \ref pm::MeasurementPhase "measurement phase" is appended as a child to a parent phase,
 * all meter that belong to the phase may be propagated to a parent meter using `propagate`.
 * 
 * \tparam T the type in question
 * \tparam D the data storage type
 */
template<typename T, typename D>
concept Meter =
    ReadableDataStorage<D> &&
    std::constructible_from<T> &&
    std::movable<T> &&
    requires(T x) {
        { x.start() };
        { x.pause() };
        { x.resume() };
        { x.stop() };
    } &&
    requires(T const x) {
        { x.key() } -> std::same_as<std::string>;
        { x.gather_metrics() } -> std::same_as<D>;
    } &&
    requires(T x, T& parent) {
        { x.propagate(parent) };
    };

}

#endif
