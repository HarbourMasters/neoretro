#pragma once

#include "models/StageEntry.h"

#include <atomic>
#include <functional>
#include <map>
#include <string>

namespace OtrGenerator {
using ErrorFn = std::function<void(const std::string&)>;

bool Generate(const std::map<std::string, StageEntry>& entries, const std::string& outputFile, bool prependAlt,
              std::atomic<int>& filesProcessed, const ErrorFn& onError);
} // namespace OtrGenerator
