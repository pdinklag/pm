#ifndef _PM_NOOP_PHASE_HPP
#define _PM_NOOP_PHASE_HPP

#include <pm/concepts.hpp>

namespace pm {

/**
 * \brief A measurement phase that does nothing
 * 
 * This class is intended to be used as a replacement of \ref Phase for when all statistics measurement is to be optimized out at compile time.
 * All functions are implemented as no-ops and should thus be completely optimized away by a compiler with proper optimization set.
 * 
 * This class satisfies the \ref pm::MeasurementPhase "MeasurementPhase" concept and provides a JSON data storage.
 */
class NoopPhase {
private:
    class NoData {
    private:
        struct IgnoreAssignment {
            template<typename Whatever>
            IgnoreAssignment& operator=(Whatever&&){ return *this; }

            // these are required in the context of unit tests
            inline IgnoreAssignment operator[](std::string&&) { return IgnoreAssignment{}; }
            template<std::default_initializable T> operator T() { return {}; }
            template<typename T> bool operator==(T const&) { return false; }
            template<typename T> bool operator!=(T const&) { return false; }
        };

    public:
        IgnoreAssignment operator[](std::string&&) { return IgnoreAssignment{}; }
        nlohmann::json operator[](std::string const&) const { return {}; }
    };

    static constexpr NoData nothing_ = {};

public:
    /**
     * \brief Returns false
     * 
     * A no-op phase has no means to store any data.
     */
    static constexpr bool has_data() { return false; }

    /**
     * \brief Returns false
     * 
     * A no-op phase has no meters.
     */
    static constexpr bool has_meters() { return false; }

    /**
     * \brief Construction by name, which is not processed or stored in any way
     */
    inline NoopPhase(std::string&&) {}

    inline NoopPhase() {}
    NoopPhase(NoopPhase const& other) = delete;
    NoopPhase(NoopPhase&& other) = default;
    NoopPhase& operator=(NoopPhase const& other) = delete;
    NoopPhase& operator=(NoopPhase&& other) = default;

    /**
     * \brief No-op
     */
    inline void start() {}

    /**
     * \brief No-op
     */
    inline void pause() {}

    /**
     * \brief No-op
     */
    inline void resume() {}

    /**
     * \brief No-op
     */
    inline void stop() {}

    /**
     * \brief No-op
     */
    inline void append_child(NoopPhase&&) {}

    /**
     * \brief Returns a dummy data storage for which all accesses result in no-ops
     */
    inline NoData data() { return nothing_; }

    /**
     * \brief Returns a dummy data storage for which all accesses result in no-ops
     */
    inline NoData gather_data() const { return nothing_; }
};

}

#endif
