#include "cpu_testbench.h"
#include "Vdut___024root.h"
#include <iostream>
#include "../support/vbuddy.cpp"

unsigned int ticks = 0;

class PdfTestbench : public CpuTestbench
{
protected:
    void waitForDisplay()
    {
        // In the unchanged reference/pdf.s, _loop3 starts at this address.
        while (top->rootp->top__DOT__pc != 0xbfc00060u) {
            ASSERT_LT(ticks, 1000000u) << "PDF build did not finish";
            ASSERT_NO_FATAL_FAILURE(runSimulation());
        }
        if (std::getenv("VBUDDY")) {
            std::ifstream config("vbuddy.cfg");
            ASSERT_TRUE(config.is_open()) << "Create tb/vbuddy.cfg with the WSL serial-device path";
            ASSERT_EQ(vbdOpen(), 1) << "Check tb/vbuddy.cfg and the WSL serial connection";
            buddyOpen = true;
            vbdHeader("Gaussian PDF");
        }
        values.open(testDirectory + "/display.csv");
        ASSERT_TRUE(values.is_open());
        values << "bin,count\n";
    }

    uint32_t readBin()
    {
        runSimulation();  // LBU copies this bin into a0.
        const uint32_t value = top->a0;
        values << bin++ << ',' << value << '\n';
        if (buddyOpen)
            vbdPlot(value, 0, 255);
        runSimulation(2);  // ADDI and BNE advance to the next bin.
        return value;
    }

    void TearDown() override
    {
        if (buddyOpen)
            vbdClose();
        values.close();
        CpuTestbench::TearDown();
    }

    bool buddyOpen = false;
    unsigned bin = 0;
    std::ofstream values;
};

