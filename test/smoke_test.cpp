#include <gtest/gtest.h>

#include <toml++/toml.hpp>

#include "vigine/config/version.h"

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
