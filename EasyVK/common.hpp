#pragma once
#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>

using u8    = std::uint8_t;
using u16   = std::uint16_t;
using u32   = std::uint32_t;
using u64   = std::uint64_t;
using i8    = std::int8_t;
using i16   = std::int16_t;
using i32   = std::int32_t;
using i64   = std::int64_t;
using ccstr = const char*;

#include "VkBootstrap.h"
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>

#define EZVK_CONVERT_OP(name, value)                                           \
  operator Vk##name() {                                                        \
    return value;                                                              \
  }

#define EZVK_ADDRESS_OP(name, value)                                           \
  Vk##name* operator&() {                                                      \
    return &value;                                                             \
  }

std::optional<std::vector<u8>> readFromFile(std::string const& filename,
                                            ccstr              mode);

template <
    typename... Destroyable,
    typename = std::void_t<decltype(std::declval<Destroyable...>().destroy())>>
void destroy(Destroyable&&... destroyList) {
  (destroyList.destroy(), ...);
}

template <
    typename... Creatable,
    typename = std::void_t<decltype(std::declval<Creatable...>().create())>>
void create(Creatable&&... destroyList) {
  (destroyList.create(), ...);
}

template <typename FirstArgT, typename... Destroyable>
void destroy1(FirstArgT&& firstArg, Destroyable&&... destroyList) {
  (destroyList.destroy(std::forward<FirstArgT>(firstArg)), ...);
}
template <typename FirstArgT, typename... Creatable>
void create1(FirstArgT&& firstArg, Creatable&&... destroyList) {
  (destroyList.create(std::forward<FirstArgT>(firstArg)), ...);
}

struct VkResultChecker {
  VkResultChecker(VkResult result);
  VkResultChecker& operator=(VkResult result);
};
