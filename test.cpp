#include "utils.h"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

TEST(test, test_print) {
  int x = 1;
  float y = 2.0;
  double z = 3.5;
  std::string s = "hello";
  std::vector<int> v1{1, 2, 3};
  std::vector<double> v2{1, 2, 3};
  std::vector<std::string> v3{"ABC", "DEF"};
  std::vector<std::vector<int>> v4{{1, 2, 3}, {4, 5}, {6, 7, 8}};
  utils::print(1);
  utils::print(1.0);
  utils::print(1, 2);
  utils::print(1, 2.0);
  utils::print(1, 2, 3.5, 4, 5, 6, 7, 8.0);
  utils::print(1, x, 2, y, 3, z, 4, s, "abc");
  utils::print(1, 2, 3, x, y, z, s, v1, v2, v3, v4);

  utils::println(1);
  utils::println(1.0);
  utils::println(1, 2);
  utils::println(1, 2.0);
  utils::println(1, 2, 3.5, 4, 5, 6, 7, 8.0);
  utils::println(1, x, 2, y, 3, z, 4, s, "abc");
  utils::println(1, 2, 3, x, y, z, s, v1, v2, v3, v4);
  utils::println(1, 2, 3, x, y, z, s, v1, v2, v3, v4,
                 std::vector<int>{1, 2, 3, 4, 5},
                 std::vector<std::vector<int>>{{1, 2, 3, 4, 5}}, 's');
}

TEST(test, sum) {
  std::vector<int> v1{1, 2, 3};
  std::vector<std::vector<int>> v2{{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}};
  EXPECT_TRUE(utils::sum(v1) == 1 + 2 + 3);
  EXPECT_TRUE(utils::sum(v2) == 4 * (1 + 2 + 3));
}

TEST(test, prod) {
  std::vector<int> v1{1, 2, 3};
  std::vector<std::vector<int>> v2{{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}};
  EXPECT_TRUE(utils::prod(v1) == 1 * 2 * 3);
  EXPECT_TRUE(utils::prod(v2) ==
              (1 * 2 * 3) * (1 * 2 * 3) * (1 * 2 * 3) * (1 * 2 * 3));
}

TEST(test, numel) {
  std::vector<int> v1{1, 2, 3};
  std::vector<std::vector<int>> v2{{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}};
  EXPECT_TRUE(utils::numel(v1) == 1 * 2 * 3);
  EXPECT_TRUE(utils::numel(v2) ==
              (1 * 2 * 3) + (1 * 2 * 3) + (1 * 2 * 3) + (1 * 2 * 3));
}

TEST(test, map) {
  std::vector<int> v1{1, 2, 3, -1, -2, -3};
  auto v2 = utils::map([](int x) { return std::sin(x); }, v1);
  auto v3 = utils::map([](int x) { return x + 1; }, v1);
  utils::println("v2=", v2, "v3=", v3);
}

TEST(test, shape_to_string) {
  std::vector<int> v1{1, 2, 3, -1, -2, -3};
  std::vector<std::vector<int>> v2{{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}};
  utils::println(utils::shape_to_string(v1), utils::shape_to_string(v2));
}

TEST(test, zip1) {
  std::vector<int> v1{1, 2, 3, -1, -2, -3};
  std::vector<int> v2{2, 3, 4, 5, 6, 7};
  for (const auto &[x, y] : utils::zip(v1, v2)) {
    utils::println("x=", x, "y=", y);
  }
}

TEST(test, zip2) {
  std::vector<int> v1{1, 2, 3, -1, -2, -3};
  std::vector<int> v2{2, 3, 4, 5, 6, 7, 9, 9, 9, 9, 9, 9, 9, 9};
  for (const auto &[x, y] : utils::zip(v1, v2)) {
    utils::println("x=", x, "y=", y);
  }
}

TEST(test, zip3) {
  std::vector<int> v1{1, 2, 3, -1, -2, -3, 9, 9, 9, 9, 9, 9, 9};
  std::vector<int> v2{2, 3, 4, 5, 6, 7};
  int index = 0;
  for (const auto &[x, y] : utils::zip(v1, v2)) {
    utils::println("x=", x, "y=", y);
    EXPECT_TRUE(x == v1[index]);
    EXPECT_TRUE(y == v2[index]);
    index++;
  }
}

TEST(test, enumerate1) {
  std::vector<int> v1{1, 2, 3, 4, 5, 6, 7};
  int index = 0;
  for (const auto &[idx, val] : utils::enumerate(v1)) {
    utils::println("idx=", idx, "val=", val);
    EXPECT_TRUE(idx == index);
    EXPECT_TRUE(val == v1[index]);
    index++;
  }
}

TEST(test, enumerate2) {
  std::vector<std::vector<int>> v1{{1, 2, 3, 4, 5, 6, 7}, {0, 0, 0}, {1, 1, 1}};
  int index = 0;
  for (const auto &[idx, val] : utils::enumerate(v1)) {
    utils::println("idx=", idx, "val=", val);
  }
}

TEST(test, vunpack1) {
  std::vector<int> v1{1, 2, 3, 4, 5, 6, 7};
  auto [x1] = utils::vunpack<1>(v1);
  auto [y1, y2] = utils::vunpack<2>(v1);
  auto [z1, z2, z3] = utils::vunpack<3>(v1);
  auto [u1, u2, u3, u4, u5, u6, u7] = utils::vunpack<7>(v1);
  EXPECT_TRUE(x1 == 1 && y1 == 1 && y2 == 2 && z1 == 1 && z2 == 2 && z3 == 3 &&
              u1 == 1 && u2 == 2 && u3 == 3 && u4 == 4 && u5 == 5 && u6 == 6 &&
              u7 == 7);
  utils::println("x=", x1, "\n", "y=", y1, y2, "\n", "z=", z1, z2, z3, "\n",
                 "u=", u1, u2, u3, u4, u5, u6, u7);
}

TEST(test, vunpack2) {
  std::vector<std::vector<int>> v1{{1, 2, 3, 4}, {5, 6, 7}};
  auto [y1, y2] = utils::vunpack<2>(v1);
  utils::println(y1, y2);
}

TEST(test, max_min) {
  std::vector<int> v1{3, 4, 5, 6, 7, 99, -100};
  auto val1 = utils::max(1, 2, 3, 4, 5, 6);
  auto val2 = utils::max<7>(v1);
  utils::println(val1, val2);
}

TEST(test, select) {
  std::vector<int> v1{3, 4, 5, 6, 7, 99, -100};
  auto val1 = utils::select(v1, [](auto v) { return v % 2 == 0; });
  utils::println(val1);
}

// newton method
TEST(test, nest) {
  auto sqrt_2 =
      utils::nest([](double x) { return (x + 2.0 / x) / 2.0; }, 1.0, 5);
  EXPECT_TRUE(sqrt_2 - std::pow(2, 0.5) < 0.0001);
}

TEST(test, fold) {
  auto factorial10 = utils::fold([](int x, int y) { return x * y; }, 1, 2, 3, 4,
                                 5, 6, 7, 8, 9, 10);
  EXPECT_TRUE(factorial10 == 1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10);
}
