// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include "precomp.hpp"

#include "log_manager.hpp"
#include <opencv2/core/utils/configuration.private.hpp>
#include <opencv2/core/utils/logger.hpp>


namespace cv {
namespace utils {
namespace logging {

LogManager::LogManager()
    : m_mutex()
    , m_configuredLogLevels()
    , m_registeredLogLevels()
    , m_globalLogLevel(defaultLogLevel())
{
    applyLogLevelConfigurationCompound();
}

LogManager::~LogManager()
{
}

LogManager& LogManager::getInstance()
{
    static LogManager instance;
    return instance;
}

void LogManager::registerLogLevel(const std::string& name, int& refLogLevel)
{
    LockType lock(m_mutex);
    auto regIter = m_registeredLogLevels.find(name);
    if (regIter == m_registeredLogLevels.end())
    {
        regIter = m_registeredLogLevels.emplace(name, &refLogLevel).first;
    }
    internal_applyConfiguredLogLevel_onRegistering(lock, *regIter);
}

void LogManager::setLogLevel(const std::string& name, int logLevel)
{
    LockType lock(m_mutex);
    auto cfgIter = m_configuredLogLevels.find(name);
    if (cfgIter == m_configuredLogLevels.end())
    {
        cfgIter = m_configuredLogLevels.emplace(name, logLevel).first;
    }
    else
    {
        cfgIter->second = logLevel;
    }
    auto regIter = m_registeredLogLevels.find(name);
    if (regIter != m_registeredLogLevels.end())
    {
        *(regIter->second) = (cfgIter->second);
    }
}

int LogManager::getLogLevel(const std::string& name) const
{
    LockType lock(m_mutex);
    const auto regIter = m_registeredLogLevels.find(name);
    if (regIter != m_registeredLogLevels.end())
    {
        return *(regIter->second);
    }
    const auto cfgIter = m_configuredLogLevels.find(name);
    if (cfgIter != m_configuredLogLevels.end())
    {
        return (cfgIter->second);
    }
    return m_globalLogLevel;
}

int& LogManager::getGlobalLogLevelVariable()
{
    return m_globalLogLevel;
}

void LogManager::internal_applyConfiguredLogLevel_onRegistering(const LockType& /*lock*/,
    const std::pair<const std::string, int*>& registeredPair)
{
    const std::string& name = registeredPair.first;
    auto cfgIter = m_configuredLogLevels.find(name);
    if (cfgIter != m_configuredLogLevels.end())
    {
        *(registeredPair.second) = (cfgIter->second);
    }
}

int LogManager::defaultLogLevel()
{
#ifdef NDEBUG
    return LOG_LEVEL_WARNING;
#else
    return LOG_LEVEL_INFO;
#endif
}

int LogManager::parseLogLevelName(const std::string& s)
{
    if (s.length() == 0u)
    {
        return defaultLogLevel();
    }
    const char c = (char)std::toupper(s[0]);
    std::string s2 = s;
    static const auto toUpperFunc = [](char c2) -> char
    {
        return (char)std::toupper(c2);
    };
    std::transform(s2.begin(), s2.end(), s2.begin(), toUpperFunc);
    switch (c)
    {
    case 'D':
        if (s2 == "DISABLED")
        {
            return LOG_LEVEL_SILENT;
        }
        else if (s2 == "DEBUG" || s2 == "D")
        {
            return LOG_LEVEL_DEBUG;
        }
        else
        {
            break;
        }
    case 'O':
        if (s2 == "OFF" || s2 == "O")
        {
            return LOG_LEVEL_SILENT;
        }
        else
        {
            break;
        }
    case '0':
        return LOG_LEVEL_SILENT;
    case 'S':
        if (s2 == "SILENT" || s2 == "S")
        {
            return LOG_LEVEL_SILENT;
        }
        else
        {
            break;
        }
    case 'F':
        return LOG_LEVEL_FATAL;
    case 'E':
        return LOG_LEVEL_ERROR;
    case 'W':
        return LOG_LEVEL_WARNING;
    case 'I':
        return LOG_LEVEL_INFO;
    case 'V':
        return LOG_LEVEL_VERBOSE_10;
    default:
        break;
    }
    std::cerr << "ERROR: Unexpected logging level value: " << s << std::endl;
    return defaultLogLevel();
}

int LogManager::parseLogLevelChar(char c)
{
    const char c2 = (char)std::toupper(c);
    switch (c2)
    {
    case 'D':
        return LOG_LEVEL_DEBUG;
    case '0':
    case 'O':
    case 'S':
        return LOG_LEVEL_SILENT;
    case 'F':
        return LOG_LEVEL_FATAL;
    case 'E':
        return LOG_LEVEL_ERROR;
    case 'W':
        return LOG_LEVEL_WARNING;
    case 'I':
        return LOG_LEVEL_INFO;
    case 'V':
        return LOG_LEVEL_VERBOSE_10;
    default:
        std::cerr << "ERROR: Unexpected logging level value: " << c << std::endl;
        return defaultLogLevel();
    }
}

void LogManager::applyLogLevelConfigurationSingle(const std::string& strLogLevelSingle)
{
    const char* sep = "=:";
    const size_t npos = std::string::npos;
    const size_t sepPos = strLogLevelSingle.find_first_of(sep);
    if (sepPos != npos)
    {
        const size_t rhsPos = strLogLevelSingle.find_first_not_of(sep, sepPos);
        std::string tag = strLogLevelSingle.substr(0u, sepPos);
        std::string strLevel = strLogLevelSingle.substr(rhsPos);
        int level = parseLogLevelName(strLevel);
        setLogLevel(tag, level);
    }
    else
    {
        m_globalLogLevel = (int)parseLogLevelName(strLogLevelSingle);
    }
}

void LogManager::applyLogLevelConfigurationCompound(const std::string& strLogLevelCompound)
{
    const char* sep = " ,;";
    const size_t npos = std::string::npos;
    const size_t compoundLen = strLogLevelCompound.length();
    size_t compoundStart = 0u;
    while (compoundStart < compoundLen)
    {
        compoundStart = strLogLevelCompound.find_first_not_of(sep, compoundStart);
        if (compoundStart >= compoundLen)
        {
            break;
        }
        size_t sepPos = strLogLevelCompound.find_first_of(sep, compoundStart);
        size_t compoundStop = (sepPos == npos) ? compoundLen : sepPos;
        std::string strSingle = strLogLevelCompound.substr(compoundStart, compoundStop - compoundStart);
        if (!strSingle.empty())
        {
            applyLogLevelConfigurationSingle(strSingle);
        }
        compoundStart = compoundStop;
    }
}

void LogManager::applyLogLevelConfigurationCompound()
{
    std::string strLogLevelCompound = utils::getConfigurationParameterString("OPENCV_LOG_LEVEL", "");
    applyLogLevelConfigurationCompound(strLogLevelCompound);
}

}}} // namespace
