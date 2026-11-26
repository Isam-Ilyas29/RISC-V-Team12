#include "cpu_testbench.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "../support/vbuddy.cpp"

unsigned int ticks = 0;

class F1Testbench : public CpuTestbench
{
protected:
    static constexpr int startupCycles = 8;
    static constexpr unsigned lightStepCycles = 132;
    // The delay loop takes two cycles per count. The call and surrounding code add 11.
    static constexpr unsigned minFinalHold = 11 + 2 * (64 + 1);
    static constexpr unsigned maxFinalHold = 11 + 2 * (64 + 127);

    void SetUp() override
    {
        CpuTestbench::SetUp();
        ASSERT_NO_FATAL_FAILURE(setupTest("f1"));

        // Keep the trace and disassembly together for each test.
        const std::string listing = testDirectory + "/program.dis";
        const auto *test = testing::UnitTest::GetInstance()->current_test_info();
        testDirectory += "/";
        testDirectory += test->name();
        std::filesystem::create_directories(testDirectory);
        std::filesystem::copy_file(listing, testDirectory + "/program.dis",
                                  std::filesystem::copy_options::overwrite_existing);

        ASSERT_NO_FATAL_FAILURE(initSimulation());
        ASSERT_NO_FATAL_FAILURE(runSimulation(startupCycles));  // Run up to the button wait.
        lights.open(testDirectory + "/lights.csv");
        ASSERT_TRUE(lights.is_open());
        lights << "cycle,lights\n" << ticks << ',' << top->a0 << '\n';
    }

    uint32_t nextLight(unsigned expectedCycles = 0)
    {
        const uint32_t previous = top->a0;
        const unsigned startCycle = ticks;
        // Check the first change in a0. A bad pattern could be gone by the next light.
        while (top->a0 == previous && ticks - startCycle < 512 && !HasFatalFailure()) {
            runSimulation();
            // Slow down hardware playback to roughly one second per light.
            if (buddyOpen)
                std::this_thread::sleep_for(std::chrono::microseconds(7576));
        }
        if (top->a0 == previous)
            ADD_FAILURE() << "No light change within 512 cycles";
        // The first light can start at different points in the button polling loop.
        if (expectedCycles != 0)
            WAVE_EXPECT_EQ(ticks - startCycle, expectedCycles);
        lights << ticks << ',' << top->a0 << '\n';
        // The hardware test sends blackout separately so it can start the stopwatch.
        if (buddyOpen && top->a0 != 0)
            vbdBar(top->a0 & 0xff);
        return top->a0;
    }

    // Check from the second light onward so the test can release the button after the first.
    void checkLightsTwoThroughEight()
    {
        WAVE_ASSERT_EQ(nextLight(lightStepCycles), 0x03u);
        WAVE_ASSERT_EQ(nextLight(lightStepCycles), 0x07u);
        WAVE_ASSERT_EQ(nextLight(lightStepCycles), 0x0fu);
        WAVE_ASSERT_EQ(nextLight(lightStepCycles), 0x1fu);
        WAVE_ASSERT_EQ(nextLight(lightStepCycles), 0x3fu);
        WAVE_ASSERT_EQ(nextLight(lightStepCycles), 0x7fu);
        WAVE_ASSERT_EQ(nextLight(lightStepCycles), 0xffu);
    }

    void openBuddy()
    {
        // The supplied Vbuddy library expects a newline-terminated device path.
        std::ifstream config("vbuddy.cfg", std::ios::binary);
        ASSERT_TRUE(config.is_open()) << "Create tb/vbuddy.cfg with the WSL serial-device path";
        std::string device;
        std::getline(config, device);
        ASSERT_FALSE(device.empty());
        ASSERT_FALSE(config.eof()) << "End the device path with a newline";
        ASSERT_EQ(vbdOpen(), 1);
        buddyOpen = true;
        vbdSetMode(1);  // One-shot mode reports one event per press.
        vbdHeader("PRESS START");
        vbdBar(0);
    }

