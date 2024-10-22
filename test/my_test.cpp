#include<gtest/gtest.h>

#include"../src/dvm_app.hpp"

namespace {
  int GetNumber() { return 35; }
}

TEST(TestTopic, DoesNotCrash) {
  EXPECT_EQ(GetNumber(), 35);
}