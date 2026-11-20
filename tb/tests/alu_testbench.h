#pragma once

#include <memory>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <bitset>

#include "Vdut.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "gtest/gtest.h"

#define MAX_SIM_CYCLES 10000

extern unsigned int ticks;

class AluWaveTestbench : public ::testing::Test
{
public:
    void SetUp() override
    {
        top = std::make_unique<Vdut>();
        tfp = std::make_unique<VerilatedVcdC>();
        Verilated::traceEverOn(true);
        top->trace(tfp.get(), 99);
        const auto *test = ::testing::UnitTest::GetInstance()->current_test_info();
        const char *dir = std::getenv("WAVE_DIR");
        std::filesystem::create_directories(dir ? dir : ".");
        std::string path = std::string(dir ? dir : ".") + "/" + test->test_suite_name() + "." + test->name() + ".vcd";
        tfp->open(path.c_str());
        ASSERT_TRUE(tfp->isOpen()) << "Cannot open trace: " << path;
        status.open(path + ".status");
        ASSERT_TRUE(status.is_open());
        emitStatus(false, false);
        initializeInputs();
    }

    void TearDown() override
    {
        failed |= HasFailure();
        done = true;
        emitStatus(false, false);
        ++sampleTime;
        emitStatus(false, false);
        status.close();
        top->final();
        tfp->close();
    }

    virtual void initializeInputs() {}

protected:

    // Sample the DUT and dump the waveforms
    void sample()
    {
        top->eval();
        tfp->dump(sampleTime++);
    }

    // Check if actual and expected values are equal, and emit status information
    template<class A, class B>
    bool checkEqual(A actual, B expected)
    {
        const bool pass = actual == expected;
        ++checkId;
        actualValue = static_cast<uint64_t>(actual);
        expectedValue = static_cast<uint64_t>(expected);
        failed |= !pass;
        tfp->dump(sampleTime);
        emitStatus(true, pass);
        ++sampleTime;
        emitStatus(false, pass);
        ++sampleTime;
        return pass;
    }

    // Emit status information to the status file
    void emitStatus(bool valid, bool pass)
    {
        status << '#' << sampleTime << '\n'
               << valid << "status_valid\n" << pass << "status_check_pass\n"
               << failed << "status_failed\n" << done << "status_done\n"
               << (done && !failed) << "status_pass\n"
               << 'b' << std::bitset<32>(checkId) << " status_id\n"
               << 'b' << std::bitset<64>(actualValue) << " status_actual\n"
               << 'b' << std::bitset<64>(expectedValue) << " status_expected\n";
    }

    // Member variables
    std::ofstream status;
    bool failed = false, done = false;
    uint32_t checkId = 0;
    uint64_t actualValue = 0, expectedValue = 0;
    unsigned sampleTime = 0;
    std::unique_ptr<Vdut> top;
    std::unique_ptr<VerilatedVcdC> tfp;
};

// Macros for checking equality and asserting equality
#define WAVE_EXPECT_EQ(actual, expected) EXPECT_TRUE(checkEqual((actual), (expected)))
#define WAVE_ASSERT_EQ(actual, expected) ASSERT_TRUE(checkEqual((actual), (expected)))
