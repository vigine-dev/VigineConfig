#include <gtest/gtest.h>

#include <stdexcept>

#include "vigine/config/configkey.h"

using vigine::config::ConfigKey;

TEST(ConfigKeyTest, ParsesDottedPath)
{
    const ConfigKey key{"window.size.width"};
    EXPECT_EQ(key.size(), 3u);
    EXPECT_EQ(key.toString(), "window.size.width");
    EXPECT_EQ(key.leaf(), "width");
}

TEST(ConfigKeyTest, DefaultAndEmptyStringAreRoot)
{
    EXPECT_TRUE(ConfigKey{}.empty());
    EXPECT_TRUE(ConfigKey{""}.empty());
}

TEST(ConfigKeyTest, ParentDropsTrailingSegment)
{
    const ConfigKey key{"a.b.c"};
    EXPECT_EQ(key.parent().toString(), "a.b");
    EXPECT_EQ(key.parent().parent().toString(), "a");
    EXPECT_TRUE(key.parent().parent().parent().empty());
}

TEST(ConfigKeyTest, AppendExtendsPath)
{
    ConfigKey key{"a"};
    key.append("b").append("c");
    EXPECT_EQ(key.toString(), "a.b.c");
}

TEST(ConfigKeyTest, RejectsEmptySegment)
{
    EXPECT_THROW((ConfigKey{"a..b"}), std::invalid_argument);
    EXPECT_THROW((ConfigKey{"a."}), std::invalid_argument);
    EXPECT_THROW((ConfigKey{".a"}), std::invalid_argument);
}
