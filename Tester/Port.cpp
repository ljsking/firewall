#include "stdafx.h"
#include <vector>
#include "Port.h"

Port::Port():port(0)
{
	//usage = new int(0);
	//state = new int(0);
}

Port::Port(int Port, int State):port(Port)
{
	//usage = new int(0);
	//state = new int(State);
}

Port::Port(const Port &other)
{
	port = other.port;
	//usage = new int(*other.usage);
	//state = new int(*other.state);
}

Port::~Port()
{
	//delete usage;
	//delete state;
}

bool Port::operator==(const Port &other)const
{
	return port == other.port;
}

bool Port::operator<(const Port &other)const
{
	return port<other.port;
}

const int Port::GetPort()const
{
	return port;
}
