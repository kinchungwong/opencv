// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef OPENCV_LOGGER_HPP
#define OPENCV_LOGGER_HPP

#include <iostream>
#include <sstream>
#include <limits.h> // INT_MAX

#include "logger.defines.hpp"

//! @addtogroup core_logging
// This section describes OpenCV logging utilities.
//
//! @{

namespace cv {
namespace utils {
namespace logging {

//! Supported logging levels and their semantic
enum LogLevel {
    LOG_LEVEL_SILENT = 0,              //!< for using in setLogVevel() call
    LOG_LEVEL_FATAL = 1,               //!< Fatal (critical) error (unrecoverable internal error)
    LOG_LEVEL_ERROR = 2,               //!< Error message
    LOG_LEVEL_WARNING = 3,             //!< Warning message
    LOG_LEVEL_INFO = 4,                //!< Info message
    LOG_LEVEL_DEBUG = 5,               //!< Debug message. Disabled in the "Release" build.
    LOG_LEVEL_VERBOSE = 6,             //!< Verbose (trace) messages. Requires verbosity level. Disabled in the "Release" build.
    LOG_LEVEL_VERBOSE_0 = LOG_LEVEL_VERBOSE,
    LOG_LEVEL_VERBOSE_1 = (LOG_LEVEL_VERBOSE + 1),
    LOG_LEVEL_VERBOSE_2 = (LOG_LEVEL_VERBOSE + 2),
    LOG_LEVEL_VERBOSE_3 = (LOG_LEVEL_VERBOSE + 3),
    LOG_LEVEL_VERBOSE_4 = (LOG_LEVEL_VERBOSE + 4),
    LOG_LEVEL_VERBOSE_5 = (LOG_LEVEL_VERBOSE + 5),
    LOG_LEVEL_VERBOSE_6 = (LOG_LEVEL_VERBOSE + 6),
    LOG_LEVEL_VERBOSE_7 = (LOG_LEVEL_VERBOSE + 7),
    LOG_LEVEL_VERBOSE_8 = (LOG_LEVEL_VERBOSE + 8),
    LOG_LEVEL_VERBOSE_9 = (LOG_LEVEL_VERBOSE + 9),
    LOG_LEVEL_VERBOSE_10 = (LOG_LEVEL_VERBOSE + 10),
#ifndef CV_DOXYGEN
    ENUM_LOG_LEVEL_FORCE_INT = INT_MAX
#endif
};

/** Set global logging level
@return previous logging level
*/
CV_EXPORTS LogLevel setLogLevel(LogLevel logLevel);

/** Get global logging level */
CV_EXPORTS LogLevel getLogLevel();

/**
Associates a mutable LogLevel variable (via pointer) with a name, so that the variable can be
read and written by the name, which in turn controls the name-based log level at runtime.
@see setLogLevelForTag
@see getLogLevelForTag
*/
CV_EXPORTS void registerLogLevelVariableForTag(const char* tag, cv::utils::logging::LogLevel& refLogLevel);

/**
Writes to the LogLevel variable by name. If the log level variable has not yet been registered,
it is temporarily stored with the log manager, so that it can be applied when the variable
is registered.
*/
CV_EXPORTS void setLogLevelForTag(const char* tag, cv::utils::logging::LogLevel logLevel);

/**
Reads the LogLevel variable by name. If the variable has not yet been registered, this function
returns the value remembered by the log manager via setLogLevelForTag.
*/
CV_EXPORTS LogLevel getLogLevelForTag(const char* tag);

namespace internal {
/** Write log message */
CV_EXPORTS void writeLogMessage(LogLevel logLevel, const char* message);

/**
Write log message with extra data, so that backend does not need to parse the string
to extract those information.
*/
CV_EXPORTS void writeLogMessageEx(LogLevel logLevel, const char* tag,
    const char* file, int line, const char* func, int time, int thread,
    const char* message);

} // namespace

/**
 * \def CV_LOG_STRIP_LEVEL
 *
 * Define CV_LOG_STRIP_LEVEL=CV_LOG_LEVEL_[DEBUG|INFO|WARN|ERROR|FATAL|DISABLED] to compile out anything at that and before that logging level
 */
#ifndef CV_LOG_STRIP_LEVEL
# if defined NDEBUG
#   define CV_LOG_STRIP_LEVEL CV_LOG_LEVEL_DEBUG
# else
#   define CV_LOG_STRIP_LEVEL CV_LOG_LEVEL_VERBOSE
# endif
#endif

#if 1 // ====== kinchungwong ====== NOCOMMIT ====== test run only ======
#undef CV_LOG_STRIP_LEVEL
#define CV_LOG_STRIP_LEVEL CV_LOG_LEVEL_VERBOSE_10
#endif  // ====== kinchungwong ====== NOCOMMIT ====== test run only ======

#if 1 // ====== kinchungwong ====== logging_20190125 ======

#define CV_EXPAND_AS_STRING(ARG_content) #ARG_content
#define CV_CONCAT(ARG_0, ARG_1) ARG_0##ARG_1
#define CV_CONCAT_LINENUM(ARG_ident) CV_CONCAT(ARG_ident, __LINE__)

// TODO
// Time function not implemented yet

#define CV_NEWLOG_GETTIME() ((int)0)

// TODO
// Thread ID function not implemented yet

#define CV_NEWLOG_GETTHREAD() ((int)0)

#define CV_NEWLOG_CHECK_LEVEL(ARG_level) \
    if (ARG_level >= CV_LOG_STRIP_LEVEL) { break; }

// CV_NEWLOG_CHECK_TAG maps to either ...
// - CV_NEWLOG_CHECK_TAG_STATIC
// - CV_NEWLOG_CHECK_TAG_DYNAMIC
//
// Static checking does not require function call. To use it, a module
// should define a static integer with that name as the log level variable.
// To enable the manager to propagate configuration into that variable,
// the static log level variable must be registered with the manager at startup.
//
// Dynamic checking requires a function call into the manager.
// No static variables or registration call necessary. Configuration
// is done via manager, based on string matching of module names.
//
// IMPORTANT SUGGESTION
// Each module can decide how to remap the CV_NEWLOG_CHECK_TAG macro based on
// whether it has implemented the static log level variable.

#define CV_NEWLOG_CHECK_TAG_STATIC(ARG_level, ARG_tag) \
    if (ARG_level >= cv_static_loglevel_tag_##ARG_tag) { break; }

#define CV_NEWLOG_CHECK_TAG_DYNAMIC(ARG_level, ARG_tag) \
    if (ARG_level >= cv::utils::logging::getLogLevelForTag(CV_EXPAND_AS_STRING(ARG_tag))) { break; }

#define CV_NEWLOG_CHECK_TAG(ARG_level, ARG_tag) \
    CV_NEWLOG_CHECK_TAG_DYNAMIC(ARG_level, ARG_tag)

// TODO
// CV_NEWLOG_CHECK_LOC not implemented yet.

#define CV_NEWLOG_CHECK_LOC(ARG_level, ARG_tag, ARG_file, ARG_line, ARG_func) \
    if (false) { break; }

#define CV_NEWLOG(ARG_tag, ARG_level, ...) \
    for(;;){ \
        CV_NEWLOG_CHECK_LEVEL(ARG_level); \
        CV_NEWLOG_CHECK_TAG(ARG_level, ARG_tag); \
        CV_NEWLOG_CHECK_LOC(ARG_level, ARG_tag, __FILE__, __LINE__, CV_Func); \
        std::ostringstream CV_CONCAT_LINENUM(cv_innertemp_strm_); \
        CV_CONCAT_LINENUM(cv_innertemp_strm_) << __VA_ARGS__; \
        cv::utils::logging::internal::writeLogMessageEx(\
            ARG_level, \
            CV_EXPAND_AS_STRING(ARG_tag), \
            __FILE__, \
            __LINE__, \
            CV_Func, \
            CV_NEWLOG_GETTIME(), \
            CV_NEWLOG_GETTHREAD(), \
            CV_CONCAT_LINENUM(cv_innertemp_strm_).str().c_str() \
        ); \
        break;\
    }

#define CV_LOG_FATAL(ARG_tag, ...) CV_NEWLOG(ARG_tag, cv::utils::logging::LOG_LEVEL_FATAL, __VA_ARGS__)
#define CV_LOG_ERROR(ARG_tag, ...) CV_NEWLOG(ARG_tag, cv::utils::logging::LOG_LEVEL_ERROR, __VA_ARGS__)
#define CV_LOG_WARNING(ARG_tag, ...) CV_NEWLOG(ARG_tag, cv::utils::logging::LOG_LEVEL_WARNING, __VA_ARGS__)

#if CV_LOG_STRIP_LEVEL <= CV_LOG_LEVEL_INFO
#define CV_LOG_INFO(ARG_tag, ...)
#else
#define CV_LOG_INFO(ARG_tag, ...) CV_NEWLOG(ARG_tag, cv::utils::logging::LOG_LEVEL_INFO, __VA_ARGS__)
#endif

#if CV_LOG_STRIP_LEVEL <= CV_LOG_LEVEL_DEBUG
#define CV_LOG_DEBUG(ARG_tag, ...)
#else
#define CV_LOG_DEBUG(ARG_tag, ...) CV_NEWLOG(ARG_tag, cv::utils::logging::LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif

#if CV_LOG_STRIP_LEVEL <= CV_LOG_LEVEL_VERBOSE
#define CV_LOG_VERBOSE(ARG_tag, ARG_verbose, ...)
#else
#define CV_LOG_VERBOSE(ARG_tag, ARG_verbose, ...) CV_NEWLOG(ARG_tag, cv::utils::logging::LOG_LEVEL_VERBOSE_##ARG_verbose, __VA_ARGS__)
#endif

#endif // ====== kinchungwong ====== logging_20190125 ======

#if 0
#define CV_LOG_FATAL(tag, ...)   for(;;) { if (cv::utils::logging::getLogLevel() < cv::utils::logging::LOG_LEVEL_FATAL) break; std::stringstream ss; ss << __VA_ARGS__; cv::utils::logging::internal::writeLogMessage(cv::utils::logging::LOG_LEVEL_FATAL, ss.str().c_str()); break; }
#define CV_LOG_ERROR(tag, ...)   for(;;) { if (cv::utils::logging::getLogLevel() < cv::utils::logging::LOG_LEVEL_ERROR) break; std::stringstream ss; ss << __VA_ARGS__; cv::utils::logging::internal::writeLogMessage(cv::utils::logging::LOG_LEVEL_ERROR, ss.str().c_str()); break; }
#define CV_LOG_WARNING(tag, ...) for(;;) { if (cv::utils::logging::getLogLevel() < cv::utils::logging::LOG_LEVEL_WARNING) break; std::stringstream ss; ss << __VA_ARGS__; cv::utils::logging::internal::writeLogMessage(cv::utils::logging::LOG_LEVEL_WARNING, ss.str().c_str()); break; }
#if CV_LOG_STRIP_LEVEL <= CV_LOG_LEVEL_INFO
#define CV_LOG_INFO(tag, ...)
#else
#define CV_LOG_INFO(tag, ...)    for(;;) { if (cv::utils::logging::getLogLevel() < cv::utils::logging::LOG_LEVEL_INFO) break; std::stringstream ss; ss << __VA_ARGS__; cv::utils::logging::internal::writeLogMessage(cv::utils::logging::LOG_LEVEL_INFO, ss.str().c_str()); break; }
#endif
#if CV_LOG_STRIP_LEVEL <= CV_LOG_LEVEL_DEBUG
#define CV_LOG_DEBUG(tag, ...)
#else
#define CV_LOG_DEBUG(tag, ...)   for(;;) { if (cv::utils::logging::getLogLevel() < cv::utils::logging::LOG_LEVEL_DEBUG) break; std::stringstream ss; ss << __VA_ARGS__; cv::utils::logging::internal::writeLogMessage(cv::utils::logging::LOG_LEVEL_DEBUG, ss.str().c_str()); break; }
#endif
#if CV_LOG_STRIP_LEVEL <= CV_LOG_LEVEL_VERBOSE
#define CV_LOG_VERBOSE(tag, v, ...)
#else
#define CV_LOG_VERBOSE(tag, v, ...) for(;;) { if (cv::utils::logging::getLogLevel() < cv::utils::logging::LOG_LEVEL_VERBOSE) break; std::stringstream ss; ss << "[VERB" << v << ":" << cv::utils::getThreadID() << "] " << __VA_ARGS__; cv::utils::logging::internal::writeLogMessage(cv::utils::logging::LOG_LEVEL_VERBOSE, ss.str().c_str()); break; }
#endif
#endif

}}} // namespace

//! @}

#endif // OPENCV_LOGGER_HPP
