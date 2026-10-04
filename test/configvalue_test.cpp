#include "vigine/config/configvalue.h"

#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <utility>

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
    EXPECT_EQ(value.integerOr(0), 8080);
}

TEST(ConfigValueTest, ValueOrFallsBackOnTypeMismatch)
{
    const ConfigValue value{std::string{"text"}};
    EXPECT_TRUE(value.isString());
    EXPECT_EQ(value.integerOr(-1), -1);
}

TEST(ConfigValueTest, NestedContainersCompareByValue)
{
    ConfigTable left;
    left.emplace("port", ConfigValue{8080});
    left.emplace("tags",
                 ConfigValue{
                     ConfigArray{ConfigValue{std::string{"a"}}, ConfigValue{std::string{"b"}}}
    });

    ConfigTable right;
    right.emplace("port", ConfigValue{8080});
    right.emplace("tags",
                  ConfigValue{
                      ConfigArray{ConfigValue{std::string{"a"}}, ConfigValue{std::string{"b"}}}
    });

    EXPECT_EQ(ConfigValue{std::move(left)}, ConfigValue{std::move(right)});
}

TEST(ConfigValueTest, EveryIntegerWidthStoresAnInteger)
{
    EXPECT_EQ(ConfigValue{static_cast<unsigned char>(7)}.type(), ConfigType::Integer);
    EXPECT_EQ(ConfigValue{static_cast<short>(-3)}.type(), ConfigType::Integer);
    EXPECT_EQ(ConfigValue{7U}.type(), ConfigType::Integer);
    EXPECT_EQ(ConfigValue{7L}.type(), ConfigType::Integer);
    EXPECT_EQ(ConfigValue{std::size_t{7}}.type(), ConfigType::Integer);
    EXPECT_EQ(ConfigValue{7LL}.integerOr(0), 7);
    EXPECT_EQ(ConfigValue{1.5F}.type(), ConfigType::Floating);
}

TEST(ConfigValueTest, EachKindAnswersOnlyItsOwnAccessors)
{
    ConfigValue table{ConfigTable{{"port", ConfigValue{8080}}}};
    EXPECT_TRUE(table.isTable());
    EXPECT_FALSE(table.isArray());
    EXPECT_EQ(table.getIfArray(), nullptr);
    ASSERT_NE(table.getIfTable(), nullptr);
    (*table.getIfTable())["name"] = ConfigValue{"map"};
    EXPECT_EQ(table.getIfTable()->at("name").stringOr({}), "map");

    const ConfigValue flag{true};
    EXPECT_TRUE(flag.isBoolean());
    EXPECT_TRUE(flag.booleanOr(false));
    EXPECT_EQ(flag.integerOr(5), 5);
    EXPECT_EQ(flag.floatingOr(2.5), 2.5);
    EXPECT_EQ(flag.stringOr("none"), "none");
    EXPECT_EQ(flag.getIfString(), nullptr);
    EXPECT_FALSE(ConfigValue{2.0}.isInteger());
    EXPECT_TRUE(ConfigValue{2.0}.isFloating());
    EXPECT_TRUE(ConfigValue{ConfigArray{}}.isArray());
}
