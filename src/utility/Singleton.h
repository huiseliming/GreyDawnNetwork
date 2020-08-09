#pragma once

namespace GreyDawn
{
    template <typename T>
    class Singleton : private T
    {
    protected:
        Singleton(){}
        ~Singleton(){}
    public:
        static T& Instance()
        {
            //必须支持c++11特性，c++11以前不保证static local初始化的线程安全
            static Singleton<T> singleton;
            return singleton;
        }
    };
}


