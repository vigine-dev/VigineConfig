#include "vigine/config/releaseindex.h"

#include <gtest/gtest.h>

/* The release index reads back what it wrote, refuses what is not one, and versions order as
   numbers with the commit after '+' left out. */

namespace
{

using namespace vigine::config;

TEST(ReleaseIndex, TheIndexReadsBackWhatWasWritten)
{
    ReleaseIndex index;
    index.project  = "codemap";
    index.version  = "0.4.2";
    index.commit   = "b485c665";
    index.platform = platformName();
    index.kind     = kKindRelease;
    index.needs    = {"shared@0.3.0"};
    index.files    = {
        {"codemap.exe", "ab12", 1024, true}
    };
    std::string refusal;
    const auto read = parseIndex(indexText(index), refusal);
    ASSERT_TRUE(read.has_value()) << refusal;
    EXPECT_EQ(read->project, "codemap");
    EXPECT_EQ(read->needs, (std::vector<std::string>{"shared@0.3.0"}));
    ASSERT_EQ(read->files.size(), 1u);
    EXPECT_EQ(read->files[0].size, 1024u);
    EXPECT_TRUE(read->files[0].signedFile);
}

TEST(ReleaseIndex, ATextThatIsNoIndexIsRefused)
{
    std::string refusal;
    EXPECT_FALSE(parseIndex("schema = \"vigine.release/9\"", refusal).has_value());
    EXPECT_FALSE(parseIndex("schema = \"vigine.release/0\"\nproject = \"x\"", refusal).has_value())
        << "no version, no platform";
}

TEST(ReleaseIndex, VersionsOrderAsNumbersAndTheCommitDoesNotOrder)
{
    EXPECT_LT(compareVersions("0.4.1", "0.4.2"), 0);
    EXPECT_GT(compareVersions("0.10.0", "0.9.9"), 0) << "numbers, not letters";
    EXPECT_EQ(compareVersions("0.4.2+b485c665", "0.4.2+2a67feef"), 0);
    EXPECT_EQ(compareVersions("1", "1.0.0"), 0) << "a missing part is 0";
    EXPECT_LT(compareVersions("1.0.0-rc1", "1.0.0"), 0) << "a pre-release before the version";
}

TEST(ReleaseIndex, AReleaseGoesUnderItsPlatformProjectAndVersion)
{
    EXPECT_EQ(releaseFolder("D:/work/releases", "codemap", "0.4.2", "b485c665"),
              std::filesystem::path{"D:/work/releases"} / platformName() / "codemap" /
                  "0.4.2+b485c665");
}

} // namespace
