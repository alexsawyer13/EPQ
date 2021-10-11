#pragma once

#include <string>
#include <chrono>

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