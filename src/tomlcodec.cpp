#include "vigine/config/tomlcodec.h"

#include "vigine/format/tomlcodec.h"

#include <string>
#include <utility>

namespace vigine::config
{
namespace
{
using vigine::format::TomlKind;
using vigine::format::TomlValue;

ConfigValue fromToml(const TomlValue &value)
{
    switch (value.kind())
    {
    case TomlKind::Table: {
        ConfigTable result;
        for (const std::string &key : value.keys())
        {
            result.emplace(key, fromToml(value.at(key)));
        }
        return ConfigValue{std::move(result)};
    }
    case TomlKind::Array: {
        ConfigArray result;
        for (const TomlValue &element : value.elements())
        {
            result.push_back(fromToml(element));
        }
        return ConfigValue{std::move(result)};
    }
    case TomlKind::String:
        return ConfigValue{value.asString()};
    case TomlKind::Integer:
        return ConfigValue{value.asInteger()};
    case TomlKind::Floating:
        return ConfigValue{value.asFloating()};
    case TomlKind::Boolean:
        return ConfigValue{value.asBoolean()};
    }
    return ConfigValue{};
}

TomlValue toToml(const ConfigValue &value);

TomlValue tableToToml(const ConfigTable &source)
{
    TomlValue table = TomlValue::table();
    for (const auto &[key, entry] : source)
    {
        if (!entry.isNone())
        {
            table.set(key, toToml(entry));
        }
    }
    return table;
}

TomlValue arrayToToml(const ConfigArray &source)
{
    TomlValue array = TomlValue::array();
    for (const ConfigValue &entry : source)
    {
        if (!entry.isNone())
        {
            array.append(toToml(entry));
        }
    }
    return array;
}

TomlValue toToml(const ConfigValue &value)
{
    switch (value.type())
    {
    case ConfigType::Boolean:
        return TomlValue::boolean(*value.getIfBoolean());
    case ConfigType::Integer:
        return TomlValue::integer(*value.getIfInteger());
    case ConfigType::Floating:
        return TomlValue::floating(*value.getIfFloating());
    case ConfigType::String:
        return TomlValue::string(*value.getIfString());
    case ConfigType::Array:
        return arrayToToml(*value.getIfArray());
    case ConfigType::Table:
        return tableToToml(*value.getIfTable());
    case ConfigType::None:
        break;
    }
    throw std::logic_error("toToml: a none value has no TOML form");
}
} // namespace

ConfigValue parseToml(std::string_view text)
{
    try
    {
        return fromToml(vigine::format::createTomlCodec()->parse(text));
    } catch (const vigine::format::TomlParseError &error)
    {
        throw ConfigParseError(error.what());
    }
}

std::string serializeToml(const ConfigValue &value)
{
    const ConfigTable *root = value.getIfTable();
    if (root == nullptr)
    {
        throw std::invalid_argument("serializeToml: root value must be a table");
    }
    return vigine::format::createTomlCodec()->serialize(tableToToml(*root));
}
} // namespace vigine::config
