#include "vigine/config/configkey.h"
#include <vigine/unittest/unittest.h>

#include <stdexcept>

using vigine::config::ConfigKey;

VIGINE_TEST(ConfigKeyTest, ParsesDottedPath)
{
    const ConfigKey key{"window.size.width"};
    VIGINE_EXPECT_EQ(key.size(), 3u);
    VIGINE_EXPECT_EQ(key.toString(), "window.size.width");
    VIGINE_EXPECT_EQ(key.leaf(), "width");
}

VIGINE_TEST(ConfigKeyTest, DefaultAndEmptyStringAreRoot)
{
    VIGINE_EXPECT_TRUE(ConfigKey{}.empty());
    VIGINE_EXPECT_TRUE(ConfigKey{""}.empty());
}

VIGINE_TEST(ConfigKeyTest, ParentDropsTrailingSegment)
{
    const ConfigKey key{"a.b.c"};
    VIGINE_EXPECT_EQ(key.parent().toString(), "a.b");
    VIGINE_EXPECT_EQ(key.parent().parent().toString(), "a");
    VIGINE_EXPECT_TRUE(key.parent().parent().parent().empty());
}

VIGINE_TEST(ConfigKeyTest, AppendExtendsPath)
{
    ConfigKey key{"a"};
    key.append("b").append("c");
    VIGINE_EXPECT_EQ(key.toString(), "a.b.c");
}

VIGINE_TEST(ConfigKeyTest, RejectsEmptySegment)
{
    VIGINE_EXPECT_THROW((ConfigKey{"a..b"}), std::invalid_argument);
    VIGINE_EXPECT_THROW((ConfigKey{"a."}), std::invalid_argument);
    VIGINE_EXPECT_THROW((ConfigKey{".a"}), std::invalid_argument);
}
