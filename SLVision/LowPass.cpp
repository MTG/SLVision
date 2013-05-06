/*
	Daniel Gallardo Grassot
	daniel.gallardo@upf.edu
	Barcelona 2013

	Licensed to the Apache Software Foundation (ASF) under one
	or more contributor license agreements.  See the NOTICE file
	distributed with this work for additional information
	regarding copyright ownership.  The ASF licenses this file
	to you under the Apache License, Version 2.0 (the
	"License"); you may not use this file except in compliance
	with the License.  You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.
*/

#include "LowPass.h"


LowPass::LowPass(unsigned int queue_size)
{
	value = 0;
	queuesize = queue_size;
}

LowPass::LowPass(const LowPass &copy):
	queuesize(copy.queuesize),
	value(copy.value),
	data(std::list<float>(copy.data))
{
	
}

LowPass::~LowPass(void)
{
}

float LowPass::addvalue(float _value)
{
	data.push_back(_value);

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

void LowPass::Reset()
{
	data.clear();
	value = 0;
}