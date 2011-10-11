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