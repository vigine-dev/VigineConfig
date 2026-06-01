#pragma once

#include <concepts>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace vigine::config
{
class ConfigValue;

using ConfigArray = std::vector<ConfigValue>;
using ConfigTable = std::map<std::string, ConfigValue, std::less<>>;

enum class ConfigType
{
    None,
    Boolean,
    Integer,
    Floating,
    String,
    Array,
    Table
};

// Tagged union of the value kinds a configuration tree can hold. The enum order
// mirrors the variant alternative order, so type() is a plain index cast.
class ConfigValue
{
public:
    ConfigValue() noexcept = default;

    ConfigValue(bool value) : _storage(value) {}

    template <std::integral IntegerType>
        requires(!std::same_as<IntegerType, bool>)
    ConfigValue(IntegerType value) : _storage(static_cast<std::int64_t>(value))
    {
    }

    template <std::floating_point FloatingType>
    ConfigValue(FloatingType value) : _storage(static_cast<double>(value))
    {
    }

    ConfigValue(std::string value) : _storage(std::move(value)) {}
    ConfigValue(const char *value) : _storage(std::string(value)) {}
    ConfigValue(ConfigArray value) : _storage(std::move(value)) {}
    ConfigValue(ConfigTable value) : _storage(std::move(value)) {}

    [[nodiscard]] ConfigType type() const noexcept
    {
        return static_cast<ConfigType>(_storage.index());
    }

    [[nodiscard]] bool isNone() const noexcept
    {
        return std::holds_alternative<std::monostate>(_storage);
    }

    template <typename ValueType>
    [[nodiscard]] bool is() const noexcept
    {
        return std::holds_alternative<ValueType>(_storage);
    }

    template <typename ValueType>
    [[nodiscard]] const ValueType *getIf() const noexcept
    {
        return std::get_if<ValueType>(&_storage);
    }

    template <typename ValueType>
    [[nodiscard]] ValueType valueOr(ValueType fallback) const
    {
        if (const ValueType *held = std::get_if<ValueType>(&_storage))
        {
            return *held;
        }
        return fallback;
    }

    bool operator==(const ConfigValue &other) const = default;

private:
    std::variant<std::monostate, bool, std::int64_t, double, std::string, ConfigArray, ConfigTable> _storage;
};
}
