#if defined(_DEBUG)
#include "gmock/gmock.h"
#include "attendance.h"

int main() {
    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}
#endif