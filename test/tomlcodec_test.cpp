#include "vigine/config/configvalue.h"
#include "vigine/config/tomlcodec.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

using vigine::config::ConfigArray;
using vigine::config::ConfigParseError;
using vigine::config::ConfigTable;
using vigine::config::ConfigValue;
using vigine::config::parseToml;
using vigine::config::serializeToml;

TEST(TomlCodecTest, ParsesScalarsAndNesting)
{
    const ConfigValue root   = parseToml("title = \"app\"\n"
                                         "[window]\n"
                                         "width = 800\n"
                                         "ratio = 1.5\n"
                                         "visible = true\n"
                                         "tags = [\"a\", \"b\"]\n");

    const ConfigTable *table = root.getIfTable();
    ASSERT_NE(table, nullptr);
    EXPECT_EQ(table->at("title").stringOr(""), "app");

    const ConfigTable *window = table->at("window").getIfTable();
    ASSERT_NE(window, nullptr);
    EXPECT_EQ(window->at("width").integerOr(0), 800);
    EXPECT_DOUBLE_EQ(window->at("ratio").floatingOr(0.0), 1.5);
    EXPECT_TRUE(window->at("visible").booleanOr(false));

    const ConfigArray *tags = window->at("tags").getIfArray();
    ASSERT_NE(tags, nullptr);
    ASSERT_EQ(tags->size(), 2u);
    EXPECT_EQ((*tags)[0].stringOr(""), "a");
    EXPECT_EQ((*tags)[1].stringOr(""), "b");
}

TEST(TomlCodecTest, RoundTripsToEqualTree)
{
    const ConfigValue original = parseToml("a = 1\n"
                                           "b = \"x\"\n"
                                           "pi = 3.25\n"
                                           "[section]\n"
                                           "flag = true\n"
                                           "list = [1, 2, 3]\n");

    const std::string text     = serializeToml(original);
    const ConfigValue reparsed = parseToml(text);
    EXPECT_EQ(original, reparsed);
}

TEST(TomlCodecTest, ThrowsOnMalformedInput)
{
    EXPECT_THROW((void)parseToml("[unterminated\n"), ConfigParseError);
}

TEST(TomlCodecTest, SerializeRejectsNonTableRoot)
{
    EXPECT_THROW((void)serializeToml(ConfigValue{std::int64_t{42}}), std::invalid_argument);
}
