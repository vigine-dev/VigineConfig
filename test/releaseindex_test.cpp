#include "vigine/config/releaseindex.h"
#include <vigine/unittest/unittest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

/* The release index reads back what it wrote, refuses what is not one, and versions order as
   numbers with the commit after '+' left out. */

namespace
{

using namespace vigine::config;

VIGINE_TEST(ReleaseIndex, TheIndexReadsBackWhatWasWritten)
{
    ReleaseIndex index;
    index.setProject("codemap")
        .setVersion("0.4.2")
        .setCommit("b485c665")
        .setPlatform(platformName())
        .setKind(kKindRelease)
        .setNeeds({
            "shared@0.3.0"
    })
        .setFiles({{"codemap.exe", "ab12", 1024, true}});
    std::string refusal;
    const auto read = parseIndex(indexText(index), refusal);
    VIGINE_ASSERT_TRUE(read.has_value()) << refusal;
    VIGINE_EXPECT_EQ(read->project(), "codemap");
    VIGINE_EXPECT_EQ(read->needs(), (std::vector<std::string>{"shared@0.3.0"}));
    VIGINE_ASSERT_EQ(read->files().size(), 1u);
    VIGINE_EXPECT_EQ(read->files()[0].size(), 1024u);
    VIGINE_EXPECT_TRUE(read->files()[0].signedFile());
}

VIGINE_TEST(ReleaseIndex, ATextThatIsNoIndexIsRefused)
{
    std::string refusal;
    VIGINE_EXPECT_FALSE(parseIndex("schema = \"vigine.release/9\"", refusal).has_value());
    VIGINE_EXPECT_FALSE(
        parseIndex("schema = \"vigine.release/0\"\nproject = \"x\"", refusal).has_value())
        << "no version, no platform";
}

VIGINE_TEST(ReleaseIndex, VersionsOrderAsNumbersAndTheCommitDoesNotOrder)
{
    VIGINE_EXPECT_LT(compareVersions("0.4.1", "0.4.2"), 0);
    VIGINE_EXPECT_GT(compareVersions("0.10.0", "0.9.9"), 0) << "numbers, not letters";
    VIGINE_EXPECT_EQ(compareVersions("0.4.2+b485c665", "0.4.2+2a67feef"), 0);
    VIGINE_EXPECT_EQ(compareVersions("1", "1.0.0"), 0) << "a missing part is 0";
    VIGINE_EXPECT_LT(compareVersions("1.0.0-rc1", "1.0.0"), 0)
        << "a pre-release before the version";
}

// A deploy's index is debug.toml, a release's release.toml; a folder's index is found by either.
VIGINE_TEST(ReleaseIndex, ADeployLiesWithDebugTomlAReleaseWithReleaseToml)
{
    VIGINE_EXPECT_STREQ(indexFileName(kKindDeploy), "debug.toml");
    VIGINE_EXPECT_STREQ(indexFileName(kKindRelease), "release.toml");
    const auto folder = std::filesystem::temp_directory_path() / "vigineconfig_index_test";
    std::filesystem::remove_all(folder);
    std::filesystem::create_directories(folder);
    ReleaseIndex index;
    index.setProject("codemap")
        .setVersion("0.4.2")
        .setPlatform(platformName())
        .setKind(kKindDeploy);
    std::ofstream{folder / "debug.toml"} << indexText(index);
    std::string refusal;
    const auto read = readIndexIn(folder, refusal);
    VIGINE_ASSERT_TRUE(read.has_value()) << refusal;
    VIGINE_EXPECT_EQ(read->kind(), kKindDeploy);
    std::filesystem::rename(folder / "debug.toml", folder / "release.toml");
    VIGINE_EXPECT_FALSE(readIndexIn(folder, refusal).has_value()) << "a deploy named as a release";
    std::filesystem::remove_all(folder);
}

VIGINE_TEST(ReleaseIndex, AReleaseGoesUnderItsPlatformProjectAndVersion)
{
    VIGINE_EXPECT_EQ(releaseFolder("D:/work/releases", "codemap", "0.4.2", "b485c665"),
                     std::filesystem::path{"D:/work/releases"} / platformName() / "codemap" /
                         "0.4.2+b485c665");
}

} // namespace
