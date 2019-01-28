// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef OPENCV_LOGGER_DEFINES_HPP
#define OPENCV_LOGGER_DEFINES_HPP

//! @addtogroup core_logging
//! @{

// Supported logging levels and their semantic
#define CV_LOG_LEVEL_SILENT 0          //!< for using in setLogLevel() call
#define CV_LOG_LEVEL_FATAL 1           //!< Fatal (critical) error (unrecoverable internal error)
#define CV_LOG_LEVEL_ERROR 2           //!< Error message
#define CV_LOG_LEVEL_WARN 3            //!< Warning message
#define CV_LOG_LEVEL_INFO 4            //!< Info message
#define CV_LOG_LEVEL_DEBUG 5           //!< Debug message. Disabled in the "Release" build.
#define CV_LOG_LEVEL_VERBOSE 6         //!< Verbose (trace) messages. Requires verbosity level. Disabled in the "Release" build.
#define CV_LOG_LEVEL_VERBOSE_0 6
#define CV_LOG_LEVEL_VERBOSE_1 7
#define CV_LOG_LEVEL_VERBOSE_2 8
#define CV_LOG_LEVEL_VERBOSE_3 9
#define CV_LOG_LEVEL_VERBOSE_4 10
#define CV_LOG_LEVEL_VERBOSE_5 11
#define CV_LOG_LEVEL_VERBOSE_6 12
#define CV_LOG_LEVEL_VERBOSE_7 13
#define CV_LOG_LEVEL_VERBOSE_8 14
#define CV_LOG_LEVEL_VERBOSE_9 15
#define CV_LOG_LEVEL_VERBOSE_10 16

//! @}

#endif // OPENCV_LOGGER_DEFINES_HPP
