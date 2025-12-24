#pragma once
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Formatters/CsvFormatter.h>
#include <plog/Formatters/FuncMessageFormatter.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <mutex>

namespace lenz
{
    enum class LogLevel
    {
        TRACE = plog::debug,
        DEBUG = plog::debug,
        INFO = plog::info,
        WARN = plog::warning,
        ERROR = plog::error,
        FATAL = plog::fatal,
        NONE = plog::none
    };

    enum class LogOutput
    {
        CONSOLE_ONLY,
        FILE_ONLY,
        BOTH,
        CUSTOM
    };

    struct LogConfig
    {
        LogLevel level = LogLevel::INFO;
        LogOutput output = LogOutput::BOTH;
        std::string logDirectory = "logs";
        std::string logFileName = "lenz";
        std::string logFileExtension = ".log";
        size_t maxFileSize = 10 * 1024 * 1024; // 10MB
        int maxFiles = 5;
        bool useTimestamp = true;
        bool useColors = true;
        bool enableAsyncLogging = false;
        std::string customFormat = "";
    };

    class Logger
    {
    public:
        static Logger& GetInstance();

        static void Init();

        static void Init(const LogConfig& config);

        static void InitModule(const std::string& moduleName, const LogConfig& config);

        static plog::IAppender* GetModuleLogger(const std::string& moduleName);

        static void SetLogLevel(LogLevel level);
        static void SetLogLevel(const std::string& moduleName, LogLevel level);

        static void Flush();

        static void Shutdown();

        static const LogConfig& GetConfig() { return GetInstance().m_config; }

        static size_t GetLogCount() { return GetInstance().m_logCount; }
        static void ResetLogCount() { GetInstance().m_logCount = 0; }

    private:
        Logger() = default;
        ~Logger();

        void InitializeInternal(const LogConfig& config);
        void InitializeModuleInternal(const std::string& moduleName, const LogConfig& config);
        std::string GenerateLogFileName(const LogConfig& config) const;
        plog::Severity ConvertLogLevel(LogLevel level) const;

        LogConfig m_config;
        std::unordered_map<std::string, std::unique_ptr<plog::RollingFileAppender<plog::TxtFormatter>>> m_moduleLoggers;
        std::mutex m_mutex;
        std::atomic<size_t> m_logCount{ 0 };
        bool m_initialized{ false };
    };

    class PerformanceLogger
    {
    public:
        static void Init(size_t bufferSize = 1000);
        static void LogBatch();
        static void AddEntry(LogLevel level, const std::string& message);

    private:
        struct LogEntry
        {
            LogLevel level;
            std::string message;
            std::chrono::high_resolution_clock::time_point timestamp;
        };

        static std::vector<LogEntry> s_buffer;
        static std::mutex s_bufferMutex;
        static size_t s_maxBufferSize;
    };
}

#define LZ_CORE_TRACE(...)    do { lenz::Logger::GetInstance(); PLOG_DEBUG << __VA_ARGS__; } while(0);
#define LZ_CORE_DEBUG(...)    do { lenz::Logger::GetInstance(); PLOG_DEBUG << __VA_ARGS__; } while(0);
#define LZ_CORE_INFO(...)     do { lenz::Logger::GetInstance(); PLOG_INFO << __VA_ARGS__; } while(0);
#define LZ_CORE_WARN(...)     do { lenz::Logger::GetInstance(); PLOG_WARNING << __VA_ARGS__; } while(0);
#define LZ_CORE_ERROR(...)    do { lenz::Logger::GetInstance(); PLOG_ERROR << __VA_ARGS__; } while(0);
#define LZ_CORE_FATAL(...)    do { lenz::Logger::GetInstance(); PLOG_FATAL << __VA_ARGS__; } while(0);

#define LZ_TRACE(...)         do { lenz::Logger::GetInstance(); PLOG_DEBUG << __VA_ARGS__; } while(0);
#define LZ_DEBUG(...)         do { lenz::Logger::GetInstance(); PLOG_DEBUG << __VA_ARGS__; } while(0);
#define LZ_INFO(...)          do { lenz::Logger::GetInstance(); PLOG_INFO << __VA_ARGS__; } while(0);
#define LZ_WARN(...)          do { lenz::Logger::GetInstance(); PLOG_WARNING << __VA_ARGS__; } while(0);
#define LZ_ERROR(...)         do { lenz::Logger::GetInstance(); PLOG_ERROR << __VA_ARGS__; } while(0);
#define LZ_FATAL(...)         do { lenz::Logger::GetInstance(); PLOG_FATAL << __VA_ARGS__; } while(0);

#define LZ_MODULE_LOG(module, level, ...) \
    do { \
        auto* logger = lenz::Logger::GetModuleLogger(module); \
        if (logger) { \
            PLOG(level) << __VA_ARGS__; \
        } \
    } while(0)

#define LZ_LOG_IF(condition, level, ...) \
    do { \
        if (condition) { \
            LZ_##level(__VA_ARGS__); \
        } \
    } while(0)

#define LZ_FUNCTION_ENTRY() LZ_TRACE("Entering function: " << __FUNCTION__)
#define LZ_FUNCTION_EXIT()  LZ_TRACE("Exiting function: " << __FUNCTION__)

#define LZ_SCOPE_TRACE(name) \
    struct ScopeTracer_##__LINE__ { \
        ScopeTracer_##__LINE__(const char* n) : name(n) { LZ_TRACE("Entering scope: " << name); } \
        ~ScopeTracer_##__LINE__() { LZ_TRACE("Exiting scope: " << name); } \
        const char* name; \
    } scopeTracer_##__LINE__(name)

#define LZ_PERF_LOG(level, ...) \
    do { \
        std::ostringstream oss; \
        oss << __VA_ARGS__; \
        lenz::PerformanceLogger::AddEntry(lenz::LogLevel::level, oss.str()); \
    } while(0)

#ifdef NDEBUG
#define LZ_ASSERT(condition, ...) ((void)0)
#define LZ_DEBUG_ONLY(...) ((void)0)
#else
#define LZ_ASSERT(condition, ...) \
        do { \
            if (!(condition)) { \
                LZ_FATAL("Assertion failed: " << #condition << " - " << __VA_ARGS__); \
                std::abort(); \
            } \
        } while(0)
#define LZ_DEBUG_ONLY(...) __VA_ARGS__
#endif

#define LZ_LOG_EVERY_N(n, level, ...) \
    do { \
        static int counter = 0; \
        if (++counter % n == 0) { \
            LZ_##level(__VA_ARGS__); \
        } \
    } while(0)

#define LZ_LOG_FIRST_N(n, level, ...) \
    do { \
        static int counter = 0; \
        if (++counter <= n) { \
            LZ_##level(__VA_ARGS__); \
        } \
    } while(0)