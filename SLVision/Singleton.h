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

#pragma once

//from ofxTableGestures
//Inspired by http://cc.byexamples.com/20080609/stl-singleton-template/

class VoidClass{};
class VoidClass2{};
template<typename T, class Base=VoidClass, class Base2=VoidClass2>
class Singleton : public Base, public Base2
{
    public:
        static T& Instance()
        {
            static T me;
            return me;
        }
        static T* get()
        {
            return &Instance();
        }
};
