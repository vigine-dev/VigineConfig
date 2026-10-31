#include "vigine/config/releaseindex.h"

#include "vigine/config/tomlcodec.h"

#include <cstdlib>
#include <fstream>
#include <sstream>

namespace vigine::config
{
namespace
{

std::string tomlQuoted(std::string_view text)
{
    std::string out{"\""};
    for (const char letter : text)
    {
        if (letter == '"' || letter == '\\')
            out.push_back('\\');
        out.push_back(letter);
    }
    return out + "\"";
}

std::string tomlList(const std::vector<std::string> &words)
{
    std::string out{"["};
    for (std::size_t index = 0; index < words.size(); ++index)
        out += (index == 0 ? "" : ", ") + tomlQuoted(words[index]);
    return out + "]";
}

std::string textOf(const ConfigTable &table, const char *name)
{
    const auto found        = table.find(name);
    const std::string *text = found != table.end() ? found->second.getIfString() : nullptr;
    return text != nullptr ? *text : std::string{};
}

std::vector<std::string> wordsOf(const ConfigTable &table, const char *name)
{
    std::vector<std::string> words;
    const auto found = table.find(name);
    if (const auto *array = found != table.end() ? found->second.getIfArray() : nullptr)
        for (const ConfigValue &item : *array)
            if (const std::string *word = item.getIfString())
                words.push_back(*word);
    return words;
}

// A version's parts before '+': the numbers of major.minor.patch and the pre-release, if any.
struct VersionParts
{
    long numbers[3]{0, 0, 0};
    std::string preRelease;
};

VersionParts partsOf(std::string_view version)
{
    VersionParts parts;
    version = version.substr(0, version.find('+'));
    if (const std::size_t dash = version.find('-'); dash != std::string_view::npos)
    {
        parts.preRelease = std::string{version.substr(dash + 1)};
        version          = version.substr(0, dash);
    }
    for (int index = 0; index < 3 && !version.empty(); ++index)
    {
        const std::size_t dot = version.find('.');
        parts.numbers[index] =
            std::strtol(std::string{version.substr(0, dot)}.c_str(), nullptr, 10);
        version = dot == std::string_view::npos ? std::string_view{} : version.substr(dot + 1);
    }
    return parts;
}

} // namespace

std::string indexText(const ReleaseIndex &index)
{
    std::string text  = "schema = " + tomlQuoted(kIndexSchema) + "\n";
    text             += "project = " + tomlQuoted(index.project) + "\n";
    text             += "version = " + tomlQuoted(index.version) + "\n";
    text             += "commit = " + tomlQuoted(index.commit) + "\n";
    text             += "engine = " + tomlQuoted(index.engine) + "\n";
    text             += "platform = " + tomlQuoted(index.platform) + "\n";
    text             += "kind = " + tomlQuoted(index.kind) + "\n";
    text             += "built = " + tomlQuoted(index.built) + "\n";
    text             += "programs = " + tomlList(index.programs) + "\n";
    text             += "needs = " + tomlList(index.needs) + "\n";
    text             += "nanokernel = " + tomlQuoted(index.nanokernel) + "\n";
    text             += "signed_by = " + tomlQuoted(index.signedBy) + "\n";
    for (const ReleaseFile &file : index.files)
    {
        text += "\n[[file]]\npath = " + tomlQuoted(file.path) + "\n";
        text += "blake3 = " + tomlQuoted(file.blake3) + "\n";
        text += "size = " + std::to_string(file.size) + "\n";
        text += std::string{"signed = "} + (file.signedFile ? "true" : "false") + "\n";
    }
    return text;
}

std::optional<ReleaseIndex> parseIndex(const std::string &text, std::string &refusal)
{
    ConfigValue root;
    try
    {
        root = parseToml(text);
    } catch (const std::exception &error)
    {
        return refusal = std::string{"the index is not TOML: "} + error.what(), std::nullopt;
    }
    const ConfigTable *table = root.getIfTable();
    if (table == nullptr || textOf(*table, "schema") != kIndexSchema)
        return refusal = std::string{"the index's schema is not "} + kIndexSchema, std::nullopt;
    ReleaseIndex index;
    index.project    = textOf(*table, "project");
    index.version    = textOf(*table, "version");
    index.commit     = textOf(*table, "commit");
    index.engine     = textOf(*table, "engine");
    index.platform   = textOf(*table, "platform");
    index.kind       = textOf(*table, "kind");
    index.built      = textOf(*table, "built");
    index.programs   = wordsOf(*table, "programs");
    index.needs      = wordsOf(*table, "needs");
    index.nanokernel = textOf(*table, "nanokernel");
    index.signedBy   = textOf(*table, "signed_by");
    if (index.project.empty() || index.version.empty() || index.platform.empty())
        return refusal = "the index names no project, version or platform", std::nullopt;
    const auto files = table->find("file");
    if (const auto *array = files != table->end() ? files->second.getIfArray() : nullptr)
        for (const ConfigValue &item : *array)
        {
            const ConfigTable *entry = item.getIfTable();
            if (entry == nullptr)
                continue;
            ReleaseFile file;
            file.path       = textOf(*entry, "path");
            file.blake3     = textOf(*entry, "blake3");
            const auto size = entry->find("size");
            file.size =
                size != entry->end() ? static_cast<std::uint64_t>(size->second.integerOr(0)) : 0;
            const auto signedValue = entry->find("signed");
            file.signedFile = signedValue != entry->end() && signedValue->second.booleanOr(false);
            index.files.push_back(std::move(file));
        }
    return index;
}

std::optional<ReleaseIndex> readIndex(const std::filesystem::path &file, std::string &refusal)
{
    std::ifstream in{file, std::ios::binary};
    if (!in)
        return refusal = "the index cannot be read: " + file.generic_string(), std::nullopt;
    std::ostringstream text;
    text << in.rdbuf();
    return parseIndex(text.str(), refusal);
}

const char *indexFileName(std::string_view kind)
{
    return kind == kKindDeploy ? kDebugIndexFileName : kReleaseIndexFileName;
}

std::optional<ReleaseIndex> readIndexIn(const std::filesystem::path &folder, std::string &refusal)
{
    std::error_code failed;
    for (const char *name : {kReleaseIndexFileName, kDebugIndexFileName})
        if (std::filesystem::exists(folder / name, failed))
        {
            std::optional<ReleaseIndex> index = readIndex(folder / name, refusal);
            // The name and the kind say the same, or the folder is not to be trusted.
            if (index && indexFileName(index->kind) != std::string_view{name})
                return refusal = std::string{name} + " says kind " + index->kind, std::nullopt;
            return index;
        }
    return refusal = "no release.toml or debug.toml in " + folder.generic_string(), std::nullopt;
}

const char *platformName()
{
#if defined(_WIN32)
    return "windows-x64";
#elif defined(__APPLE__)
    return "macos-arm64";
#else
    return "linux-x64";
#endif
}

std::filesystem::path releaseFolder(const std::filesystem::path &root, std::string_view project,
                                    std::string_view version, std::string_view commit)
{
    return root / platformName() / std::string{project} /
           (std::string{version} + "+" + std::string{commit});
}

int compareVersions(std::string_view left, std::string_view right)
{
    const VersionParts older = partsOf(left);
    const VersionParts newer = partsOf(right);
    for (int index = 0; index < 3; ++index)
        if (older.numbers[index] != newer.numbers[index])
            return older.numbers[index] < newer.numbers[index] ? -1 : 1;
    if (older.preRelease == newer.preRelease)
        return 0;
    // A pre-release comes before the version itself.
    if (older.preRelease.empty() || newer.preRelease.empty())
        return older.preRelease.empty() ? 1 : -1;
    return older.preRelease < newer.preRelease ? -1 : 1;
}

} // namespace vigine::config
