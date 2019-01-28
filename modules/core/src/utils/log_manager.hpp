// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef OPENCV_CORE_LOG_MANAGER_HPP
#define OPENCV_CORE_LOG_MANAGER_HPP

#ifndef __OPENCV_BUILD
#  error this is a private header which should not be used from outside of the OpenCV library
#endif

#include "opencv2/core.hpp"
#include "cvconfig.h"
#include <unordered_map>

namespace cv {
namespace utils {
namespace logging {

class LogManager
{
private:
    using MutexType = std::mutex;
    using LockType = std::lock_guard<MutexType>;

private:
    mutable MutexType m_mutex;
    std::unordered_map<std::string, int> m_configuredLogLevels;
    std::unordered_map<std::string, int*> m_registeredLogLevels;
    int m_globalLogLevel;

private:
    LogManager();
    ~LogManager();

public:
    static LogManager& getInstance();

public:
    void registerLogLevel(const std::string& name, int& refLogLevel);
    void setLogLevel(const std::string& name, int logLevel);
    int getLogLevel(const std::string& name) const;
    int& getGlobalLogLevelVariable();

private:
    void internal_applyConfiguredLogLevel_onRegistering(const LockType& lock,
        const std::pair<const std::string, int*>& registeredPair);

private:
    static int defaultLogLevel();
    static int parseLogLevelName(const std::string& s);
    static int parseLogLevelChar(char c);
    void applyLogLevelConfigurationSingle(const std::string& strLogLevelSingle);
    void applyLogLevelConfigurationCompound(const std::string& strLogLevelCompound);
    void applyLogLevelConfigurationCompound();
};

}}} // namespace

#endif // OPENCV_CORE_LOG_MANAGER_HPP
