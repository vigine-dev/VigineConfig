#include "vigine/config/tomlcodec.h"

#include <sstream>
#include <string>
#include <utility>

#include <toml++/toml.hpp>

namespace vigine::config
{
namespace
{
ConfigValue fromNode(const toml::node &node)
{
    switch (node.type())
    {
    case toml::node_type::table:
    {
        ConfigTable result;
        for (auto &&[key, value] : *node.as_table())
        {
            result.emplace(std::string(key.str()), fromNode(value));
        }
        return ConfigValue{std::move(result)};
    }
    case toml::node_type::array:
    {
        ConfigArray result;
        for (auto &&element : *node.as_array())
        {
            result.push_back(fromNode(element));
        }
        return ConfigValue{std::move(result)};
    }
    case toml::node_type::string:
        return ConfigValue{node.as_string()->get()};
    case toml::node_type::integer:
        return ConfigValue{static_cast<std::int64_t>(node.as_integer()->get())};
    case toml::node_type::floating_point:
        return ConfigValue{node.as_floating_point()->get()};
    case toml::node_type::boolean:
        return ConfigValue{node.as_boolean()->get()};
    case toml::node_type::date:
    {
        std::ostringstream stream;
        stream << node.as_date()->get();
        return ConfigValue{stream.str()};
    }
    case toml::node_type::time:
    {
        std::ostringstream stream;
        stream << node.as_time()->get();
        return ConfigValue{stream.str()};
    }
    case toml::node_type::date_time:
    {
        std::ostringstream stream;
        stream << node.as_date_time()->get();
        return ConfigValue{stream.str()};
    }
    case toml::node_type::none:
    default:
        return ConfigValue{};
    }
}

toml::table buildTable(const ConfigTable &source);
toml::array buildArray(const ConfigArray &source);

void insertValue(toml::table &table, std::string_view key, const ConfigValue &value)
{
    switch (value.type())
    {
    case ConfigType::Boolean:
        table.insert(key, *value.getIf<bool>());
        break;
    case ConfigType::Integer:
        table.insert(key, *value.getIf<std::int64_t>());
        break;
    case ConfigType::Floating:
        table.insert(key, *value.getIf<double>());
        break;
    case ConfigType::String:
        table.insert(key, *value.getIf<std::string>());
        break;
    case ConfigType::Array:
        table.insert(key, buildArray(*value.getIf<ConfigArray>()));
        break;
    case ConfigType::Table:
        table.insert(key, buildTable(*value.getIf<ConfigTable>()));
        break;
    case ConfigType::None:
        break;
    }
}

void pushValue(toml::array &array, const ConfigValue &value)
{
    switch (value.type())
    {
    case ConfigType::Boolean:
        array.push_back(*value.getIf<bool>());
        break;
    case ConfigType::Integer:
        array.push_back(*value.getIf<std::int64_t>());
        break;
    case ConfigType::Floating:
        array.push_back(*value.getIf<double>());
        break;
    case ConfigType::String:
        array.push_back(*value.getIf<std::string>());
        break;
    case ConfigType::Array:
        array.push_back(buildArray(*value.getIf<ConfigArray>()));
        break;
    case ConfigType::Table:
        array.push_back(buildTable(*value.getIf<ConfigTable>()));
        break;
    case ConfigType::None:
        break;
    }
}

toml::table buildTable(const ConfigTable &source)
{
    toml::table table;
    for (const auto &[key, value] : source)
    {
        insertValue(table, key, value);
    }
    return table;
}

toml::array buildArray(const ConfigArray &source)
{
    toml::array array;
    for (const auto &value : source)
    {
        pushValue(array, value);
    }
    return array;
}
}

ConfigValue parseToml(std::string_view text)
{
    try
    {
        const toml::table table = toml::parse(text);
        return fromNode(table);
    }
    catch (const toml::parse_error &error)
    {
        std::ostringstream stream;
        stream << "TOML parse error: " << error.description() << " (line " << error.source().begin.line
               << ", column " << error.source().begin.column << ")";
        throw ConfigParseError(stream.str());
    }
}

std::string serializeToml(const ConfigValue &value)
{
    const ConfigTable *root = value.getIf<ConfigTable>();
    if (root == nullptr)
    {
        throw std::invalid_argument("serializeToml: root value must be a table");
    }
    const toml::table table = buildTable(*root);
    std::ostringstream stream;
    stream << table;
    return stream.str();
}
}