// Check each displayed bin after building the original Gaussian reference PDF.
TEST_F(PdfTestbench, GaussianDisplay)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("pdf", ".s", "reference"));
    ASSERT_NO_FATAL_FAILURE(setData("reference/gaussian.mem"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    ASSERT_NO_FATAL_FAILURE(waitForDisplay());

    // Bins 0 to 15.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 0.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 1.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 2.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 3.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 4.
    WAVE_ASSERT_EQ(readBin(), 2u);  // Bin 5.
    WAVE_ASSERT_EQ(readBin(), 2u);  // Bin 6.
    WAVE_ASSERT_EQ(readBin(), 1u);  // Bin 7.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 8.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 9.
    WAVE_ASSERT_EQ(readBin(), 1u);  // Bin 10.
    WAVE_ASSERT_EQ(readBin(), 8u);  // Bin 11.
    WAVE_ASSERT_EQ(readBin(), 2u);  // Bin 12.
    WAVE_ASSERT_EQ(readBin(), 5u);  // Bin 13.
    WAVE_ASSERT_EQ(readBin(), 6u);  // Bin 14.
    WAVE_ASSERT_EQ(readBin(), 9u);  // Bin 15.
    // Bins 16 to 31.
    WAVE_ASSERT_EQ(readBin(), 11u);  // Bin 16.
    WAVE_ASSERT_EQ(readBin(), 6u);  // Bin 17.
    WAVE_ASSERT_EQ(readBin(), 11u);  // Bin 18.
    WAVE_ASSERT_EQ(readBin(), 9u);  // Bin 19.
    WAVE_ASSERT_EQ(readBin(), 10u);  // Bin 20.
    WAVE_ASSERT_EQ(readBin(), 12u);  // Bin 21.
    WAVE_ASSERT_EQ(readBin(), 17u);  // Bin 22.
    WAVE_ASSERT_EQ(readBin(), 16u);  // Bin 23.
    WAVE_ASSERT_EQ(readBin(), 20u);  // Bin 24.
    WAVE_ASSERT_EQ(readBin(), 21u);  // Bin 25.
    WAVE_ASSERT_EQ(readBin(), 25u);  // Bin 26.
    WAVE_ASSERT_EQ(readBin(), 22u);  // Bin 27.
    WAVE_ASSERT_EQ(readBin(), 14u);  // Bin 28.
    WAVE_ASSERT_EQ(readBin(), 20u);  // Bin 29.
    WAVE_ASSERT_EQ(readBin(), 24u);  // Bin 30.
    WAVE_ASSERT_EQ(readBin(), 36u);  // Bin 31.
    // Bins 32 to 47.
    WAVE_ASSERT_EQ(readBin(), 32u);  // Bin 32.
    WAVE_ASSERT_EQ(readBin(), 34u);  // Bin 33.
    WAVE_ASSERT_EQ(readBin(), 40u);  // Bin 34.
    WAVE_ASSERT_EQ(readBin(), 46u);  // Bin 35.
    WAVE_ASSERT_EQ(readBin(), 33u);  // Bin 36.
    WAVE_ASSERT_EQ(readBin(), 41u);  // Bin 37.
    WAVE_ASSERT_EQ(readBin(), 41u);  // Bin 38.
    WAVE_ASSERT_EQ(readBin(), 39u);  // Bin 39.
    WAVE_ASSERT_EQ(readBin(), 46u);  // Bin 40.
    WAVE_ASSERT_EQ(readBin(), 65u);  // Bin 41.
    WAVE_ASSERT_EQ(readBin(), 40u);  // Bin 42.
    WAVE_ASSERT_EQ(readBin(), 38u);  // Bin 43.
    WAVE_ASSERT_EQ(readBin(), 52u);  // Bin 44.
    WAVE_ASSERT_EQ(readBin(), 62u);  // Bin 45.
    WAVE_ASSERT_EQ(readBin(), 65u);  // Bin 46.
    WAVE_ASSERT_EQ(readBin(), 47u);  // Bin 47.
    // Bins 48 to 63.
    WAVE_ASSERT_EQ(readBin(), 60u);  // Bin 48.
    WAVE_ASSERT_EQ(readBin(), 49u);  // Bin 49.
    WAVE_ASSERT_EQ(readBin(), 65u);  // Bin 50.
    WAVE_ASSERT_EQ(readBin(), 75u);  // Bin 51.
    WAVE_ASSERT_EQ(readBin(), 65u);  // Bin 52.
    WAVE_ASSERT_EQ(readBin(), 67u);  // Bin 53.
    WAVE_ASSERT_EQ(readBin(), 87u);  // Bin 54.
    WAVE_ASSERT_EQ(readBin(), 70u);  // Bin 55.
    WAVE_ASSERT_EQ(readBin(), 88u);  // Bin 56.
    WAVE_ASSERT_EQ(readBin(), 80u);  // Bin 57.
    WAVE_ASSERT_EQ(readBin(), 98u);  // Bin 58.
    WAVE_ASSERT_EQ(readBin(), 100u);  // Bin 59.
    WAVE_ASSERT_EQ(readBin(), 89u);  // Bin 60.
    WAVE_ASSERT_EQ(readBin(), 109u);  // Bin 61.
    WAVE_ASSERT_EQ(readBin(), 114u);  // Bin 62.
    WAVE_ASSERT_EQ(readBin(), 86u);  // Bin 63.
    // Bins 64 to 79.
    WAVE_ASSERT_EQ(readBin(), 105u);  // Bin 64.
    WAVE_ASSERT_EQ(readBin(), 111u);  // Bin 65.
    WAVE_ASSERT_EQ(readBin(), 125u);  // Bin 66.
    WAVE_ASSERT_EQ(readBin(), 138u);  // Bin 67.
    WAVE_ASSERT_EQ(readBin(), 121u);  // Bin 68.
    WAVE_ASSERT_EQ(readBin(), 138u);  // Bin 69.
    WAVE_ASSERT_EQ(readBin(), 127u);  // Bin 70.
    WAVE_ASSERT_EQ(readBin(), 108u);  // Bin 71.
    WAVE_ASSERT_EQ(readBin(), 136u);  // Bin 72.
    WAVE_ASSERT_EQ(readBin(), 129u);  // Bin 73.
    WAVE_ASSERT_EQ(readBin(), 131u);  // Bin 74.
    WAVE_ASSERT_EQ(readBin(), 144u);  // Bin 75.
    WAVE_ASSERT_EQ(readBin(), 149u);  // Bin 76.
    WAVE_ASSERT_EQ(readBin(), 151u);  // Bin 77.
    WAVE_ASSERT_EQ(readBin(), 143u);  // Bin 78.
    WAVE_ASSERT_EQ(readBin(), 147u);  // Bin 79.
    // Bins 80 to 95.
    WAVE_ASSERT_EQ(readBin(), 162u);  // Bin 80.
    WAVE_ASSERT_EQ(readBin(), 156u);  // Bin 81.
    WAVE_ASSERT_EQ(readBin(), 161u);  // Bin 82.
    WAVE_ASSERT_EQ(readBin(), 143u);  // Bin 83.
    WAVE_ASSERT_EQ(readBin(), 190u);  // Bin 84.
    WAVE_ASSERT_EQ(readBin(), 172u);  // Bin 85.
    WAVE_ASSERT_EQ(readBin(), 163u);  // Bin 86.
    WAVE_ASSERT_EQ(readBin(), 152u);  // Bin 87.
    WAVE_ASSERT_EQ(readBin(), 153u);  // Bin 88.
    WAVE_ASSERT_EQ(readBin(), 169u);  // Bin 89.
    WAVE_ASSERT_EQ(readBin(), 168u);  // Bin 90.
    WAVE_ASSERT_EQ(readBin(), 181u);  // Bin 91.
    WAVE_ASSERT_EQ(readBin(), 171u);  // Bin 92.
    WAVE_ASSERT_EQ(readBin(), 181u);  // Bin 93.
    WAVE_ASSERT_EQ(readBin(), 160u);  // Bin 94.
    WAVE_ASSERT_EQ(readBin(), 166u);  // Bin 95.
    // Bins 96 to 111.
    WAVE_ASSERT_EQ(readBin(), 189u);  // Bin 96.
    WAVE_ASSERT_EQ(readBin(), 167u);  // Bin 97.
    WAVE_ASSERT_EQ(readBin(), 176u);  // Bin 98.
    WAVE_ASSERT_EQ(readBin(), 173u);  // Bin 99.
    WAVE_ASSERT_EQ(readBin(), 167u);  // Bin 100.
    WAVE_ASSERT_EQ(readBin(), 171u);  // Bin 101.
    WAVE_ASSERT_EQ(readBin(), 158u);  // Bin 102.
    WAVE_ASSERT_EQ(readBin(), 146u);  // Bin 103.
    WAVE_ASSERT_EQ(readBin(), 168u);  // Bin 104.
    WAVE_ASSERT_EQ(readBin(), 150u);  // Bin 105.
    WAVE_ASSERT_EQ(readBin(), 168u);  // Bin 106.
    WAVE_ASSERT_EQ(readBin(), 200u);  // Bin 107.
    WAVE_ASSERT_EQ(readBin(), 188u);  // Bin 108.
    WAVE_ASSERT_EQ(readBin(), 153u);  // Bin 109.
    WAVE_ASSERT_EQ(readBin(), 168u);  // Bin 110.
    WAVE_ASSERT_EQ(readBin(), 163u);  // Bin 111.
    // Bins 112 to 127.
    WAVE_ASSERT_EQ(readBin(), 166u);  // Bin 112.
    WAVE_ASSERT_EQ(readBin(), 186u);  // Bin 113.
    WAVE_ASSERT_EQ(readBin(), 148u);  // Bin 114.
    WAVE_ASSERT_EQ(readBin(), 168u);  // Bin 115.
    WAVE_ASSERT_EQ(readBin(), 154u);  // Bin 116.
    WAVE_ASSERT_EQ(readBin(), 136u);  // Bin 117.
    WAVE_ASSERT_EQ(readBin(), 157u);  // Bin 118.
    WAVE_ASSERT_EQ(readBin(), 155u);  // Bin 119.
    WAVE_ASSERT_EQ(readBin(), 141u);  // Bin 120.
    WAVE_ASSERT_EQ(readBin(), 172u);  // Bin 121.
    WAVE_ASSERT_EQ(readBin(), 137u);  // Bin 122.
    WAVE_ASSERT_EQ(readBin(), 139u);  // Bin 123.
    WAVE_ASSERT_EQ(readBin(), 145u);  // Bin 124.
    WAVE_ASSERT_EQ(readBin(), 134u);  // Bin 125.
    WAVE_ASSERT_EQ(readBin(), 142u);  // Bin 126.
    WAVE_ASSERT_EQ(readBin(), 134u);  // Bin 127.
    // Bins 128 to 143.
    WAVE_ASSERT_EQ(readBin(), 137u);  // Bin 128.
    WAVE_ASSERT_EQ(readBin(), 138u);  // Bin 129.
    WAVE_ASSERT_EQ(readBin(), 114u);  // Bin 130.
    WAVE_ASSERT_EQ(readBin(), 106u);  // Bin 131.
    WAVE_ASSERT_EQ(readBin(), 121u);  // Bin 132.
    WAVE_ASSERT_EQ(readBin(), 128u);  // Bin 133.
    WAVE_ASSERT_EQ(readBin(), 125u);  // Bin 134.
    WAVE_ASSERT_EQ(readBin(), 99u);  // Bin 135.
    WAVE_ASSERT_EQ(readBin(), 105u);  // Bin 136.
    WAVE_ASSERT_EQ(readBin(), 110u);  // Bin 137.
    WAVE_ASSERT_EQ(readBin(), 106u);  // Bin 138.
    WAVE_ASSERT_EQ(readBin(), 87u);  // Bin 139.
    WAVE_ASSERT_EQ(readBin(), 87u);  // Bin 140.
    WAVE_ASSERT_EQ(readBin(), 82u);  // Bin 141.
    WAVE_ASSERT_EQ(readBin(), 89u);  // Bin 142.
    WAVE_ASSERT_EQ(readBin(), 62u);  // Bin 143.
    // Bins 144 to 159.
    WAVE_ASSERT_EQ(readBin(), 85u);  // Bin 144.
    WAVE_ASSERT_EQ(readBin(), 62u);  // Bin 145.
    WAVE_ASSERT_EQ(readBin(), 89u);  // Bin 146.
    WAVE_ASSERT_EQ(readBin(), 75u);  // Bin 147.
    WAVE_ASSERT_EQ(readBin(), 61u);  // Bin 148.
    WAVE_ASSERT_EQ(readBin(), 59u);  // Bin 149.
    WAVE_ASSERT_EQ(readBin(), 61u);  // Bin 150.
    WAVE_ASSERT_EQ(readBin(), 57u);  // Bin 151.
    WAVE_ASSERT_EQ(readBin(), 52u);  // Bin 152.
    WAVE_ASSERT_EQ(readBin(), 54u);  // Bin 153.
    WAVE_ASSERT_EQ(readBin(), 53u);  // Bin 154.
    WAVE_ASSERT_EQ(readBin(), 52u);  // Bin 155.
    WAVE_ASSERT_EQ(readBin(), 35u);  // Bin 156.
    WAVE_ASSERT_EQ(readBin(), 45u);  // Bin 157.
    WAVE_ASSERT_EQ(readBin(), 44u);  // Bin 158.
    WAVE_ASSERT_EQ(readBin(), 29u);  // Bin 159.
    // Bins 160 to 175.
    WAVE_ASSERT_EQ(readBin(), 46u);  // Bin 160.
    WAVE_ASSERT_EQ(readBin(), 48u);  // Bin 161.
    WAVE_ASSERT_EQ(readBin(), 45u);  // Bin 162.
    WAVE_ASSERT_EQ(readBin(), 39u);  // Bin 163.
    WAVE_ASSERT_EQ(readBin(), 28u);  // Bin 164.
    WAVE_ASSERT_EQ(readBin(), 33u);  // Bin 165.
    WAVE_ASSERT_EQ(readBin(), 21u);  // Bin 166.
    WAVE_ASSERT_EQ(readBin(), 22u);  // Bin 167.
    WAVE_ASSERT_EQ(readBin(), 22u);  // Bin 168.
    WAVE_ASSERT_EQ(readBin(), 24u);  // Bin 169.
    WAVE_ASSERT_EQ(readBin(), 29u);  // Bin 170.
    WAVE_ASSERT_EQ(readBin(), 26u);  // Bin 171.
    WAVE_ASSERT_EQ(readBin(), 22u);  // Bin 172.
    WAVE_ASSERT_EQ(readBin(), 20u);  // Bin 173.
    WAVE_ASSERT_EQ(readBin(), 10u);  // Bin 174.
    WAVE_ASSERT_EQ(readBin(), 11u);  // Bin 175.
    // Bins 176 to 191.
    WAVE_ASSERT_EQ(readBin(), 13u);  // Bin 176.
    WAVE_ASSERT_EQ(readBin(), 12u);  // Bin 177.
    WAVE_ASSERT_EQ(readBin(), 12u);  // Bin 178.
    WAVE_ASSERT_EQ(readBin(), 11u);  // Bin 179.
    WAVE_ASSERT_EQ(readBin(), 14u);  // Bin 180.
    WAVE_ASSERT_EQ(readBin(), 7u);  // Bin 181.
    WAVE_ASSERT_EQ(readBin(), 7u);  // Bin 182.
    WAVE_ASSERT_EQ(readBin(), 8u);  // Bin 183.
    WAVE_ASSERT_EQ(readBin(), 3u);  // Bin 184.
    WAVE_ASSERT_EQ(readBin(), 4u);  // Bin 185.
    WAVE_ASSERT_EQ(readBin(), 4u);  // Bin 186.
    WAVE_ASSERT_EQ(readBin(), 2u);  // Bin 187.
    WAVE_ASSERT_EQ(readBin(), 2u);  // Bin 188.
    WAVE_ASSERT_EQ(readBin(), 1u);  // Bin 189.
    WAVE_ASSERT_EQ(readBin(), 3u);  // Bin 190.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 191.
    // Bins 192 to 207.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 192.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 193.
    WAVE_ASSERT_EQ(readBin(), 1u);  // Bin 194.
    WAVE_ASSERT_EQ(readBin(), 1u);  // Bin 195.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 196.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 197.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 198.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 199.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 200.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 201.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 202.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 203.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 204.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 205.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 206.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 207.
    // Bins 208 to 223.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 208.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 209.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 210.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 211.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 212.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 213.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 214.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 215.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 216.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 217.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 218.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 219.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 220.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 221.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 222.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 223.
    // Bins 224 to 239.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 224.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 225.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 226.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 227.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 228.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 229.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 230.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 231.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 232.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 233.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 234.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 235.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 236.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 237.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 238.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 239.
    // Bins 240 to 254.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 240.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 241.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 242.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 243.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 244.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 245.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 246.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 247.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 248.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 249.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 250.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 251.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 252.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 253.
    WAVE_ASSERT_EQ(readBin(), 0u);  // Bin 254.
}
