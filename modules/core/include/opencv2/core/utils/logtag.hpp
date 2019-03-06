// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef OPENCV_CORE_LOGTAG_HPP
#define OPENCV_CORE_LOGTAG_HPP

#include "opencv2/core/cvstd.hpp"
#include "logger.defines.hpp"

//! @addtogroup core_logging
// This section describes OpenCV logging utilities.
//
//! @{

namespace cv {
namespace utils {
namespace logging {

//! @brief LogTag attaches additional information to each log message.
//
// The additional information serves two purposes. Firstly, it specifies the module and scope,
// which can be printed as part of the log message, or parsed by the logging backend without
// having to find it from the log message string. Secondly, each LogTag contains a LogLevel,
// which is used as the threshold for log level filtering.
//
// When a LogTag is registered with LogTagManager, the log filtering threshold can be
// programmatically configured by the LogTag's name. Such configuration can be specified
// from the environment, which is parsed by LogTagConfigParser.
//
// @sa CV_LOG_WITH_TAG macro
//
// For ease of auto-registration, use LogTagAuto.
//
struct CV_EXPORTS LogTag
{
    const char* name;
    LogLevel level;

    //! @brief Initializes a LogTag with name and initial log filtering threshold level.
    //
    // This constructor does not automatically register. To use automatic registration,
    // use LogTagAuto class instead.
    //
    constexpr LogTag(const char* _name, LogLevel _level)
        : name(_name)
        , level(_level)
    {}
};

}}}

//! @}

#endif
