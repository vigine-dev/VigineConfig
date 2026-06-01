#include <gtest/gtest.h>

#include <cstdint>
#include <string_view>

#include <toml++/toml.hpp>

#include "vigineconfig/vigineconfig.h"

// The nested toml++ submodule resolves and parses.
TEST(VigineConfigSmoke, TomlPlusPlusParsesAValue)
{
    const toml::table table = toml::parse("[server]\nport = 8080\nname = \"vigine\"\n");
    EXPECT_EQ(table["server"]["port"].value_or(0), 8080);
    EXPECT_EQ(table["server"]["name"].value_or(std::string_view{}), "vigine");
}

TEST(VigineConfigSmoke, LibraryLinks)
{
    EXPECT_STRNE(vigine::config::version(), "");
}

// The umbrella header exposes the public API behind a single include.
TEST(VigineConfigSmoke, UmbrellaHeaderExposesApi)
{
    const vigine::config::ConfigValue value{std::int64_t{7}};
    EXPECT_EQ(value.valueOr<std::int64_t>(0), 7);

    const vigine::config::ConfigKey key{"a.b"};
    EXPECT_EQ(key.size(), 2u);
}
