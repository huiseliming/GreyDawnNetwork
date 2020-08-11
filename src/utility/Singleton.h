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
            //����֧��c++11���ԣ�c++11��ǰ����֤static local��ʼ�����̰߳�ȫ
            static Singleton<T> singleton;
            return singleton;
        }
    };
}


