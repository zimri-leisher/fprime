// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "FpySequencerTester.hpp"

TEST(Nominal, testDispatch) {
    Svc::FpySequencerTester tester;
    tester.testDispatch();
}

TEST(Nominal, testLogStatus) {
    Svc::FpySequencerTester tester;
    tester.testLogStatus();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
