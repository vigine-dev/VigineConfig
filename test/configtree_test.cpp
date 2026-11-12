#include "vigine/config/configkey.h"
#include "vigine/config/configtree.h"
#include "vigine/config/configvalue.h"
#include <vigine/unittest/unittest.h>

#include <cstdint>

using vigine::config::ConfigKey;
using vigine::config::ConfigTable;
using vigine::config::ConfigValue;
using vigine::config::findValue;
using vigine::config::setValue;

VIGINE_TEST(ConfigTreeTest, FindsNestedValue)
{
    ConfigValue tree{ConfigTable{}};
    setValue(tree, ConfigKey{"window.size.width"}, ConfigValue{std::int64_t{800}});
    const ConfigValue *found = findValue(tree, ConfigKey{"window.size.width"});
    VIGINE_ASSERT_NE(found, nullptr);
    VIGINE_EXPECT_EQ(found->integerOr(0), 800);
}

VIGINE_TEST(ConfigTreeTest, FindMissingReturnsNull)
{
    const ConfigValue tree{ConfigTable{}};
    VIGINE_EXPECT_EQ(findValue(tree, ConfigKey{"absent.key"}), nullptr);
}

VIGINE_TEST(ConfigTreeTest, FindRootReturnsTreeItself)
{
    const ConfigValue tree{ConfigTable{}};
    VIGINE_EXPECT_EQ(findValue(tree, ConfigKey{}), &tree);
}

VIGINE_TEST(ConfigTreeTest, SetCreatesIntermediateTables)
{
    ConfigValue tree;
    setValue(tree, ConfigKey{"a.b.c"}, ConfigValue{true});

    const ConfigValue *leaf = findValue(tree, ConfigKey{"a.b.c"});
    VIGINE_ASSERT_NE(leaf, nullptr);
    VIGINE_EXPECT_TRUE(leaf->booleanOr(false));

    const ConfigValue *intermediate = findValue(tree, ConfigKey{"a"});
    VIGINE_ASSERT_NE(intermediate, nullptr);
    VIGINE_EXPECT_TRUE(intermediate->isTable());
}

VIGINE_TEST(ConfigTreeTest, SetOverwritesExisting)
{
    ConfigValue tree{ConfigTable{}};
    setValue(tree, ConfigKey{"k"}, ConfigValue{std::int64_t{1}});
    setValue(tree, ConfigKey{"k"}, ConfigValue{std::int64_t{2}});
    VIGINE_EXPECT_EQ(findValue(tree, ConfigKey{"k"})->integerOr(0), 2);
}
