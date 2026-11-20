#pragma once

#include <memory>
#include <string>
#include <cstdlib>

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
#ifndef __APPLE__
        tfp = std::make_unique<VerilatedVcdC>();
        Verilated::traceEverOn(true);
        top->trace(tfp.get(), 99);
        const auto *test = ::testing::UnitTest::GetInstance()->current_test_info();
        const char *dir = std::getenv("WAVE_DIR");
        std::string path = std::string(dir ? dir : ".") + "/" + test->test_suite_name() + "." + test->name() + ".vcd";
        tfp->open(path.c_str());
#endif
        initializeInputs();
    }

    void TearDown() override
    {
        top->final();
#ifndef __APPLE__
        tfp->close();
#endif
    }

    virtual void initializeInputs() {}

protected:
    void sample()
    {
        top->eval();
#ifndef __APPLE__
        tfp->dump(sampleTime++);
#endif
    }

    unsigned sampleTime = 0;
    std::unique_ptr<Vdut> top;
#ifndef __APPLE__
    std::unique_ptr<VerilatedVcdC> tfp;
#endif
};