#include "vigine/config/configkey.h"
#include "vigine/config/configtree.h"
#include "vigine/config/configvalue.h"
#include "vigine/config/schemaregistry.h"
#include <vigine/unittest/unittest.h>

#include <cstdint>
#include <memory>
#include <string>

using vigine::config::ConfigKey;
using vigine::config::ConfigTable;
using vigine::config::ConfigType;
using vigine::config::ConfigValue;
using vigine::config::createSchemaRegistry;
using vigine::config::findValue;
using vigine::config::ISchemaRegistry;
using vigine::config::setValue;
using vigine::config::ValidationResult;

VIGINE_TEST(SchemaRegistryTest, FillsDefaultsForMissingKeys)
{
    const std::unique_ptr<ISchemaRegistry> registry = createSchemaRegistry();
    registry->declare(ConfigKey{"window.width"}, ConfigType::Integer,
                      ConfigValue{std::int64_t{800}});

    const ValidationResult result = registry->validate(ConfigValue{ConfigTable{}});
    VIGINE_EXPECT_TRUE(result.ok());
    VIGINE_EXPECT_TRUE(result.errors().empty());

    const ConfigValue *width = findValue(result.resolved(), ConfigKey{"window.width"});
    VIGINE_ASSERT_NE(width, nullptr);
    VIGINE_EXPECT_EQ(width->integerOr(0), 800);
}

VIGINE_TEST(SchemaRegistryTest, ReportsTypeMismatch)
{
    const std::unique_ptr<ISchemaRegistry> registry = createSchemaRegistry();
    registry->declare(ConfigKey{"port"}, ConfigType::Integer, ConfigValue{std::int64_t{0}});

    ConfigValue tree{ConfigTable{}};
    setValue(tree, ConfigKey{"port"}, ConfigValue{std::string{"oops"}});

    const ValidationResult result = registry->validate(tree);
    VIGINE_EXPECT_FALSE(result.ok());
    VIGINE_ASSERT_EQ(result.errors().size(), 1u);
}

VIGINE_TEST(SchemaRegistryTest, PreservesPresentCorrectValue)
{
    const std::unique_ptr<ISchemaRegistry> registry = createSchemaRegistry();
    registry->declare(ConfigKey{"port"}, ConfigType::Integer, ConfigValue{std::int64_t{0}});

    ConfigValue tree{ConfigTable{}};
    setValue(tree, ConfigKey{"port"}, ConfigValue{std::int64_t{9090}});

    const ValidationResult result = registry->validate(tree);
    VIGINE_EXPECT_TRUE(result.ok());
    VIGINE_EXPECT_EQ(findValue(result.resolved(), ConfigKey{"port"})->integerOr(0), 9090);
}

VIGINE_TEST(SchemaRegistryTest, RedeclareReplacesEntry)
{
    const std::unique_ptr<ISchemaRegistry> registry = createSchemaRegistry();
    registry->declare(ConfigKey{"x"}, ConfigType::Integer, ConfigValue{std::int64_t{1}});
    registry->declare(ConfigKey{"x"}, ConfigType::String, ConfigValue{std::string{"hi"}});

    VIGINE_ASSERT_EQ(registry->entries().size(), 1u);
    VIGINE_EXPECT_EQ(registry->entries().front().type(), ConfigType::String);
}
