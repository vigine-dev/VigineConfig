#include <gtest/gtest.h>

#include <cstdint>

#include "vigine/config/configkey.h"
#include "vigine/config/configtree.h"
#include "vigine/config/configvalue.h"

using vigine::config::ConfigKey;
using vigine::config::ConfigTable;
using vigine::config::ConfigValue;
using vigine::config::findValue;
using vigine::config::setValue;

TEST(ConfigTreeTest, FindsNestedValue)
{
    ConfigValue tree{ConfigTable{}};
    setValue(tree, ConfigKey{"window.size.width"}, ConfigValue{std::int64_t{800}});
    const ConfigValue *found = findValue(tree, ConfigKey{"window.size.width"});
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->valueOr<std::int64_t>(0), 800);
}

TEST(ConfigTreeTest, FindMissingReturnsNull)
{
    const ConfigValue tree{ConfigTable{}};
    EXPECT_EQ(findValue(tree, ConfigKey{"absent.key"}), nullptr);
}

TEST(ConfigTreeTest, FindRootReturnsTreeItself)
{
    const ConfigValue tree{ConfigTable{}};
    EXPECT_EQ(findValue(tree, ConfigKey{}), &tree);
}

TEST(ConfigTreeTest, SetCreatesIntermediateTables)
{
    ConfigValue tree;
    setValue(tree, ConfigKey{"a.b.c"}, ConfigValue{true});

    const ConfigValue *leaf = findValue(tree, ConfigKey{"a.b.c"});
    ASSERT_NE(leaf, nullptr);
    EXPECT_TRUE(leaf->valueOr<bool>(false));

    const ConfigValue *intermediate = findValue(tree, ConfigKey{"a"});
    ASSERT_NE(intermediate, nullptr);
    EXPECT_TRUE(intermediate->is<ConfigTable>());
}

TEST(ConfigTreeTest, SetOverwritesExisting)
{
    ConfigValue tree{ConfigTable{}};
    setValue(tree, ConfigKey{"k"}, ConfigValue{std::int64_t{1}});
    setValue(tree, ConfigKey{"k"}, ConfigValue{std::int64_t{2}});
    EXPECT_EQ(findValue(tree, ConfigKey{"k"})->valueOr<std::int64_t>(0), 2);
}
