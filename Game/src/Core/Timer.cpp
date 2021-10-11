#include <Core/Timer.h>

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