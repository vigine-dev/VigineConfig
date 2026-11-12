#include <vigine/unittest/unittest.h>

#include "vigineconfig/vigineconfig.h"

#include <cstdint>
#include <string>
#include <string_view>

// TOML text reads through VigineFormat (the only holder of toml++) into a value tree.
VIGINE_TEST(VigineConfigSmoke, TomlParsesAValue)
{
    const vigine::config::ConfigValue tree =
        vigine::config::parseToml("[server]\nport = 8080\nname = \"vigine\"\n");
    const vigine::config::ConfigValue *server =
        vigine::config::findValue(tree, vigine::config::ConfigKey{"server"});
    VIGINE_ASSERT_NE(server, nullptr);
    VIGINE_EXPECT_EQ(server->getIfTable()->find("port")->second.integerOr(0), 8080);
    VIGINE_EXPECT_EQ(server->getIfTable()->find("name")->second.stringOr(""), "vigine");
}

VIGINE_TEST(VigineConfigSmoke, LibraryLinks)
{
    VIGINE_EXPECT_NE(std::string_view{vigine::config::version()}, std::string_view{});
}

// The umbrella header exposes the public API behind a single include.
VIGINE_TEST(VigineConfigSmoke, UmbrellaHeaderExposesApi)
{
    const vigine::config::ConfigValue value{std::int64_t{7}};
    VIGINE_EXPECT_EQ(value.integerOr(0), 7);

    const vigine::config::ConfigKey key{"a.b"};
    VIGINE_EXPECT_EQ(key.size(), 2u);
}
