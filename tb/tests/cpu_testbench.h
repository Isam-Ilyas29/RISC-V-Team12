#pragma once

#include "testbench.h"
#include <filesystem>
#include <fstream>

class CpuTestbench : public Testbench
{
public:
    void SetUp() override
    {
        ticks = 0;
    }

    void setupTest(const std::string &name, const std::string &extension = ".s",
                   const std::string &folder = "asm")
    {
        testDirectory = "test_out/" + name;
        ASSERT_EQ(std::system(("bash assemble.sh " + folder + "/" + name + extension).c_str()), 0);
        std::ofstream data("data.hex");
        ASSERT_TRUE(data.is_open());
    }

    void setData(const std::string &filename)
    {
        std::ifstream source(filename);
        std::ofstream data("data.hex");
        ASSERT_TRUE(source.is_open());
        ASSERT_TRUE(data.is_open());
        // The brief's input samples start at byte address 0x10000.
        data << "@10000\n" << source.rdbuf();
        ASSERT_TRUE(data.good());
    }

    void initSimulation()
    {
        BaseTestbench::SetUp();
        ASSERT_FALSE(HasFatalFailure());
        reset();
    }

    void TearDown() override
    {
        if (!top)
            return;
        BaseTestbench::TearDown();
        if (!testDirectory.empty()) {
            std::filesystem::copy_file("program.hex", testDirectory + "/program.hex",
                                      std::filesystem::copy_options::overwrite_existing);
            std::filesystem::copy_file("data.hex", testDirectory + "/data.hex",
                                      std::filesystem::copy_options::overwrite_existing);
        }
    }

protected:
    std::string waveformPath() override
    {
        if (testDirectory.empty())
            return BaseTestbench::waveformPath();
        return testDirectory + "/waveform.vcd";
    }

    void initializeInputs() override
    {
        ticks = 0;
        top->clk = 0;
        top->rst = 1;
        top->trigger = 0;
    }

    void reset()
    {
        top->rst = 1;
        runSimulation(10);
        top->rst = 0;
    }

    uint32_t step()
    {
        const uint32_t previous = top->a0;
        top->eval();
        WAVE_EXPECT_EQ(top->a0, previous) << "a0 changed before rising edge " << ticks;
        runSimulation();
        return top->a0;
    }

    std::string testDirectory;
};
