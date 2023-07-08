#include <Pulsar/Logger.h>
#include <iostream>
#include <ctime>

namespace pulsar
{
    static void _GetTime(string& str)
    {
        time_t t;
        std::time(&t);
        struct tm* p = std::localtime(&t);
        char buf[128];
        ::sprintf_s(buf, 128, "%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec);
        str = buf;
    }

    string LogRecord::GetFriendlyInfo() const
    {
        string ret;
        ret.reserve(16 + this->time.size() + this->text.size());
        ret.append(Logger::GetLevelHead(this->level));
        ret.append("[");
        ret.append(this->time);
        ret.append("]");
        ret.append(this->text);
        return ret;
    }

    using namespace std;
    void Logger::Log(string_view str, LogLevel level)
    {
        LogRecord record;
        record.level = level;
        record.stacktrace = std::stacktrace::current(1);
        _GetTime(record.time);
        record.text.reserve(str.size() + 16);

        record.text.append(str);

        cout << GetLevelHead(level) << record.text << endl;

        LogListener.Invoke(record);
    }

    const char* Logger::GetLevelHead(LogLevel level)
    {
        switch (level)
        {
        case pulsar::LogLevel::Info:
            return "[Info]";
        case pulsar::LogLevel::Warning:
            return"[Warning]";
        case pulsar::LogLevel::Error:
            return "[Error]";
        }
        return nullptr;
    }

}