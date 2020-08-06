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
        static T& instance()
        {
            static Singleton<T> singleton;
            return singleton;
        }
    };
}


