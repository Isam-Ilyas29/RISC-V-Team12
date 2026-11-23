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

class BaseTestbench : public ::testing::Test
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
        const std::filesystem::path directory = dir ? dir : ".";
        std::filesystem::create_directories(directory);
        const std::string filename = std::string(test->test_suite_name()) + "." + test->name() + ".vcd";
        const std::string path = (directory / filename).string();
        tfp->open(path.c_str());
        ASSERT_TRUE(tfp->isOpen()) << "Cannot open trace: " << path;
        status.open(path + ".status");
        ASSERT_TRUE(status.is_open());
        emitStatus(false, false);
        checks.open(path + ".checks.tsv");
        ASSERT_TRUE(checks.is_open());
        checks << "check_id\ttime\tsource\tactual_expression\texpected_expression\tactual\texpected\tpass\n";
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
        checks.close();
        top->final();
        tfp->close();
    }

    virtual void initializeInputs() {}

protected:

    void sample()
    {
        top->eval();
        tfp->dump(sampleTime++);
    }

    template<class A, class B>
    ::testing::AssertionResult checkEqual(A actual, B expected, const char *file, unsigned line,
                                          const char *actualExpression, const char *expectedExpression)
    {
        const bool pass = actual == expected;
        ++checkId;
        sourceLine = line;
        actualValue = static_cast<uint64_t>(actual);
        expectedValue = static_cast<uint64_t>(expected);
        failed |= !pass;
        checks << checkId << '\t' << sampleTime << '\t' << file << ':' << line << '\t'
               << actualExpression << '\t' << expectedExpression << '\t'
               << actualValue << '\t' << expectedValue << '\t' << pass << '\n';
        tfp->dump(sampleTime);
        emitStatus(true, pass);
        ++sampleTime;
        emitStatus(false, pass);
        ++sampleTime;
        if (pass)
            return ::testing::AssertionSuccess();
        return ::testing::AssertionFailure() << "check " << checkId << ": " << actualExpression
            << " = " << actualValue << ", expected " << expectedExpression << " = " << expectedValue;
    }

    void emitStatus(bool valid, bool pass)
    {
        status << '#' << sampleTime << '\n'
               << valid << "status_valid\n" << pass << "status_check_pass\n"
               << failed << "status_failed\n" << done << "status_done\n"
               << (done && !failed) << "status_pass\n"
               << 'b' << std::bitset<32>(sourceLine) << " status_line\n"
               << 'b' << std::bitset<32>(checkId) << " status_id\n"
               << 'b' << std::bitset<64>(actualValue) << " status_actual\n"
               << 'b' << std::bitset<64>(expectedValue) << " status_expected\n";
    }

    std::ofstream status, checks;
    bool failed = false, done = false;
    uint32_t checkId = 0, sourceLine = 0;
    uint64_t actualValue = 0, expectedValue = 0;
    unsigned sampleTime = 0;
    std::unique_ptr<Vdut> top;
    std::unique_ptr<VerilatedVcdC> tfp;
};

#define WAVE_EXPECT_EQ(actual, expected) EXPECT_TRUE(checkEqual((actual), (expected), __FILE__, __LINE__, #actual, #expected))
#define WAVE_ASSERT_EQ(actual, expected) ASSERT_TRUE(checkEqual((actual), (expected), __FILE__, __LINE__, #actual, #expected))
