#include "vigine/config/configvalue.h"
#include "vigine/config/tomlcodec.h"
#include <vigine/unittest/unittest.h>

#include <cstdint>
#include <stdexcept>
#include <string>

using vigine::config::ConfigArray;
using vigine::config::ConfigParseError;
using vigine::config::ConfigTable;
using vigine::config::ConfigValue;
using vigine::config::parseToml;
using vigine::config::serializeToml;

VIGINE_TEST(TomlCodecTest, ParsesScalarsAndNesting)
{
    const ConfigValue root   = parseToml("title = \"app\"\n"
                                         "[window]\n"
                                         "width = 800\n"
                                         "ratio = 1.5\n"
                                         "visible = true\n"
                                         "tags = [\"a\", \"b\"]\n");

    const ConfigTable *table = root.getIfTable();
    VIGINE_ASSERT_NE(table, nullptr);
    VIGINE_EXPECT_EQ(table->at("title").stringOr(""), "app");

    const ConfigTable *window = table->at("window").getIfTable();
    VIGINE_ASSERT_NE(window, nullptr);
    VIGINE_EXPECT_EQ(window->at("width").integerOr(0), 800);
    VIGINE_EXPECT_EQ(window->at("ratio").floatingOr(0.0), 1.5);
    VIGINE_EXPECT_TRUE(window->at("visible").booleanOr(false));

    const ConfigArray *tags = window->at("tags").getIfArray();
    VIGINE_ASSERT_NE(tags, nullptr);
    VIGINE_ASSERT_EQ(tags->size(), 2u);
    VIGINE_EXPECT_EQ((*tags)[0].stringOr(""), "a");
    VIGINE_EXPECT_EQ((*tags)[1].stringOr(""), "b");
}

VIGINE_TEST(TomlCodecTest, RoundTripsToEqualTree)
{
    const ConfigValue original = parseToml("a = 1\n"
                                           "b = \"x\"\n"
                                           "pi = 3.25\n"
                                           "[section]\n"
                                           "flag = true\n"
                                           "list = [1, 2, 3]\n");

    const std::string text     = serializeToml(original);
    const ConfigValue reparsed = parseToml(text);
    VIGINE_EXPECT_EQ(original, reparsed);
}

VIGINE_TEST(TomlCodecTest, ThrowsOnMalformedInput)
{
    VIGINE_EXPECT_THROW((void)parseToml("[unterminated\n"), ConfigParseError);
}

VIGINE_TEST(TomlCodecTest, SerializeRejectsNonTableRoot)
{
    VIGINE_EXPECT_THROW((void)serializeToml(ConfigValue{std::int64_t{42}}), std::invalid_argument);
}
