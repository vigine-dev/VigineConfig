#include "vigine/config/configkey.h"
#include "vigine/config/configtree.h"
#include "vigine/config/configvalue.h"
#include "vigine/config/layeredstore.h"
#include <vigine/unittest/unittest.h>

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

using vigine::config::ConfigKey;
using vigine::config::ConfigTable;
using vigine::config::ConfigValue;
using vigine::config::createLayeredStore;
using vigine::config::findValue;
using vigine::config::ILayeredStore;
using vigine::config::setValue;

namespace
{
ConfigValue tableWith(const ConfigKey &key, ConfigValue value)
{
    ConfigValue tree{ConfigTable{}};
    setValue(tree, key, std::move(value));
    return tree;
}
} // namespace

VIGINE_TEST(LayeredStoreTest, HigherLayerOverridesLower)
{
    const std::unique_ptr<ILayeredStore> store = createLayeredStore();
    store->pushLayer(tableWith(ConfigKey{"a"}, ConfigValue{std::int64_t{1}}));
    store->pushLayer(tableWith(ConfigKey{"a"}, ConfigValue{std::int64_t{2}}));
    VIGINE_EXPECT_EQ(store->integerOr(ConfigKey{"a"}, 0), 2);
}

VIGINE_TEST(LayeredStoreTest, FallsThroughToLowerLayer)
{
    const std::unique_ptr<ILayeredStore> store = createLayeredStore();
    store->pushLayer(tableWith(ConfigKey{"base"}, ConfigValue{std::string{"x"}}));
    store->pushLayer(tableWith(ConfigKey{"top"}, ConfigValue{std::string{"y"}}));
    VIGINE_EXPECT_EQ(store->stringOr(ConfigKey{"base"}, ""), "x");
    VIGINE_EXPECT_EQ(store->stringOr(ConfigKey{"top"}, ""), "y");
}

VIGINE_TEST(LayeredStoreTest, ValueOrReturnsFallbackWhenAbsent)
{
    const std::unique_ptr<ILayeredStore> store = createLayeredStore();
    VIGINE_EXPECT_EQ(store->integerOr(ConfigKey{"missing"}, -1), -1);
}

VIGINE_TEST(LayeredStoreTest, EveryKindFallsBackWhenTheKeyHoldsAnother)
{
    const std::unique_ptr<ILayeredStore> store = createLayeredStore();
    store->pushLayer(tableWith(ConfigKey{"debug"}, ConfigValue{true}));
    VIGINE_EXPECT_TRUE(store->booleanOr(ConfigKey{"debug"}, false));
    VIGINE_EXPECT_EQ(store->floatingOr(ConfigKey{"debug"}, 0.5), 0.5);
    VIGINE_EXPECT_EQ(store->floatingOr(ConfigKey{"missing"}, 1.5), 1.5);
}

VIGINE_TEST(LayeredStoreTest, FlattenDeepMergesTables)
{
    const std::unique_ptr<ILayeredStore> store = createLayeredStore();

    ConfigValue base{ConfigTable{}};
    setValue(base, ConfigKey{"window.width"}, ConfigValue{std::int64_t{800}});
    setValue(base, ConfigKey{"window.height"}, ConfigValue{std::int64_t{600}});

    ConfigValue top{ConfigTable{}};
    setValue(top, ConfigKey{"window.height"}, ConfigValue{std::int64_t{700}});
    setValue(top, ConfigKey{"window.title"}, ConfigValue{std::string{"app"}});

    store->pushLayer(base);
    store->pushLayer(top);

    const ConfigValue merged = store->flatten();
    VIGINE_EXPECT_EQ(findValue(merged, ConfigKey{"window.width"})->integerOr(0), 800);
    VIGINE_EXPECT_EQ(findValue(merged, ConfigKey{"window.height"})->integerOr(0), 700);
    VIGINE_EXPECT_EQ(findValue(merged, ConfigKey{"window.title"})->stringOr(""), "app");
}

VIGINE_TEST(LayeredStoreTest, SetLayerReplacesByIndex)
{
    const std::unique_ptr<ILayeredStore> store = createLayeredStore();
    store->pushLayer(tableWith(ConfigKey{"a"}, ConfigValue{std::int64_t{1}}));
    store->setLayer(0, tableWith(ConfigKey{"a"}, ConfigValue{std::int64_t{9}}));
    VIGINE_EXPECT_EQ(store->layerCount(), 1u);
    VIGINE_EXPECT_EQ(store->integerOr(ConfigKey{"a"}, 0), 9);
}
