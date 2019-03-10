// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include "perf_precomp.hpp"
#include "opencv2/core/utils/logger.hpp"

#include "../src/utils/logtagmanager.hpp"
#include "../src/utils/logtagconfigparser.hpp"

// Because "LogTagManager" isn't exported from "opencv_core", the only way
// to perform test is to compile the source code into "opencv_perf_core".
// This workaround may cause step debugger breakpoints to work unreliably.
#if 1
#include "../src/utils/logtagmanager.cpp"
#include "../src/utils/logtagconfigparser.cpp"
#endif

namespace {

size_t& LoggerPerfTestSpyCounter()
{
    static size_t counter = 0u;
    return counter;
}

size_t LoggerPerfTestSpyCounterIncrement()
{
    return ++LoggerPerfTestSpyCounter();
}

size_t LoggerPerfTestSpyCounterRead()
{
    return LoggerPerfTestSpyCounter();
}

size_t LoggerPerfTestSpyCounterReset()
{
    size_t& countVariable = LoggerPerfTestSpyCounter();
    size_t oldCount = countVariable;
    countVariable = 0u;
    return oldCount;
}

} // namespace

namespace cv { namespace utils { namespace logging { namespace internal {

void writeNothingToLogEx(...)
{
    LoggerPerfTestSpyCounterIncrement();
}

}}}} // namespace

namespace
{

static constexpr const cv::utils::logging::LogTag LoggerPerfTestTag{ "whatever", cv::utils::logging::LOG_LEVEL_SILENT };

enum class TestTagSource
{
    UseProvidedTag,
    UseGlobalTag
};

} // namespace

namespace opencv_test {

using namespace perf;

typedef TestBaseWithParam<tuple<TestTagSource>> LogWithTagMacroPerfFixture;

PERF_TEST_P(LogWithTagMacroPerfFixture, LogWithTagMacroPerfTest,
    Values(TestTagSource::UseProvidedTag, TestTagSource::UseGlobalTag)
)
{
    const TestTagSource testTagSource = std::get<0u>(GetParam());
    const int repeatCount = 1000 * 1000;
    LoggerPerfTestSpyCounterReset();

#pragma push_macro("CV_LOGTAG_FALLBACK")
#pragma push_macro("CV_LOGTAG_GLOBAL")
#undef CV_LOGTAG_FALLBACK
#define CV_LOGTAG_FALLBACK nullptr
#define writeLogMessageEx writeNothingToLogEx

    PERF_SAMPLE_BEGIN()
    switch (testTagSource)
    {
    case TestTagSource::UseProvidedTag:
        for (int k = 0; k < repeatCount; ++k)
        {
            CV_LOG_ERROR(&LoggerPerfTestTag, ".");
        }
        break;
    case TestTagSource::UseGlobalTag:
        for (int k = 0; k < repeatCount; ++k)
        {
            CV_LOG_WITH_TAG(nullptr, 50, ".");
        }
        break;
    }
    PERF_SAMPLE_END()

#undef writeLogMessageEx
#pragma pop_macro("CV_LOGTAG_FALLBACK")

    const size_t spyCount = LoggerPerfTestSpyCounterReset();
    SANITY_CHECK(spyCount == repeatCount);
}

enum class NumNameParts
{
    One = 1001,
    Two_Unique = 2001,
    Two_SameFirst = 2002,
    Three_Unique = 3001,
    Three_SameFirst = 3002,
    Three_SameFirstSecond = 3003,
};

std::ostream& operator<< (std::ostream& s, NumNameParts v)
{
    switch (v)
    {
    case NumNameParts::One:
        s << "One";
        return s;
    case NumNameParts::Two_Unique:
        s << "Two_Unique";
        return s;
    case NumNameParts::Two_SameFirst:
        s << "Two_SameFirst";
        return s;
    case NumNameParts::Three_Unique:
        s << "Three_Unique";
        return s;
    case NumNameParts::Three_SameFirst:
        s << "Three_SameFirst";
        return s;
    case NumNameParts::Three_SameFirstSecond:
        s << "Three_SameFirstSecond";
        return s;
    default:
        throw std::logic_error("Invalid enum value for NumNameParts, see perf_logger.cpp");
    }
}

typedef TestBaseWithParam<tuple<NumNameParts, int>> LogTagManagerAssignPerfFixture;

PERF_TEST_P(LogTagManagerAssignPerfFixture, LogTagManagerAssignPerfTest,
    Combine(
        Values(NumNameParts::One,
            NumNameParts::Two_Unique,
            NumNameParts::Two_SameFirst,
            NumNameParts::Three_Unique,
            NumNameParts::Three_SameFirst,
            NumNameParts::Three_SameFirstSecond),
        Values(256, 512, 1024, 2048, 4096, 8192, 16384))
)
{
    using namespace cv::utils::logging;
    // Whether we are using name tags with one, two, or three name parts.
    // An example of three name parts is "coree.gemmm.blass"
    // Also, whether the name parts are same or unique across all LogTags.
    const NumNameParts numNameParts = std::get<0u>(GetParam());
    // How many LogTags to be registered (assigned) with LogTagManager
    const int testTagCount = std::get<1u>(GetParam());
    std::vector<std::string> logTagNames;
    std::vector<LogTag> logTags;
    logTagNames.reserve((size_t)testTagCount);
    logTags.reserve((size_t)testTagCount);
    for (int k = 0; k < testTagCount; ++k)
    {
        std::string tagName;
        switch (numNameParts)
        {
        case NumNameParts::One:
            tagName = "tagname" + std::to_string(k);
            break;
        case NumNameParts::Two_Unique:
            tagName = "firstpart" + std::to_string(k) + ".tagname" + std::to_string(k);
            break;
        case NumNameParts::Two_SameFirst:
            tagName = "firstpart.tagname" + std::to_string(k);
            break;
        case NumNameParts::Three_Unique:
            tagName = "firstpart" + std::to_string(k) + ".secondpart" + std::to_string(k) + ".tagname" + std::to_string(k);
            break;
        case NumNameParts::Three_SameFirst:
            tagName = "firstpart.secondpart" + std::to_string(k) + ".tagname" + std::to_string(k);
            break;
        case NumNameParts::Three_SameFirstSecond:
            tagName = "firstpart.secondpart.tagname" + std::to_string(k);
            break;
        default:
            FAIL() << "Invalid enum. Check parameterized test values.";
        }
        logTagNames.emplace_back(std::move(tagName));
    }
    for (int k = 0; k < testTagCount; ++k)
    {
        logTags.emplace_back(LogTag{ logTagNames.at(k).c_str(), LOG_LEVEL_DEBUG });
    }
    LogTagManager testLogTagManager(LOG_LEVEL_DEBUG);

    PERF_SAMPLE_BEGIN()
        for (int k = 0; k < testTagCount; ++k)
        {
            testLogTagManager.assign(logTagNames.at(k), &(logTags.at(k)));
        }
    PERF_SAMPLE_END()

    const bool hasLastItem = (testTagCount >= 1);
    if (hasLastItem)
    {
        const int lastTagIndex = (testTagCount - 1);
        const auto& lastTagName = logTagNames.at(lastTagIndex);
        const auto& lastTag = logTags.at(lastTagIndex);
        SANITY_CHECK(testLogTagManager.get(lastTagName) == &lastTag);
    }
    else
    {
        SANITY_CHECK(true);
    }
}

} // namespace
