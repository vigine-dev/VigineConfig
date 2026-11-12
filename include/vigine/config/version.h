#pragma once

#include "vigine/config/vigineconfig_export.h"

namespace vigine::config
{
// Library version string ("major.minor.patch").
[[nodiscard]] VIGINECONFIG_API const char *version() noexcept;
} // namespace vigine::config
