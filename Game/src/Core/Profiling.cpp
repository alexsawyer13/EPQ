#include <Core/Profiling.h>
#include <Core/Core.h>
#include <Core/Assets.h>

#include <spdlog/spdlog.h>

#include <fstream>
#include <ctime>


ProfileTimerMs::ProfileTimerMs(const std::string &name)
{
	m_Name = name;
	m_Start = std::chrono::system_clock::now();
}

ProfileTimerMs::~ProfileTimerMs()
{
	auto iter = core.profiler.current_frame.Milliseconds.find(m_Name);
	if (iter == core.profiler.current_frame.Milliseconds.end())
	{
		core.profiler.current_frame.Milliseconds[m_Name] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_Start).count();
	}
	else
	{
		core.profiler.current_frame.Milliseconds[m_Name] += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_Start).count();
	}
}

ProfileTimerUs::ProfileTimerUs(const std::string &name)
{
	m_Name = name;
	m_Start = std::chrono::system_clock::now();
}

ProfileTimerUs::~ProfileTimerUs()
{
	auto iter = core.profiler.current_frame.Microseconds.find(m_Name);
	if (iter == core.profiler.current_frame.Microseconds.end())
	{
		core.profiler.current_frame.Microseconds[m_Name] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - m_Start).count();
	}
	else
	{
		core.profiler.current_frame.Microseconds[m_Name] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - m_Start).count();
	}
}

void StartProfilerFrame()
{
	core.profiler.current_frame.Milliseconds.clear();
	core.profiler.current_frame.Microseconds.clear();
	core.profiler.frame_timer.Reset();
}

void EndProfilerFrame()
{
	core.profiler.last_frame = core.profiler.current_frame;
	core.profiler.last_frame_time = core.profiler.frame_timer.Time();
}

ScopeTimerMs::ScopeTimerMs(const std::string &name)
{
	m_Name = name;
	m_Start = std::chrono::system_clock::now();
}

ScopeTimerMs::~ScopeTimerMs()
{
	printf("%s: %lld milliseconds\n", m_Name.c_str(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_Start).count());
}

ScopeTimerUs::ScopeTimerUs(const std::string &name)
{
	m_Name = name;
	m_Start = std::chrono::system_clock::now();
}

ScopeTimerUs::~ScopeTimerUs()
{
	printf("%s: %lld microseconds\n", m_Name.c_str(), std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - m_Start).count());
}

TimerMs::TimerMs()
{
	Reset();
}

void TimerMs::Reset()
{
	m_Start = std::chrono::system_clock::now();
}

long long TimerMs::Time()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_Start).count();
}

TimerUs::TimerUs()
{
	Reset();
}

void TimerUs::Reset()
{
	m_Start = std::chrono::system_clock::now();
}
long long TimerUs::Time()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - m_Start).count();
}

void CsvAddData(CsvFile &csv, const std::string &colname, int data)
{
	if (csv.hashed_columns.find(colname) == csv.hashed_columns.end())
	{
		csv.hashed_columns[colname] = csv.columns.size();
		csv.columns.push_back(colname);
	}
	csv.current_row[colname] = data;
}

void CsvNextRow(CsvFile &csv)
{
	csv.rows.push_back(csv.current_row);
	csv.current_row.clear();
}

void CsvFlush(CsvFile &csv)
{
	SCOPE_TIMER_US("CsvFlush");

	std::string filepath = s_ProjectDir + "profiling/" + core.profiler.session_string + "/" + GetTimeFormatted() + ".csv";
	
	std::ofstream file;
	file.open(filepath);
	if (!file.is_open())
	{
		spdlog::error("Failed to flush csv data, file \"{}\" could not be opened", filepath);
		return;
	}
	else
	{
		spdlog::debug("Flushing to csv file \"{}\"", filepath);
	}

	// Header
	for (int i = 0; i < csv.columns.size(); i++)
	{
		file << csv.columns[i];
		if (i < csv.columns.size() - 1)
		{
			file << ",";
		}
		else
		{
			file << "\n";
		}
	}

	// Rows
	for (auto &row : csv.rows)
	{
		for (int i = 0; i < csv.columns.size(); i++)
		{
			auto iter = row.find(csv.columns[i]);
			if (iter != row.end())
			{
				file << row[csv.columns[i]];
			}
			if (i < csv.columns.size() - 1)
			{
				file << ",";
			}
			else
			{
				file << "\n";
			}
		}
	}

	file.close();

	core.profiler.most_recent_csv = filepath;
}

std::string GetTimeFormatted()
{
	std::tm bt = {};
	std:: time_t timer = std::time(0);
	localtime_s(&bt, &timer);
	
	char buffer[128];
	std::strftime(buffer, 128, "%F %T", &bt);

	for (int i = 0; i < 128; i++)
	{
		if (buffer[i] == ':')
			buffer[i] = '-';
	}

	return std::string(buffer);
}