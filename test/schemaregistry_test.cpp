#include <gtest/gtest.h>

#include <cstdint>
#include <string>

#include "vigine/config/configkey.h"
#include "vigine/config/configtree.h"
#include "vigine/config/configvalue.h"
#include "vigine/config/schemaregistry.h"

using vigine::config::ConfigKey;
using vigine::config::ConfigTable;
using vigine::config::ConfigType;
using vigine::config::ConfigValue;
using vigine::config::findValue;
using vigine::config::SchemaRegistry;
using vigine::config::setValue;
using vigine::config::ValidationResult;

TEST(SchemaRegistryTest, FillsDefaultsForMissingKeys)
{
    SchemaRegistry registry;
    registry.declare(ConfigKey{"window.width"}, ConfigType::Integer, ConfigValue{std::int64_t{800}});

    const ValidationResult result = registry.validate(ConfigValue{ConfigTable{}});
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.errors.empty());

    const ConfigValue *width = findValue(result.resolved, ConfigKey{"window.width"});
    ASSERT_NE(width, nullptr);
    EXPECT_EQ(width->valueOr<std::int64_t>(0), 800);
}

TEST(SchemaRegistryTest, ReportsTypeMismatch)
{
    SchemaRegistry registry;
    registry.declare(ConfigKey{"port"}, ConfigType::Integer, ConfigValue{std::int64_t{0}});

    ConfigValue tree{ConfigTable{}};
    setValue(tree, ConfigKey{"port"}, ConfigValue{std::string{"oops"}});

    const ValidationResult result = registry.validate(tree);
    EXPECT_FALSE(result.ok);
    ASSERT_EQ(result.errors.size(), 1u);
}

TEST(SchemaRegistryTest, PreservesPresentCorrectValue)
{
    SchemaRegistry registry;
    registry.declare(ConfigKey{"port"}, ConfigType::Integer, ConfigValue{std::int64_t{0}});

    ConfigValue tree{ConfigTable{}};
    setValue(tree, ConfigKey{"port"}, ConfigValue{std::int64_t{9090}});

    const ValidationResult result = registry.validate(tree);
    EXPECT_TRUE(result.ok);
    EXPECT_EQ(findValue(result.resolved, ConfigKey{"port"})->valueOr<std::int64_t>(0), 9090);
}

TEST(SchemaRegistryTest, RedeclareReplacesEntry)
{
    SchemaRegistry registry;
    registry.declare(ConfigKey{"x"}, ConfigType::Integer, ConfigValue{std::int64_t{1}});
    registry.declare(ConfigKey{"x"}, ConfigType::String, ConfigValue{std::string{"hi"}});

    ASSERT_EQ(registry.entries().size(), 1u);
    EXPECT_EQ(registry.entries().front().type, ConfigType::String);
}
