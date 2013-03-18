#pragma once
#include <list>

class LowPass
{
private:
	int queuesize;
	std::list<float> data;
	float value;
public:

	///folat addvalue(float value);
	//adds a velue to the queue and return the med value

	///float getvalue ()
	//return the med value

	///LowPass(int queue_size)
	LowPass(int queue_size);
	float addvalue(float value);
	float getvalue ();

	~LowPass(void);
};

