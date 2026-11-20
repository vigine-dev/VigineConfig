#include "vigine/config/tomlcodec.h"

#include "vigine/format/datavalue.h"
#include "vigine/format/tomlcodec.h"

#include <string>
#include <utility>

namespace vigine::config
{
namespace
{
using vigine::format::DataKind;
using vigine::format::DataValue;

ConfigValue fromToml(const DataValue &value)
{
    switch (value.kind())
    {
    case DataKind::Table: {
        ConfigTable result;
        for (const std::string &key : value.keys())
        {
            result.emplace(key, fromToml(value.at(key)));
        }
        return ConfigValue{std::move(result)};
    }
    case DataKind::Array: {
        ConfigArray result;
        for (const DataValue &element : value.elements())
        {
            result.push_back(fromToml(element));
        }
        return ConfigValue{std::move(result)};
    }
    case DataKind::String:
        return ConfigValue{value.asString()};
    case DataKind::Integer:
        return ConfigValue{value.asInteger()};
    case DataKind::Float:
        return ConfigValue{value.asFloating()};
    case DataKind::Boolean:
        return ConfigValue{value.asBoolean()};
    case DataKind::Null:
        return ConfigValue{};
    }
    return ConfigValue{};
}

DataValue toToml(const ConfigValue &value);

DataValue tableToToml(const ConfigTable &source)
{
    DataValue table = DataValue::table();
    for (const auto &[key, entry] : source)
    {
        if (!entry.isNone())
        {
            table.set(key, toToml(entry));
        }
    }
    return table;
}

DataValue arrayToToml(const ConfigArray &source)
{
    DataValue array = DataValue::array();
    for (const ConfigValue &entry : source)
    {
        if (!entry.isNone())
        {
            array.append(toToml(entry));
        }
    }
    return array;
}

DataValue toToml(const ConfigValue &value)
{
    switch (value.type())
    {
    case ConfigType::Boolean:
        return DataValue::boolean(*value.getIfBoolean());
    case ConfigType::Integer:
        return DataValue::integer(*value.getIfInteger());
    case ConfigType::Floating:
        return DataValue::floating(*value.getIfFloating());
    case ConfigType::String:
        return DataValue::string(*value.getIfString());
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
