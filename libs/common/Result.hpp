#pragma once

#include "Error.hpp"

#include <expected>

namespace common {

template <typename T>
using Result = std::expected<T, Error>;

}  // namespace common
