# VigineConfig

A small, dependency-light configuration core built on a vendored
[toml++](https://github.com/marzer/tomlplusplus) fork. It is a *functional
core*: no file I/O, no threads, no event system — only value types and pure
transformations that a host application wires into its own I/O and eventing.

## Components

- **ConfigValue** — a tagged-union value (bool, integer, floating, string,
  array, table).
- **ConfigKey** — a dotted path (`window.size.width`) into a value tree.
- **TOML codec** — `parseToml` / `serializeToml` convert TOML text to and from
  a `ConfigValue` tree.
- **Config tree** — `findValue` / `setValue` navigate and mutate a tree by key.
- **SchemaRegistry** — declare expected keys with a type and default; `validate`
  type-checks a tree and fills in defaults for absent keys.
- **LayeredStore** — stack configuration layers (defaults < file < overrides)
  with fall-through lookup and a deep-merge `flatten`.

## Build

```sh
git clone --recursive https://github.com/vigine-dev/VigineConfig.git
cmake -S VigineConfig -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build
```

Requires a C++23 compiler. toml++ and GoogleTest are vendored as submodules.