    void waitForButton(const char *prompt)
    {
        std::cout << prompt << std::endl;
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
        while (!vbdFlag()) {
            ASSERT_LT(std::chrono::steady_clock::now(), deadline) << "Button wait timed out";
            // Keep clocking the CPU so the LFSR advances before the start press.
            ASSERT_NO_FATAL_FAILURE(runSimulation());
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void TearDown() override
    {
        if (buddyOpen) {
            vbdBar(0);
            vbdClose();
        }
        lights.close();
        CpuTestbench::TearDown();
    }

    bool buddyOpen = false;
    std::ofstream lights;
};

// Run two starts and check the light patterns, timing and button release.
TEST_F(F1Testbench, SequenceAndRetrigger)
{
    WAVE_ASSERT_EQ(top->a0, 0u);
    // No trigger means no start.
    ASSERT_NO_FATAL_FAILURE(runSimulation(200));
    WAVE_ASSERT_EQ(top->a0, 0u);

    top->trigger = 1;
    WAVE_ASSERT_EQ(nextLight(), 0x01u);
    ASSERT_NO_FATAL_FAILURE(checkLightsTwoThroughEight());
    // 200 idle cycles advance the seed 20 times. The hold advances it once more to 15.
    WAVE_ASSERT_EQ(nextLight(169), 0x00u);

    // Holding the trigger high must leave the lights off after the sequence.
    ASSERT_NO_FATAL_FAILURE(runSimulation(512));
    WAVE_ASSERT_EQ(top->a0, 0u);
    top->trigger = 0;
    ASSERT_NO_FATAL_FAILURE(runSimulation(16));
    WAVE_ASSERT_EQ(top->a0, 0u);

    // A new press starts again; the second LFSR value gives a different final hold.
    top->trigger = 1;
    WAVE_ASSERT_EQ(nextLight(), 0x01u);
    top->trigger = 0;  // Releasing during the sequence must not interrupt it.
    ASSERT_NO_FATAL_FAILURE(checkLightsTwoThroughEight());
    WAVE_ASSERT_EQ(nextLight(381), 0x00u);  // The second hold uses LFSR value 121.
    ASSERT_NO_FATAL_FAILURE(runSimulation(200));
    WAVE_ASSERT_EQ(top->a0, 0u);
}

// Reset while the lights are on and the button is held.
TEST_F(F1Testbench, ResetDuringSequence)
{
    top->trigger = 1;
    WAVE_ASSERT_EQ(nextLight(), 0x01u);
    WAVE_ASSERT_EQ(nextLight(), 0x03u);
    WAVE_ASSERT_EQ(nextLight(), 0x07u);

    ASSERT_NO_FATAL_FAILURE(reset());
    ASSERT_NO_FATAL_FAILURE(runSimulation(startupCycles));
    WAVE_ASSERT_EQ(top->a0, 0u);
    ASSERT_NO_FATAL_FAILURE(runSimulation(200));
    WAVE_ASSERT_EQ(top->a0, 0u);
    top->trigger = 0;
    ASSERT_NO_FATAL_FAILURE(runSimulation(16));
    WAVE_ASSERT_EQ(top->a0, 0u);
    top->trigger = 1;
    WAVE_ASSERT_EQ(nextLight(), 0x01u);
}

// Run the lights and measure the second press with Vbuddy's own stopwatch.
TEST_F(F1Testbench, DISABLED_VbuddyLights)
{
    ASSERT_NO_FATAL_FAILURE(openBuddy());
    ASSERT_NO_FATAL_FAILURE(waitForButton("Press the rotary knob to start."));
    vbdClear();
    vbdHeader("PRESS AT GO");

    top->trigger = 1;
    WAVE_ASSERT_EQ(nextLight(), 0x01u);
    top->trigger = 0;
    ASSERT_NO_FATAL_FAILURE(checkLightsTwoThroughEight());
    const unsigned allLightsOn = ticks;
    WAVE_ASSERT_EQ(nextLight(), 0x00u);
    const unsigned finalHold = ticks - allLightsOn;
    ASSERT_GE(finalHold, minFinalHold);
    ASSERT_LE(finalHold, maxFinalHold);

    // Clear any queued early press. Start the stopwatch after sending lights-off.
    (void)vbdFlag();
    vbdBar(0);
    vbdInitWatch();
    ASSERT_NO_FATAL_FAILURE(waitForButton("Lights out. Press the rotary knob again."));

    const int reactionMs = vbdElapsed();
    ASSERT_GT(reactionMs, 0) << "Vbuddy did not return a positive reaction time";
    const std::string time = std::to_string(reactionMs) + " ms";
    vbdClear();
    vbdHeader(time.c_str());
    std::cout << "Reaction: " << time << '\n';

    std::ofstream report(testDirectory + "/reaction.txt");
    report << "Final hold: " << finalHold << " CPU cycles\n"
           << "Reaction: " << time << '\n';
    ASSERT_TRUE(report.good());
}
