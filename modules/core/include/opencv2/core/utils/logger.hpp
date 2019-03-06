// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef OPENCV_LOGGER_HPP
#define OPENCV_LOGGER_HPP

#include <iostream>
#include <sstream>
#include <limits.h> // INT_MAX

#include "logger.defines.hpp"
#include "logtag.hpp"

//! @addtogroup core_logging
// This section describes OpenCV logging utilities.
//
//! @{

namespace cv {
namespace utils {
namespace logging {

/** Set global logging level
@return previous logging level
*/
CV_EXPORTS LogLevel setLogLevel(LogLevel logLevel);

/** Get global logging level */
CV_EXPORTS LogLevel getLogLevel();

/**
@brief Registers a LogTag. This is done automatically when using LogTagAuto.
@details Internally, the LogTag is registered with a global singleton of LogTagManager.
*/
CV_EXPORTS void registerLogTag(LogTag* plogtag);

//! @brief Changes the log level of all LogTags having the exact name.
CV_EXPORTS void setLogTagLevel(const char* tag, LogLevel level);

//! @brief Returns the log level of the LogTag having the exact name.
CV_EXPORTS LogLevel getLogTagLevel(const char* tag);

namespace internal {

/** Get global log tag */
CV_EXPORTS LogTag* getGlobalLogTag();

/** Write log message */
CV_EXPORTS void writeLogMessage(LogLevel logLevel, const char* message);

/** Write log message */
CV_EXPORTS void writeLogMessageEx(LogLevel logLevel, const char* tag, const char* file, int line, const char* func, const char* message);

} // namespace

//! @brief LogTagAuto is a LogTag with automatic registration in the constructor.
struct CV_EXPORTS LogTagAuto
    : public LogTag
{
    //! @brief Initializes a LogTagAuto (derived from LogTag) and automatically
    // registers with the logging system.
    LogTagAuto(const char* _name, LogLevel _level)
        : LogTag(_name, _level)
    {
        registerLogTag(this);
    }
};

/**
 * \def CV_LOG_STRIP_LEVEL
 *
 * Define CV_LOG_STRIP_LEVEL=CV_LOG_LEVEL_[DEBUG|INFO|WARN|ERROR|FATAL|SILENT] to compile out anything at that and before that logging level
 */
#ifndef CV_LOG_STRIP_LEVEL
# if defined NDEBUG
#   define CV_LOG_STRIP_LEVEL CV_LOG_LEVEL_DEBUG
# else
#   define CV_LOG_STRIP_LEVEL CV_LOG_LEVEL_VERBOSE
# endif
#endif

/**
@def CV_LOGTAG_PTR_CAST
@brief Casts the given C++ expression into a const pointer to LogTag. Null pointer is allowed.
*/
#define CV_LOGTAG_PTR_CAST(expr) static_cast<const cv::utils::logging::LogTag*>(expr)

/**
@def CV_LOGTAG_EXPAND_NAME
@brief Allows preprocessor modification of the tag argument before using it as a C++ expression.

CV_LOGTAG_EXPAND_NAME is intended to be re-defined in other compilation units to allows logging
users to use a shorter name argument when calling CV_LOG_WITH_TAG or its related macros
such as CV_LOG_INFO.

This macro is intended to modify the tag argument as a string (token), via preprocessor token
pasting or metaprogramming techniques. A typical usage is to apply a prefix, such as
@code
#define CV_LOGTAG_EXPAND_NAME(tag) cv_logtag_##tag
@endcode

It is permitted to re-define to a hard-coded expression, ignoring the tag. This would work
identically like the CV_LOGTAG_FALLBACK macro.

Important: When the logging macro is called with tag being NULL, a user-defined
CV_LOGTAG_EXPAND_NAME may expand it into
@code
cv_logtag_0, cv_logtag_NULL, or cv_logtag_nullptr.
@endcode
Use with care. Also be mindful of C++ symbol redefinitions.

If there is significant amount of logging code with tag being NULL, it is recommended to use
(re-define) CV_LOGTAG_FALLBACK to inject locally a default tag at the beginning of a
compilation unit, to minimize lines of code changes.
*/
#define CV_LOGTAG_EXPAND_NAME(tag) tag

/**
@def CV_LOGTAG_FALLBACK
@brief Specifies fallback LogTag to use when the logging statement uses NULL for tag.

This macro needs to expand into a C++ expression that can be static_cast into
@code
(cv::utils::logging::LogTag*)
@endcode
. Null (nullptr) is permitted.
*/
#define CV_LOGTAG_FALLBACK nullptr

/**
@def CV_LOGTAG_GLOBAL
@brief Specifies global LogTag to use when neither the logging statement nor the
CV_LOGTAG_FALLBACK provides a tag.

This macro needs to expand into a C++ expression that can be static_cast into
@code
(cv::utils::logging::LogTag*)
@endcode
. Must be non-null. It is NOT recommended to redefine this macro in other compilation units.
*/
#define CV_LOGTAG_GLOBAL cv::utils::logging::internal::getGlobalLogTag()

/**
@def CV_LOG_WITH_TAG
@brief CV_LOG_WITH_TAG is the new logging macro that allows use of LogTag for adding module
and scope information, and to allow tag-based log level filtering threshold.

For convenience, logging macros with log level hard-coded into the name and the expression
are also available.

@sa CV_LOG_FATAL
@sa CV_LOG_ERROR
@sa CV_LOG_WARNING
@sa CV_LOG_INFO
@sa CV_LOG_DEBUG
@sa CV_LOG_VERBOSE
*/
#define CV_LOG_WITH_TAG(tag, msgLevel, ...) \
    for(;;) { \
        const auto cv_temp_msglevel = (cv::utils::logging::LogLevel)(msgLevel); \
        if (cv_temp_msglevel >= (CV_LOG_STRIP_LEVEL)) break; \
        auto cv_temp_logtagptr = CV_LOGTAG_PTR_CAST(CV_LOGTAG_EXPAND_NAME(tag)); \
        if (!cv_temp_logtagptr) cv_temp_logtagptr = CV_LOGTAG_PTR_CAST(CV_LOGTAG_FALLBACK); \
        if (!cv_temp_logtagptr) cv_temp_logtagptr = CV_LOGTAG_PTR_CAST(CV_LOGTAG_GLOBAL); \
        if (cv_temp_logtagptr && (cv_temp_msglevel > cv_temp_logtagptr->level)) break; \
        std::stringstream cv_temp_logstream; \
        cv_temp_logstream << __VA_ARGS__; \
        cv::utils::logging::internal::writeLogMessageEx( \
            cv_temp_msglevel, \
            (cv_temp_logtagptr ? cv_temp_logtagptr->name : nullptr), \
            __FILE__, \
            __LINE__, \
            CV_Func, \
            cv_temp_logstream.str().c_str()); \
        break; \
    }

#define CV_LOG_FATAL(tag, ...)   CV_LOG_WITH_TAG(tag, cv::utils::logging::LOG_LEVEL_FATAL, __VA_ARGS__)
#define CV_LOG_ERROR(tag, ...)   CV_LOG_WITH_TAG(tag, cv::utils::logging::LOG_LEVEL_ERROR, __VA_ARGS__)
#define CV_LOG_WARNING(tag, ...) CV_LOG_WITH_TAG(tag, cv::utils::logging::LOG_LEVEL_WARNING, __VA_ARGS__)
#define CV_LOG_INFO(tag, ...) CV_LOG_WITH_TAG(tag, cv::utils::logging::LOG_LEVEL_INFO, __VA_ARGS__)
#define CV_LOG_DEBUG(tag, ...) CV_LOG_WITH_TAG(tag, cv::utils::logging::LOG_LEVEL_DEBUG, __VA_ARGS__)
#define CV_LOG_VERBOSE(tag, v, ...) CV_LOG_WITH_TAG(tag, (cv::utils::logging::LOG_LEVEL_VERBOSE + (int)(v)), __VA_ARGS__)

#if CV_LOG_STRIP_LEVEL <= CV_LOG_LEVEL_INFO
# undef CV_LOG_INFO
// CV_LOG_INFO is disabled because of CV_LOG_STRIP_LEVEL.
# define CV_LOG_INFO(tag, ...)
#endif

#if CV_LOG_STRIP_LEVEL <= CV_LOG_LEVEL_DEBUG
# undef CV_LOG_DEBUG
// CV_LOG_DEBUG is disabled because of CV_LOG_STRIP_LEVEL.
# define CV_LOG_DEBUG(tag, ...)
#endif

#if CV_LOG_STRIP_LEVEL <= CV_LOG_LEVEL_VERBOSE
# undef CV_LOG_VERBOSE
// CV_LOG_VERBOSE is disabled because of CV_LOG_STRIP_LEVEL.
# define CV_LOG_VERBOSE(tag, v, ...)
#endif

}}} // namespace

//! @}

#endif // OPENCV_LOGGER_HPP
