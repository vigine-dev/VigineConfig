#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <utility>

#include "vigine/config/configkey.h"
#include "vigine/config/configtree.h"
#include "vigine/config/configvalue.h"
#include "vigine/config/layeredstore.h"

using vigine::config::ConfigKey;
using vigine::config::ConfigTable;
using vigine::config::ConfigValue;
using vigine::config::findValue;
using vigine::config::LayeredStore;
using vigine::config::setValue;

namespace
{
ConfigValue tableWith(const ConfigKey &key, ConfigValue value)
{
    ConfigValue tree{ConfigTable{}};
    setValue(tree, key, std::move(value));
    return tree;
}
}

TEST(LayeredStoreTest, HigherLayerOverridesLower)
{
    LayeredStore store;
    store.pushLayer(tableWith(ConfigKey{"a"}, ConfigValue{std::int64_t{1}}));
    store.pushLayer(tableWith(ConfigKey{"a"}, ConfigValue{std::int64_t{2}}));
    EXPECT_EQ(store.valueOr<std::int64_t>(ConfigKey{"a"}, 0), 2);
}

TEST(LayeredStoreTest, FallsThroughToLowerLayer)
{
    LayeredStore store;
    store.pushLayer(tableWith(ConfigKey{"base"}, ConfigValue{std::string{"x"}}));
    store.pushLayer(tableWith(ConfigKey{"top"}, ConfigValue{std::string{"y"}}));
    EXPECT_EQ(store.valueOr<std::string>(ConfigKey{"base"}, ""), "x");
    EXPECT_EQ(store.valueOr<std::string>(ConfigKey{"top"}, ""), "y");
}

TEST(LayeredStoreTest, ValueOrReturnsFallbackWhenAbsent)
{
    const LayeredStore store;
    EXPECT_EQ(store.valueOr<std::int64_t>(ConfigKey{"missing"}, -1), -1);
}

TEST(LayeredStoreTest, FlattenDeepMergesTables)
{
    LayeredStore store;

    ConfigValue base{ConfigTable{}};
    setValue(base, ConfigKey{"window.width"}, ConfigValue{std::int64_t{800}});
    setValue(base, ConfigKey{"window.height"}, ConfigValue{std::int64_t{600}});

    ConfigValue top{ConfigTable{}};
    setValue(top, ConfigKey{"window.height"}, ConfigValue{std::int64_t{700}});
    setValue(top, ConfigKey{"window.title"}, ConfigValue{std::string{"app"}});

    store.pushLayer(base);
    store.pushLayer(top);

    const ConfigValue merged = store.flatten();
    EXPECT_EQ(findValue(merged, ConfigKey{"window.width"})->valueOr<std::int64_t>(0), 800);
    EXPECT_EQ(findValue(merged, ConfigKey{"window.height"})->valueOr<std::int64_t>(0), 700);
    EXPECT_EQ(findValue(merged, ConfigKey{"window.title"})->valueOr<std::string>(""), "app");
}

TEST(LayeredStoreTest, SetLayerReplacesByIndex)
{
    LayeredStore store;
    store.pushLayer(tableWith(ConfigKey{"a"}, ConfigValue{std::int64_t{1}}));
    store.setLayer(0, tableWith(ConfigKey{"a"}, ConfigValue{std::int64_t{9}}));
    EXPECT_EQ(store.layerCount(), 1u);
    EXPECT_EQ(store.valueOr<std::int64_t>(ConfigKey{"a"}, 0), 9);
}
