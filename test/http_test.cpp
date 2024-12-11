#include "http_base.h"
#include <gtest/gtest.h>

// Test case for the MyClass::add function
TEST(HttpReqBaseTest, Get) {
  HttpReqBase obj;
  EXPECT_EQ(obj.add(1, 1), 2);
  EXPECT_EQ(obj.add(3, 4), 7);
}

TEST(MyClassTest, AddNegativeNumbers) {
  MyClass obj;
  EXPECT_EQ(obj.add(-1, -1), -2);
  EXPECT_EQ(obj.add(-3, -4), -7);
}
