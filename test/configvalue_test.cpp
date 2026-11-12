#include "vigine/config/configvalue.h"
#include <vigine/unittest/unittest.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

using vigine::config::ConfigArray;
using vigine::config::ConfigTable;
using vigine::config::ConfigType;
using vigine::config::ConfigValue;

VIGINE_TEST(ConfigValueTest, DefaultIsNone)
{
    const ConfigValue value;
    VIGINE_EXPECT_TRUE(value.isNone());
    VIGINE_EXPECT_EQ(value.type(), ConfigType::None);
}

VIGINE_TEST(ConfigValueTest, HoldsScalarKinds)
{
    VIGINE_EXPECT_EQ(ConfigValue{true}.type(), ConfigType::Boolean);
    VIGINE_EXPECT_EQ(ConfigValue{std::int64_t{42}}.type(), ConfigType::Integer);
    VIGINE_EXPECT_EQ(ConfigValue{3.5}.type(), ConfigType::Floating);
    VIGINE_EXPECT_EQ(ConfigValue{std::string{"hi"}}.type(), ConfigType::String);
}

VIGINE_TEST(ConfigValueTest, IntegerLiteralStaysInteger)
{
    const ConfigValue value{8080};
    VIGINE_EXPECT_EQ(value.type(), ConfigType::Integer);
    VIGINE_EXPECT_EQ(value.integerOr(0), 8080);
}

VIGINE_TEST(ConfigValueTest, ValueOrFallsBackOnTypeMismatch)
{
    const ConfigValue value{std::string{"text"}};
    VIGINE_EXPECT_TRUE(value.isString());
    VIGINE_EXPECT_EQ(value.integerOr(-1), -1);
}

VIGINE_TEST(ConfigValueTest, NestedContainersCompareByValue)
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

    VIGINE_EXPECT_EQ(ConfigValue{std::move(left)}, ConfigValue{std::move(right)});
}

VIGINE_TEST(ConfigValueTest, EveryIntegerWidthStoresAnInteger)
{
    VIGINE_EXPECT_EQ(ConfigValue{static_cast<unsigned char>(7)}.type(), ConfigType::Integer);
    VIGINE_EXPECT_EQ(ConfigValue{static_cast<short>(-3)}.type(), ConfigType::Integer);
    VIGINE_EXPECT_EQ(ConfigValue{7U}.type(), ConfigType::Integer);
    VIGINE_EXPECT_EQ(ConfigValue{7L}.type(), ConfigType::Integer);
    VIGINE_EXPECT_EQ(ConfigValue{std::size_t{7}}.type(), ConfigType::Integer);
    VIGINE_EXPECT_EQ(ConfigValue{7LL}.integerOr(0), 7);
    VIGINE_EXPECT_EQ(ConfigValue{1.5F}.type(), ConfigType::Floating);
}

VIGINE_TEST(ConfigValueTest, EachKindAnswersOnlyItsOwnAccessors)
{
    ConfigValue table{ConfigTable{{"port", ConfigValue{8080}}}};
    VIGINE_EXPECT_TRUE(table.isTable());
    VIGINE_EXPECT_FALSE(table.isArray());
    VIGINE_EXPECT_EQ(table.getIfArray(), nullptr);
    VIGINE_ASSERT_NE(table.getIfTable(), nullptr);
    (*table.getIfTable())["name"] = ConfigValue{"map"};
    VIGINE_EXPECT_EQ(table.getIfTable()->at("name").stringOr({}), "map");

    const ConfigValue flag{true};
    VIGINE_EXPECT_TRUE(flag.isBoolean());
    VIGINE_EXPECT_TRUE(flag.booleanOr(false));
    VIGINE_EXPECT_EQ(flag.integerOr(5), 5);
    VIGINE_EXPECT_EQ(flag.floatingOr(2.5), 2.5);
    VIGINE_EXPECT_EQ(flag.stringOr("none"), "none");
    VIGINE_EXPECT_EQ(flag.getIfString(), nullptr);
    VIGINE_EXPECT_FALSE(ConfigValue{2.0}.isInteger());
    VIGINE_EXPECT_TRUE(ConfigValue{2.0}.isFloating());
    VIGINE_EXPECT_TRUE(ConfigValue{ConfigArray{}}.isArray());
}
