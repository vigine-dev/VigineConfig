#pragma once

#include "vigine/config/vigineconfig_export.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace vigine::config
{

// One file of a release, named relative to the release's folder with '/'.
class VIGINECONFIG_API ReleaseFile
{
  public:
    ReleaseFile() = default;
    ReleaseFile(std::string path, std::string blake3, std::uint64_t size, bool signedFile)
        : _path(std::move(path)), _blake3(std::move(blake3)), _size(size), _signedFile(signedFile)
    {
    }

    [[nodiscard]] const std::string &path() const noexcept { return _path; }
    [[nodiscard]] const std::string &blake3() const noexcept { return _blake3; }
    [[nodiscard]] std::uint64_t size() const noexcept { return _size; }
    // A `.sig` lies beside it.
    [[nodiscard]] bool signedFile() const noexcept { return _signedFile; }

  private:
    std::string _path;
    std::string _blake3;
    std::uint64_t _size{0};
    bool _signedFile{false};
};

/* What a release's folder holds, as `release.toml` says it: which project at which version and
   commit, built from which engine, for which platform, as a release or a deploy; the programs in
   it, what has to stand beside it, the hash of the kernel a change of which restarts the program,
   and every file with its hash. */
class VIGINECONFIG_API ReleaseIndex
{
  public:
    [[nodiscard]] const std::string &project() const noexcept { return _project; }
    [[nodiscard]] const std::string &version() const noexcept { return _version; }
    [[nodiscard]] const std::string &commit() const noexcept { return _commit; }
    [[nodiscard]] const std::string &engine() const noexcept { return _engine; }
    [[nodiscard]] const std::string &platform() const noexcept { return _platform; }
    // "release" or "deploy".
    [[nodiscard]] const std::string &kind() const noexcept { return _kind; }
    [[nodiscard]] const std::string &built() const noexcept { return _built; }
    [[nodiscard]] const std::vector<std::string> &programs() const noexcept { return _programs; }
    [[nodiscard]] const std::vector<std::string> &needs() const noexcept { return _needs; }
    [[nodiscard]] const std::string &nanokernel() const noexcept { return _nanokernel; }
    [[nodiscard]] const std::string &signedBy() const noexcept { return _signedBy; }
    [[nodiscard]] const std::vector<ReleaseFile> &files() const noexcept { return _files; }

    ReleaseIndex &setProject(std::string value)
    {
        _project = std::move(value);
        return *this;
    }
    ReleaseIndex &setVersion(std::string value)
    {
        _version = std::move(value);
        return *this;
    }
    ReleaseIndex &setCommit(std::string value)
    {
        _commit = std::move(value);
        return *this;
    }
    ReleaseIndex &setEngine(std::string value)
    {
        _engine = std::move(value);
        return *this;
    }
    ReleaseIndex &setPlatform(std::string value)
    {
        _platform = std::move(value);
        return *this;
    }
    ReleaseIndex &setKind(std::string value)
    {
        _kind = std::move(value);
        return *this;
    }
    ReleaseIndex &setBuilt(std::string value)
    {
        _built = std::move(value);
        return *this;
    }
    ReleaseIndex &setPrograms(std::vector<std::string> value)
    {
        _programs = std::move(value);
        return *this;
    }
    ReleaseIndex &setNeeds(std::vector<std::string> value)
    {
        _needs = std::move(value);
        return *this;
    }
    ReleaseIndex &setNanokernel(std::string value)
    {
        _nanokernel = std::move(value);
        return *this;
    }
    ReleaseIndex &setSignedBy(std::string value)
    {
        _signedBy = std::move(value);
        return *this;
    }
    ReleaseIndex &setFiles(std::vector<ReleaseFile> value)
    {
        _files = std::move(value);
        return *this;
    }

  private:
    std::string _project;
    std::string _version;
    std::string _commit;
    std::string _engine;
    std::string _platform;
    std::string _kind;
    std::string _built;
    std::vector<std::string> _programs;
    std::vector<std::string> _needs;
    std::string _nanokernel;
    std::string _signedBy;
    std::vector<ReleaseFile> _files;
};

inline constexpr const char *kIndexSchema = "vigine.release/0";
inline constexpr const char *kKindRelease = "release";
inline constexpr const char *kKindDeploy  = "deploy";

/* A release -- for the target network -- lies with release.toml; a debug deploy -- for the debug
   network, laid out the same way -- with debug.toml, the same form: the name says which it is. */
inline constexpr const char *kReleaseIndexFileName = "release.toml";
inline constexpr const char *kDebugIndexFileName   = "debug.toml";

// The index's file name for a kind: debug.toml for a deploy, release.toml for a release.
[[nodiscard]] VIGINECONFIG_API const char *indexFileName(std::string_view kind);

// The index in a release's or a deploy's folder, whichever of the two it holds.
[[nodiscard]] VIGINECONFIG_API std::optional<ReleaseIndex>
readIndexIn(const std::filesystem::path &folder, std::string &refusal);

// The index as TOML text, and the index a text says; nothing, with why, when it is not one.
[[nodiscard]] VIGINECONFIG_API std::string indexText(const ReleaseIndex &index);
[[nodiscard]] VIGINECONFIG_API std::optional<ReleaseIndex> parseIndex(const std::string &text,
                                                                      std::string &refusal);
[[nodiscard]] VIGINECONFIG_API std::optional<ReleaseIndex>
readIndex(const std::filesystem::path &file, std::string &refusal);

// The platform this build is for: windows-x64, linux-x64, macos-arm64.
[[nodiscard]] VIGINECONFIG_API const char *platformName();

// The folder a release of `project` at `version` from `commit` goes under `root`.
[[nodiscard]] VIGINECONFIG_API std::filesystem::path
releaseFolder(const std::filesystem::path &root, std::string_view project, std::string_view version,
              std::string_view commit);

/* Which of two versions is the newer: major.minor.patch compared as numbers, a missing part as 0;
   one with a pre-release ("-rc1") before the same without; what follows '+' (the commit) does not
   order. Negative when `left` is older, 0 when the same, positive when newer. */
[[nodiscard]] VIGINECONFIG_API int compareVersions(std::string_view left, std::string_view right);

} // namespace vigine::config
