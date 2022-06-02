#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <thread>

#include <pm.hpp>

namespace pm::test {

using namespace pm;

TEST_SUITE("pm_malloc") {
    class TestCallback : public MallocCallback {
    public:
        TestCallback() : MallocCallback() {
            register_callback();
        }

        size_t current = 0;
        size_t peak = 0;

        virtual void on_alloc(size_t bytes) override {
            current += bytes;
            peak = std::max(peak, current);
        }

        virtual void on_free(size_t bytes) override {
            current -= bytes;
        }
    };

    TEST_CASE("MallocCallback") {
        SUBCASE("basic") {
            TestCallback cb;
            {
                char* array = new char[1024];
                array[0] = 0;
                CHECK(cb.current == 1024);
                CHECK(cb.peak == 1024);
                delete[] array;
            }

            CHECK(cb.current == 0);
            CHECK(cb.peak == 1024);
        }

        SUBCASE("peak") {
            TestCallback cb;

            for(size_t i = 0; i <= 10; i++) {
                char* array = new char[1U << i];
                CHECK(cb.current == 1U << i);
                CHECK(cb.peak == 1U << i);
                array[0] = 0;
                delete[] array;
                CHECK(cb.current == 0);
                CHECK(cb.peak == 1U << i);
            }
            for(size_t i = 10; i > 0; i--) {
                char* array = new char[1U << (i-1)];
                CHECK(cb.current == 1U << (i-1));
                array[0] = 0;
                delete[] array;
                CHECK(cb.current == 0);
                CHECK(cb.peak == 1024);
            }
        }

        SUBCASE("multiple") {
            TestCallback cb1;
            char* array1 = new char[1024];
            array1[0] = 0;
            {
                TestCallback cb2;
                char* array2 = new char[1024];
                array2[0] = 0;
                delete[] array2;
                CHECK(cb2.current == 0);
                CHECK(cb2.peak == 1024);
            }
            delete[] array1;
            CHECK(cb1.current == 0);
            CHECK(cb1.peak == 2048);
        }
    }

    TEST_CASE("MallocCounter") {
        SUBCASE("basic") {
            MallocCounter c;
            c.start();
            {
                char* array = new char[1024];
                array[0] = 0;
                CHECK(c.count() == 1024);
                CHECK(c.peak() == 1024);
                delete[] array;
            }
            c.stop();

            CHECK(c.count() == 0);
            CHECK(c.peak() == 1024);
            CHECK(c.alloc_num() == 1);
            CHECK(c.alloc_bytes() == 1024);
            CHECK(c.free_num() == 1);
            CHECK(c.free_bytes() == 1024);
        }

        SUBCASE("pause") {
            MallocCounter c;
            c.start();
            {
                c.pause();
                char* array = new char[1024];
                array[0] = 0;
                CHECK(c.count() == 0);
                CHECK(c.peak() == 0);
                delete[] array;
                c.resume();
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
        CHECK(phase.meter<0>().peak() == 1024);
    }
}

}
