#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <thread>

#include <pm.hpp>

namespace pm::test {

using namespace pm;

TEST_SUITE("pm") {
    TEST_CASE("Stopwatch") {
        Stopwatch s;
        CHECK(s.elapsed_time_millis() == 0);

        SUBCASE("single") {
            s.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            s.stop();
            CHECK(s.elapsed_time_millis() >= 10.0);
        }

        SUBCASE("pause_resume") {
            s.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            s.pause();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            s.resume();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            s.stop();
            CHECK(s.elapsed_time_millis() >= 20.0);
            CHECK(s.elapsed_time_millis() < 30.0);
        }
    }

    TEST_CASE("MallocCounter") {
        // because the malloc override is disabled, we shouldn't be counting anything!
        SUBCASE("basic") {
            MallocCounter c;
            c.start();
            {
                char* array = new char[1024];
                array[0] = 0;
                CHECK(c.count() == 0);
                CHECK(c.peak() == 0);
                delete[] array;
            }
            c.stop();

            CHECK(c.count() == 0);
            CHECK(c.peak() == 0);
            CHECK(c.alloc_num() == 0);
            CHECK(c.alloc_bytes() == 0);
            CHECK(c.free_num() == 0);
            CHECK(c.free_bytes() == 0);
        }
    }

    TEST_CASE("Phase") {
        Phase<MallocCounter, Stopwatch> phase("test");
        phase.start();
        {
            char* array = new char[1024];
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            delete[] array;
        }
        phase.stop();

        CHECK(phase.meter<1>().elapsed_time_millis() >= 10);
        CHECK(phase.meter<0>().peak() == 0); // nb: malloc override is disabled
    }

    TEST_CASE("Result") {
        SUBCASE("primitive") {
            Result r;
            r.add("str", "test");
            r.add("int", -1337);
            r.add("double", 3.125); // use something that is unlikely to cause rounding errors on any system
            r.add("bool", false);
            r.sort();
            CHECK(r.str() == "RESULT bool=false double=3.125 int=-1337 str=test");
        }

        SUBCASE("phase") {
            TimePhase phase("test");
            phase.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            phase.stop();
            phase.data()["int"] = -1337;

            Result r;
            r.add(phase);
            r.sort();

            // nb: we cannot match the exact output here, because the sleep introduces randomness
            CHECK(r.str().starts_with("RESULT data.int=-1337 metrics.time="));
        }
    }
}

}
