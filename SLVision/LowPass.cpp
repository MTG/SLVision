/*
 * Copyright (C) 2011-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of SLVision
 *
 * SLVision is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/*
 *	Daniel Gallardo Grassot
 *	daniel.gallardo@upf.edu
 *	Barcelona 2011
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
