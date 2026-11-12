#pragma once

#include "vigine/config/configvalue.h"
#include "vigine/config/vigineconfig_export.h"

#include <stdexcept>
#include <string>
#include <string_view>

namespace vigine::config
{
class VIGINECONFIG_API ConfigParseError : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

// Parse TOML text into a ConfigValue table tree. Throws ConfigParseError on
// malformed input. TOML dates/times become ISO-8601 strings.
[[nodiscard]] VIGINECONFIG_API ConfigValue parseToml(std::string_view text);

// Serialize a ConfigValue (which must hold a Table) back to TOML text. Throws
// std::invalid_argument when the root value is not a table.
[[nodiscard]] VIGINECONFIG_API std::string serializeToml(const ConfigValue &value);
} // namespace vigine::config
