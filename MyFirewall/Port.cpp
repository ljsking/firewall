#include "stdafx.h"
#include "Port.h"

Port::Port():port(0), usage(0), state(0)
{
}

Port::Port(int Port, int State):port(Port), usage(0), state(State)
{
}

Port::Port(const Port &other):port(other.port), usage(other.usage), state(other.state)
{
}

Port::~Port()
{
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

const int Port::GetUsage()const
{
	return usage;
}

void Port::SetUsage(const int Usage)
{
	usage = Usage;
}

const int Port::GetState()const
{
	return state;
}

void Port::SetState(const int State)
{
	state = State;
}