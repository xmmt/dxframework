#pragma once

#pragma push_macro("ERROR")
#undef ERROR

enum class DebugWriteType {
    INFO,
    ERROR,
};

template <DebugWriteType Type>
class DebugWrite {
public:
    template <typename T>
    DebugWrite<Type>&& operator<<(T&& whatever) && {
        oss_ << whatever;
        return std::move(*this);
    }

    DebugWrite() = default;
    DebugWrite(DebugWrite&) = delete;
    DebugWrite(DebugWrite&& other)
        : oss_{ std::move(other.oss_) } {
    }
    DebugWrite& operator=(DebugWrite&) = delete;
    DebugWrite& operator=(DebugWrite&& other) {
        oss_ = std::move(other.oss_);
    }

    ~DebugWrite() {
        if constexpr (Type == DebugWriteType::ERROR) {
            std::cerr << oss_.str();
        } else {
            std::cout << oss_.str();
        }
        OutputDebugStringA(oss_.str().c_str());
    }

private:
    std::ostringstream oss_;

#undef Self
};

using WriteInfo = DebugWrite<DebugWriteType::INFO>;
using WriteError = DebugWrite<DebugWriteType::ERROR>;

#pragma pop_macro("ERROR")

#define ZCHECK(exp)                    \
    if (FAILED(exp)) {                 \
        WriteError()                   \
          << "Check failed at file: "  \
            + std::string(__FILE__)    \
            + " at line "              \
            + std::to_string(__LINE__) \
            + "\n";                    \
        ::exit(0);                     \
    }