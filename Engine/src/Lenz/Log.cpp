#include "Log.h"
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace lenz
{
    std::vector<PerformanceLogger::LogEntry> PerformanceLogger::s_buffer;
    std::mutex PerformanceLogger::s_bufferMutex;
    size_t PerformanceLogger::s_maxBufferSize = 1000;

    Logger& Logger::GetInstance()
    {
        static Logger instance;
        return instance;
    }

    void Logger::Init()
    {
        LogConfig defaultConfig;
        Init(defaultConfig);
    }

    void Logger::Init(const LogConfig& config)
    {
        GetInstance().InitializeInternal(config);
    }

    void Logger::InitModule(const std::string& moduleName, const LogConfig& config)
    {
        GetInstance().InitializeModuleInternal(moduleName, config);
    }

    void Logger::InitializeInternal(const LogConfig& config)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_initialized)
        {
            PLOG_WARNING << "Logger already initialized. Reinitializing...";
        }

        m_config = config;

        try
        {
            if (!std::filesystem::exists(config.logDirectory))
            {
                std::filesystem::create_directories(config.logDirectory);
            }

            plog::Severity severity = ConvertLogLevel(config.level);

            switch (config.output)
            {
            case LogOutput::CONSOLE_ONLY:
            {
                static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
                plog::init(severity, &consoleAppender);
                break;
            }
            case LogOutput::FILE_ONLY:
            {
                std::string logPath = GenerateLogFileName(config);
                static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(
                    logPath.c_str(), config.maxFileSize, config.maxFiles);
                plog::init(severity, &fileAppender);
                break;
            }
            case LogOutput::BOTH:
            {
                std::string logPath = GenerateLogFileName(config);
                static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(
                    logPath.c_str(), config.maxFileSize, config.maxFiles);
                static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;

                plog::init(severity, &fileAppender).addAppender(&consoleAppender);
                break;
            }
            case LogOutput::CUSTOM:
                PLOG_INFO << "Custom log output selected. User should handle initialization.";
                break;
            }

            m_initialized = true;
            PLOG_INFO << "Logger initialized successfully";
            PLOG_INFO << "Log level: " << static_cast<int>(config.level);
            PLOG_INFO << "Output mode: " << static_cast<int>(config.output);

            if (config.output != LogOutput::CONSOLE_ONLY)
            {
                PLOG_INFO << "Log directory: " << config.logDirectory;
                PLOG_INFO << "Max file size: " << config.maxFileSize << " bytes";
                PLOG_INFO << "Max files: " << config.maxFiles;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Failed to initialize logger: " << e.what() << std::endl;
            throw;
        }
    }

    void Logger::InitializeModuleInternal(const std::string& moduleName, const LogConfig& config)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        try
        {
            std::string moduleLogDir = config.logDirectory + "/" + moduleName;
            if (!std::filesystem::exists(moduleLogDir))
            {
                std::filesystem::create_directories(moduleLogDir);
            }

            LogConfig moduleConfig = config;
            moduleConfig.logFileName = moduleName + "_" + config.logFileName;

            std::string logPath = moduleLogDir + "/" + moduleConfig.logFileName + moduleConfig.logFileExtension;

            auto moduleLogger = std::make_unique<plog::RollingFileAppender<plog::TxtFormatter>>(
                logPath.c_str(), config.maxFileSize, config.maxFiles);

            m_moduleLoggers[moduleName] = std::move(moduleLogger);

            PLOG_INFO << "Module logger initialized for: " << moduleName;
            PLOG_INFO << "Module log path: " << logPath;
        }
        catch (const std::exception& e)
        {
            PLOG_ERROR << "Failed to initialize module logger for " << moduleName << ": " << e.what();
            throw;
        }
    }

    plog::IAppender* Logger::GetModuleLogger(const std::string& moduleName)
    {
        std::lock_guard<std::mutex> lock(GetInstance().m_mutex);
        auto it = GetInstance().m_moduleLoggers.find(moduleName);
        if (it != GetInstance().m_moduleLoggers.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    void Logger::SetLogLevel(LogLevel level)
    {
        plog::get()->setMaxSeverity(GetInstance().ConvertLogLevel(level));
        GetInstance().m_config.level = level;
        PLOG_INFO << "Global log level changed to: " << static_cast<int>(level);
    }

    void Logger::SetLogLevel(const std::string& moduleName, LogLevel level)
    {
    }

    void Logger::Flush()
    {
        if (plog::get())
        {
            PLOG_DEBUG << "Flushing logs...";
        }
    }

    void Logger::Shutdown()
    {
        std::lock_guard<std::mutex> lock(GetInstance().m_mutex);

        PLOG_INFO << "Shutting down logger...";
        PLOG_INFO << "Total logs processed: " << GetInstance().m_logCount;

        GetInstance().m_moduleLoggers.clear();
        GetInstance().m_initialized = false;
    }

    std::string Logger::GenerateLogFileName(const LogConfig& config) const
    {
        std::string fileName = config.logFileName;

        if (config.useTimestamp)
        {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);

            std::ostringstream oss;
            oss << std::put_time(std::localtime(&time_t), "_%Y%m%d_%H%M%S");
            fileName += oss.str();
        }

        return config.logDirectory + "/" + fileName + config.logFileExtension;
    }

    plog::Severity Logger::ConvertLogLevel(LogLevel level) const
    {
        switch (level)
        {
        case LogLevel::DEBUG:   return plog::debug;
        case LogLevel::INFO:    return plog::info;
        case LogLevel::WARN:    return plog::warning;
        case LogLevel::ERROR:   return plog::error;
        case LogLevel::FATAL:   return plog::fatal;
        case LogLevel::NONE:    return plog::none;
        default:                return plog::info;
        }
    }

    Logger::~Logger()
    {
        if (m_initialized)
        {
            Shutdown();
        }
    }

    void PerformanceLogger::Init(size_t bufferSize)
    {
        std::lock_guard<std::mutex> lock(s_bufferMutex);
        s_maxBufferSize = bufferSize;
        s_buffer.reserve(bufferSize);
        PLOG_INFO << "Performance logger initialized with buffer size: " << bufferSize;
    }

    void PerformanceLogger::AddEntry(LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(s_bufferMutex);

        if (s_buffer.size() >= s_maxBufferSize)
        {
            LogBatch(); // Flush when buffer is full
        }

        s_buffer.push_back({
            level,
            message,
            std::chrono::high_resolution_clock::now()
            });
    }

    void PerformanceLogger::LogBatch()
    {
        for (const auto& entry : s_buffer)
        {
            auto time_since_epoch = entry.timestamp.time_since_epoch();
            auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(time_since_epoch);

            std::ostringstream oss;
            oss << "[PERF:" << microseconds.count() << "us] " << entry.message;

            switch (entry.level)
            {
            case LogLevel::DEBUG:   PLOG_DEBUG << oss.str(); break;
            case LogLevel::INFO:    PLOG_INFO << oss.str(); break;
            case LogLevel::WARN:    PLOG_WARNING << oss.str(); break;
            case LogLevel::ERROR:   PLOG_ERROR << oss.str(); break;
            case LogLevel::FATAL:   PLOG_FATAL << oss.str(); break;
            default: break;
            }
        }
        s_buffer.clear();
    }
}