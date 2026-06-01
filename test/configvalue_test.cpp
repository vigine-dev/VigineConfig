#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <utility>

#include "vigine/config/configvalue.h"

using vigine::config::ConfigArray;
using vigine::config::ConfigTable;
using vigine::config::ConfigType;
using vigine::config::ConfigValue;

TEST(ConfigValueTest, DefaultIsNone)
{
    const ConfigValue value;
    EXPECT_TRUE(value.isNone());
    EXPECT_EQ(value.type(), ConfigType::None);
}

TEST(ConfigValueTest, HoldsScalarKinds)
{
    EXPECT_EQ(ConfigValue{true}.type(), ConfigType::Boolean);
    EXPECT_EQ(ConfigValue{std::int64_t{42}}.type(), ConfigType::Integer);
    EXPECT_EQ(ConfigValue{3.5}.type(), ConfigType::Floating);
    EXPECT_EQ(ConfigValue{std::string{"hi"}}.type(), ConfigType::String);
}

TEST(ConfigValueTest, IntegerLiteralStaysInteger)
{
    const ConfigValue value{8080};
    EXPECT_EQ(value.type(), ConfigType::Integer);
    EXPECT_EQ(value.valueOr<std::int64_t>(0), 8080);
}

TEST(ConfigValueTest, ValueOrFallsBackOnTypeMismatch)
{
    const ConfigValue value{std::string{"text"}};
    EXPECT_TRUE(value.is<std::string>());
    EXPECT_EQ(value.valueOr<std::int64_t>(-1), -1);
}

TEST(ConfigValueTest, NestedContainersCompareByValue)
{
    ConfigTable left;
    left.emplace("port", ConfigValue{8080});
    left.emplace("tags", ConfigValue{ConfigArray{ConfigValue{std::string{"a"}}, ConfigValue{std::string{"b"}}}});

    ConfigTable right;
    right.emplace("port", ConfigValue{8080});
    right.emplace("tags", ConfigValue{ConfigArray{ConfigValue{std::string{"a"}}, ConfigValue{std::string{"b"}}}});

    EXPECT_EQ(ConfigValue{std::move(left)}, ConfigValue{std::move(right)});
}
