#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "Serial/Serializer.hpp"

namespace serial {

struct MyTestContext : public BasicOutputContext<char*> {
  using Base = BasicOutputContext<char*>;

  using Base::AdvanceTo;
  using Base::BasicOutputContext;
  using Base::Output;
};

template <>
struct Serializer<int> {
  Serializer(std::string_view prefix = "INT ") : m_prefix{prefix} {}

  template <class Context>
  auto Serialize(int val, Context& ctx) const
      -> decltype(std::declval<Context&>().Output()) {
    std::string out{};

    if constexpr (std::is_same_v<Context, MyTestContext>) {
      out += "CUSTOM CONTEXT: ";
    }

    out += m_prefix;
    out += std::to_string(val);
    return std::copy(std::cbegin(out), std::cend(out), ctx.Output());
  }

 private:
  std::string m_prefix;
};

template <>
struct Serializer<double> {
  static constexpr std::string_view prefix = "DBL ";

  template <class Context>
  auto Serialize(double val, Context& ctx) const
      -> decltype(std::declval<Context&>().Output()) {
    std::string out{prefix};
    out += std::to_string(val);
    return std::copy(std::cbegin(out), std::cend(out), ctx.Output());
  }
};

template <>
struct Serializer<std::string> {
  template <class Context>
  auto Serialize(const std::string& val, Context& ctx) const
      -> decltype(std::declval<Context&>().Output()) {
    return std::copy(std::cbegin(val), std::cend(val), ctx.Output());
  }
};

namespace {

TEST(TestSerializer, BasicOutputContext) {
  std::array<int, 2> array;
  auto ctx = BasicOutputContext{array.begin()};

  EXPECT_EQ(ctx.Output(), array.begin());
  EXPECT_EQ(ctx.AdvanceTo(array.end()), array.end());
  EXPECT_EQ(ctx.Output(), array.end());
}

TEST(TestSerializer, Serialize) {
  std::array<char, 256> out_str;
  std::fill(std::begin(out_str), std::end(out_str), '\0');

  auto ctx = BasicOutputContext{std::begin(out_str)};

  // Does nothing without args
  EXPECT_NO_THROW(Serialize(ctx));
  EXPECT_EQ(ctx.Output(), std::begin(out_str));

  // Rvalue/Lvalue tests
  {
    int i = 2;
    constexpr std::string_view expected_output = "INT 1INT 2DBL 3.000000FOO";
    EXPECT_NO_THROW(Serialize(ctx, 1, i, 3., std::string{"FOO"}));

    EXPECT_EQ(ctx.Output(), std::begin(out_str) + expected_output.size());
    EXPECT_EQ(expected_output, std::string_view{out_str.data()});
  }

  std::fill(std::begin(out_str), std::end(out_str), '\0');
  ctx.AdvanceTo(std::begin(out_str));
  // With custom Serializers
  {
    constexpr std::string_view expected_output = "FOO 1";

    EXPECT_NO_THROW(
        Serialize(ctx, ForwardAsSerialArg(1, Serializer<int>{"FOO "})));

    EXPECT_EQ(ctx.Output(), std::begin(out_str) + expected_output.size());
    EXPECT_EQ(expected_output, std::string_view{out_str.data()});
  }
}

TEST(TestSerializer, SerializeCustomContext) {
  std::array<char, 256> out_str;
  std::fill(std::begin(out_str), std::end(out_str), '\0');

  auto ctx = MyTestContext{std::begin(out_str)};

  // Does nothing without args
  EXPECT_NO_THROW(Serialize(ctx));
  EXPECT_EQ(ctx.Output(), std::begin(out_str));

  // Rvalue/Lvalue tests
  {
    int i = 2;
    constexpr std::string_view expected_output =
        "CUSTOM CONTEXT: INT 1CUSTOM CONTEXT: INT 2DBL 3.000000FOO";
    EXPECT_NO_THROW(Serialize(ctx, 1, i, 3., std::string{"FOO"}));

    EXPECT_EQ(ctx.Output(), std::begin(out_str) + expected_output.size());
    EXPECT_EQ(expected_output, std::string_view{out_str.data()});
  }
}

TEST(TestSerializer, SerializeInto) {
  std::array<char, 256> out_str;
  std::fill(std::begin(out_str), std::end(out_str), '\0');

  auto begin = std::begin(out_str);

  // Does nothing without args
  EXPECT_NO_THROW({ EXPECT_EQ(SerializeInto(begin), begin); });

  // Rvalue/Lvalue tests
  {
    int i = 2;
    constexpr std::string_view expected_output = "INT 1INT 2DBL 3.000000FOO";
    EXPECT_NO_THROW({
      auto end = SerializeInto(begin, 1, i, 3., std::string{"FOO"});
      EXPECT_EQ(end, begin + expected_output.size());
      EXPECT_EQ(expected_output, std::string_view{out_str.data()});
    });
  }

  std::fill(std::begin(out_str), std::end(out_str), '\0');
  // With custom Serializers
  {
    constexpr std::string_view expected_output = "FOO 2";

    EXPECT_NO_THROW({
      auto end =
          SerializeInto(begin, ForwardAsSerialArg(2, Serializer<int>{"FOO "}));
      EXPECT_EQ(end, begin + expected_output.size());
      EXPECT_EQ(expected_output, std::string_view{out_str.data()});
    });
  }
}

}  // namespace

}  // namespace serial
