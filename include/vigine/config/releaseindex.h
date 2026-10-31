#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace vigine::config
{

// One file of a release, named relative to the release's folder with '/'.
// ENCAP EXEMPT: pure value aggregate.
struct ReleaseFile
{
    std::string path;
    std::string blake3;
    std::uint64_t size{0};
    // A `.sig` lies beside it.
    bool signedFile{false};
};

/* What a release's folder holds, as `release.toml` says it: which project at which version and
   commit, built from which engine, for which platform, as a release or a deploy; the programs in
   it, what has to stand beside it, the hash of the kernel a change of which restarts the program,
   and every file with its hash. ENCAP EXEMPT: pure value aggregate. */
struct ReleaseIndex
{
    std::string project;
    std::string version;
    std::string commit;
    std::string engine;
    std::string platform;
    // "release" or "deploy".
    std::string kind;
    std::string built;
    std::vector<std::string> programs;
    std::vector<std::string> needs;
    std::string nanokernel;
    std::string signedBy;
    std::vector<ReleaseFile> files;
};

inline constexpr const char *kIndexSchema   = "vigine.release/0";
inline constexpr const char *kIndexFileName = "release.toml";
inline constexpr const char *kKindRelease   = "release";
inline constexpr const char *kKindDeploy    = "deploy";

// The index as TOML text, and the index a text says; nothing, with why, when it is not one.
[[nodiscard]] std::string indexText(const ReleaseIndex &index);
[[nodiscard]] std::optional<ReleaseIndex> parseIndex(const std::string &text, std::string &refusal);
[[nodiscard]] std::optional<ReleaseIndex> readIndex(const std::filesystem::path &file,
                                                    std::string &refusal);

// The platform this build is for: windows-x64, linux-x64, macos-arm64.
[[nodiscard]] const char *platformName();

// The folder a release of `project` at `version` from `commit` goes under `root`.
[[nodiscard]] std::filesystem::path releaseFolder(const std::filesystem::path &root,
                                                  std::string_view project,
                                                  std::string_view version,
                                                  std::string_view commit);

/* Which of two versions is the newer: major.minor.patch compared as numbers, a missing part as 0;
   one with a pre-release ("-rc1") before the same without; what follows '+' (the commit) does not
   order. Negative when `left` is older, 0 when the same, positive when newer. */
[[nodiscard]] int compareVersions(std::string_view left, std::string_view right);

} // namespace vigine::config
