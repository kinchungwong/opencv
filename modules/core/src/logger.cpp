// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include "precomp.hpp"

#include <opencv2/core/utils/configuration.private.hpp>
#include <opencv2/core/utils/logger.hpp>

#include <sstream>
#include <iostream>
#include <fstream>
#include "utils/log_manager.hpp"

#ifdef __ANDROID__
# include <android/log.h>
#endif

namespace cv {
namespace utils {
namespace logging {

static LogLevel& getLogLevelVariable()
{
    return reinterpret_cast<LogLevel&>(LogManager::getInstance().getGlobalLogLevelVariable());
}

LogLevel setLogLevel(LogLevel logLevel)
{
    LogLevel old = getLogLevelVariable();
    getLogLevelVariable() = logLevel;
    return old;
}

LogLevel getLogLevel()
{
    return getLogLevelVariable();
}

void registerLogLevelVariableForTag(const char* tag, cv::utils::logging::LogLevel& refLogLevel)
{
    static_assert(sizeof(int) == sizeof(cv::utils::logging::LogLevel),
        "Requires LogLevel enum to have same underlying representation as int.");
    const std::string s{ tag };
    int* pLogLevel = reinterpret_cast<int*>(std::addressof(refLogLevel));
    LogManager::getInstance().registerLogLevel(s, *pLogLevel);
}

void setLogLevelForTag(const char* tag, cv::utils::logging::LogLevel logLevel)
{
    const std::string s{ tag };
    LogManager::getInstance().setLogLevel(s, (int)logLevel);
}

LogLevel getLogLevelForTag(const char* tag)
{
    const std::string s{ tag };
    return (cv::utils::logging::LogLevel)LogManager::getInstance().getLogLevel(s);
}

namespace internal {

void writeLogMessage(LogLevel logLevel, const char* message)
{
    const int threadID = cv::utils::getThreadID();
    std::ostringstream ss;
    switch (logLevel)
    {
    case LOG_LEVEL_FATAL:   ss << "[FATAL:" << threadID << "] " << message << std::endl; break;
    case LOG_LEVEL_ERROR:   ss << "[ERROR:" << threadID << "] " << message << std::endl; break;
    case LOG_LEVEL_WARNING: ss << "[ WARN:" << threadID << "] " << message << std::endl; break;
    case LOG_LEVEL_INFO:    ss << "[ INFO:" << threadID << "] " << message << std::endl; break;
    case LOG_LEVEL_DEBUG:   ss << "[DEBUG:" << threadID << "] " << message << std::endl; break;
    case LOG_LEVEL_VERBOSE: ss << message << std::endl; break;
    default:
        return;
    }
#ifdef __ANDROID__
    int android_logLevel = ANDROID_LOG_INFO;
    switch (logLevel)
    {
    case LOG_LEVEL_FATAL:   android_logLevel = ANDROID_LOG_FATAL; break;
    case LOG_LEVEL_ERROR:   android_logLevel = ANDROID_LOG_ERROR; break;
    case LOG_LEVEL_WARNING: android_logLevel = ANDROID_LOG_WARN; break;
    case LOG_LEVEL_INFO:    android_logLevel = ANDROID_LOG_INFO; break;
    case LOG_LEVEL_DEBUG:   android_logLevel = ANDROID_LOG_DEBUG; break;
    case LOG_LEVEL_VERBOSE: android_logLevel = ANDROID_LOG_VERBOSE; break;
    default:
        break;
    }
    __android_log_print(android_logLevel, "OpenCV/" CV_VERSION, "%s", ss.str().c_str());
#endif
    std::ostream* out = (logLevel <= LOG_LEVEL_WARNING) ? &std::cerr : &std::cout;
    (*out) << ss.str();
    if (logLevel <= LOG_LEVEL_WARNING)
        (*out) << std::flush;
}

void writeLogMessageEx(LogLevel logLevel, const char* /*tag*/,
    const char* /*file*/, int /*line*/, const char* /*func*/, int /*time*/, int /*thread*/,
    const char* message)
{
    // TODO
    // Implement formatting for the extra data.
    writeLogMessage(logLevel, message);
}

} // namespace

}}} // namespace
