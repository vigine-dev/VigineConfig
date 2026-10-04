#pragma once

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

    ConfigValue(char value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(signed char value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(unsigned char value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(short value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(unsigned short value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(int value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(unsigned int value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(long value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(unsigned long value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(long long value) : _storage(static_cast<std::int64_t>(value)) {}
    ConfigValue(unsigned long long value) : _storage(static_cast<std::int64_t>(value)) {}

    ConfigValue(float value) : _storage(static_cast<double>(value)) {}
    ConfigValue(double value) : _storage(static_cast<double>(value)) {}
    ConfigValue(long double value) : _storage(static_cast<double>(value)) {}

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

    [[nodiscard]] bool isBoolean() const noexcept { return std::holds_alternative<bool>(_storage); }
    [[nodiscard]] bool isInteger() const noexcept
    {
        return std::holds_alternative<std::int64_t>(_storage);
    }
    [[nodiscard]] bool isFloating() const noexcept
    {
        return std::holds_alternative<double>(_storage);
    }
    [[nodiscard]] bool isString() const noexcept
    {
        return std::holds_alternative<std::string>(_storage);
    }
    [[nodiscard]] bool isArray() const noexcept
    {
        return std::holds_alternative<ConfigArray>(_storage);
    }
    [[nodiscard]] bool isTable() const noexcept
    {
        return std::holds_alternative<ConfigTable>(_storage);
    }

    [[nodiscard]] const bool *getIfBoolean() const noexcept { return std::get_if<bool>(&_storage); }
    [[nodiscard]] bool *getIfBoolean() noexcept { return std::get_if<bool>(&_storage); }
    [[nodiscard]] const std::int64_t *getIfInteger() const noexcept
    {
        return std::get_if<std::int64_t>(&_storage);
    }
    [[nodiscard]] std::int64_t *getIfInteger() noexcept
    {
        return std::get_if<std::int64_t>(&_storage);
    }
    [[nodiscard]] const double *getIfFloating() const noexcept
    {
        return std::get_if<double>(&_storage);
    }
    [[nodiscard]] double *getIfFloating() noexcept { return std::get_if<double>(&_storage); }
    [[nodiscard]] const std::string *getIfString() const noexcept
    {
        return std::get_if<std::string>(&_storage);
    }
    [[nodiscard]] std::string *getIfString() noexcept
    {
        return std::get_if<std::string>(&_storage);
    }
    [[nodiscard]] const ConfigArray *getIfArray() const noexcept
    {
        return std::get_if<ConfigArray>(&_storage);
    }
    [[nodiscard]] ConfigArray *getIfArray() noexcept { return std::get_if<ConfigArray>(&_storage); }
    [[nodiscard]] const ConfigTable *getIfTable() const noexcept
    {
        return std::get_if<ConfigTable>(&_storage);
    }
    [[nodiscard]] ConfigTable *getIfTable() noexcept { return std::get_if<ConfigTable>(&_storage); }

    // The held value of that kind, or the fallback when the value holds another
    // kind.
    [[nodiscard]] bool booleanOr(bool fallback) const noexcept
    {
        const bool *held = getIfBoolean();
        return held != nullptr ? *held : fallback;
    }
    [[nodiscard]] std::int64_t integerOr(std::int64_t fallback) const noexcept
    {
        const std::int64_t *held = getIfInteger();
        return held != nullptr ? *held : fallback;
    }
    [[nodiscard]] double floatingOr(double fallback) const noexcept
    {
        const double *held = getIfFloating();
        return held != nullptr ? *held : fallback;
    }
    [[nodiscard]] std::string stringOr(std::string fallback) const
    {
        const std::string *held = getIfString();
        return held != nullptr ? *held : std::move(fallback);
    }

    bool operator==(const ConfigValue &other) const = default;

  private:
    std::variant<std::monostate, bool, std::int64_t, double, std::string, ConfigArray, ConfigTable>
        _storage;
};
} // namespace vigine::config
