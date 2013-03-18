#include "LowPass.h"


LowPass::LowPass(int queue_size)
{
	value = 0;
	queuesize = queue_size;
}


LowPass::~LowPass(void)
{
}

float LowPass::addvalue(float value)
{
	data.push_back(value);

	while ( data.size() > queuesize)
		data.pop_front();

	value = 0;
	for(std::list<float>::iterator it = data.begin();
		it != data.end(); 
		it++)
	{
		value += (*it);
	}

	value = value / data.size();

	return value;
}

float LowPass::getvalue ()
{
	return value;
}