#pragma once

#include <string>
#include <chrono>
#include <unordered_map>

#ifdef PROFILER_ENABLE_ALL
	#define PROFILER_ENABLE_CSV
	#define PROFILER_ENABLE_TIMERS
	#define PROFILER_ENABLE_PROFILER
#endif

#ifdef PROFILER_ENABLE_CSV
	#define PROFILER_CSV_START() ProfilerCsvStart()
	#define PROFILER_CSV_FLUSH() ProfilerCsvFlush()
	#define PROFILER_CSV_PUSH(number) ProfilerCsvPush(number)
	#define PROFILER_CSV_NEWLINE() ProfilerCsvNewline()
#else
	#define PROFILER_CSV_START() 0
	#define PROFILER_CSV_FLUSH() 0
	#define PROFILER_CSV_PUSH(number) 0
	#define PROFILER_CSV_NEWLINE() 0
#endif

#ifdef PROFILER_ENABLE_PROFILER
	#define START_PROFILER_FRAME() StartProfilerFrame()
	#define PROFILE_SCOPE_MS(name) ProfileTimerMs __timer(name)
	#define PROFILE_SCOPE_US(name) ProfileTimerUs __timer(name)
	#define END_PROFILER_FRAME() EndProfilerFrame()
#else
	#define START_FRAME() 0
	#define PROFILE_SCOPE(name) 0
	#define END_FRAME() 0
#endif

#ifdef PROFILER_ENABLE_TIMERS
	#define SCOPE_TIMER_MS(name) ScopeTimerMs __timer(name)
	#define SCOPE_TIMER_US(name) ScopeTimerUs __timer(name)
#else
	#define SCOPE_TIMER_MS(name) 0
	#define SCOPE_TIMER_US(name) 0
#endif

struct ProfilerFrame
{
	std::unordered_map<std::string, long long> Milliseconds;
	std::unordered_map<std::string, long long> Microseconds;
};

class ProfileTimerMs
{
public:
	ProfileTimerMs(const std::string &name);
	~ProfileTimerMs();

private:
	std::string m_Name;
	std::chrono::system_clock::time_point m_Start;
};

class ProfileTimerUs
{
public:
	ProfileTimerUs(const std::string &name);
	~ProfileTimerUs();

private:
	std::string m_Name;
	std::chrono::system_clock::time_point m_Start;
};

class ScopeTimerMs
{
public:
	ScopeTimerMs(const std::string &name);
	~ScopeTimerMs();

private:
	std::string m_Name;
	std::chrono::system_clock::time_point m_Start;
};

class ScopeTimerUs
{
public:
	ScopeTimerUs(const std::string &name);
	~ScopeTimerUs();

private:
	std::string m_Name;
	std::chrono::system_clock::time_point m_Start;
};

class TimerMs
{
public:
	TimerMs();
	void Reset();
	long long Time();

private:
	std::chrono::system_clock::time_point m_Start;
};

class TimerUs
{
public:
	TimerUs();
	void Reset();
	long long Time();

private:
	std::chrono::system_clock::time_point m_Start;
};

struct CsvFile
{
	std::vector<std::string> columns;
	std::unordered_map<std::string, int> hashed_columns;

	std::vector<std::unordered_map<std::string, int>> rows;
	std::unordered_map<std::string, int> current_row;
};

void CsvAddData(CsvFile &csv, const std::string &column, int data);
void CsvNextRow(CsvFile &csv);
void CsvFlush(CsvFile &csv);

struct Profiler
{
	std::string session_string;
	std::string most_recent_csv;

	ProfilerFrame current_frame;
	ProfilerFrame last_frame;
	TimerUs frame_timer;
	long long last_frame_time;
	CsvFile current_csv;
};

void StartProfilerFrame();
void EndProfilerFrame();

std::string GetTimeFormatted();