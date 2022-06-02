#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <pm.hpp>

namespace pm::examples {

using namespace pm;

// nb: these aren't really "unit tests", they mainly exist to check whether the examples compile
TEST_SUITE("pm") {
    TEST_CASE("MemoryTimePhase") { 
        pm::MemoryTimePhase compute_phase("Example");
        {
            int sum = 0;
            compute_phase.start();
        
            size_t const bufsize = 1'000'000;
            char* buffer = new char[bufsize];
            for(size_t i = 0; i < bufsize; i++) buffer[i] = (char)i;
            
            compute_phase.pause();
            std::cout << "initialization done!" << std::endl;
            compute_phase.resume();
            
            for(size_t i = 0; i < bufsize; i++) sum += buffer[i];
            delete[] buffer;

            compute_phase.stop();
            compute_phase.data()["sum"] = sum;
        }
    
        std::cout << compute_phase.gather_data().dump(4) << std::endl;

        // not in example:
        {
            auto const json = compute_phase.gather_data();
            CHECK(json[pm::JSON_KEY_DATA]["sum"] == -497952);
            CHECK(json[pm::JSON_KEY_METRICS]["memory"]["peak"] == 1'000'000);
        }
    }

    TEST_CASE("Noop") { 
        constexpr bool is_production = true;
        using Phase = std::conditional_t<is_production, pm::NoopPhase, pm::MemoryTimePhase>;

        // not in example:
        static_assert(std::is_same_v<Phase, pm::NoopPhase>);

        Phase compute_phase("Example");
        {
            int sum = 0;
            compute_phase.start();
        
            size_t const bufsize = 1'000'000;
            char* buffer = new char[bufsize];
            for(size_t i = 0; i < bufsize; i++) buffer[i] = (char)i;
            
            compute_phase.pause();
            std::cout << "initialization done!" << std::endl;
            compute_phase.resume();
            
            for(size_t i = 0; i < bufsize; i++) sum += buffer[i];
            delete[] buffer;
            compute_phase.stop();
            compute_phase.data()["sum"] = sum;
        }
    
        if constexpr(!is_production) std::cout << compute_phase.gather_data().dump(4) << std::endl;
    }

    TEST_CASE("Hierarchy") {
        pm::MemoryTimePhase compute_phase("Example");
        {
            int sum = 0;
            
            pm::TimePhase iota_phase("Iota");
            pm::TimePhase sum_phase("Sum");
            
            compute_phase.start();
            iota_phase.start();
            size_t const bufsize = 1'000'000;
            char* buffer = new char[bufsize];
            for(size_t i = 0; i < bufsize; i++) buffer[i] = (char)i;
            iota_phase.stop();

            compute_phase.pause();
            //std::cout << "initialization done!" << std::endl;
            compute_phase.resume();

            sum_phase.start();
            for(size_t i = 0; i < bufsize; i++) sum += buffer[i];
            sum_phase.stop();
            
            delete[] buffer;
            compute_phase.stop();
            compute_phase.append_child(iota_phase);
            compute_phase.append_child(sum_phase);
            compute_phase.data()["sum"] = sum;
        }

        std::cout << compute_phase.gather_data().dump(4) << std::endl;
    }
}

}
