#pragma once

class UsageTrace
{
public:
	UsageTrace();
	UsageTrace(int Port);
	UsageTrace(const UsageTrace &other);
	~UsageTrace();
	bool operator==(const UsageTrace &other)const;
	bool operator<(const UsageTrace &other)const;
	void InsertUsage(const int usage);
	std::list<int>::iterator begin();
	std::list<int>::iterator end();
private:
	int port;
	std::list<int> trace;
	const int managedTraceNumber;
};

typedef std::set<UsageTrace> TraceSet;