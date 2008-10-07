#include "stdafx.h"
#include "UsageTrace.h"

UsageTrace::UsageTrace():port(0),managedTraceNumber(100)
{
}
UsageTrace::UsageTrace(int Port):port(Port),managedTraceNumber(100)
{
}
UsageTrace::UsageTrace(const UsageTrace &other):port(other.port),managedTraceNumber(100)
{
	if(other.trace.size())
	{
		trace.resize(other.trace.size());
		std::copy(other.trace.begin(), other.trace.end(), trace.begin());
	}
}
UsageTrace::~UsageTrace()
{
}
bool UsageTrace::operator==(const UsageTrace &other)const
{
	return port == other.port;
}
bool UsageTrace::operator<(const UsageTrace &other)const
{
	return port < other.port;
}
void UsageTrace::InsertUsage(const int usage)
{
	trace.push_back(usage);
	while(trace.size()>managedTraceNumber)
		trace.pop_front();
}
std::list<int>::iterator UsageTrace::begin()
{
	return trace.begin();
}
std::list<int>::iterator UsageTrace::end()
{
	return trace.end();
}