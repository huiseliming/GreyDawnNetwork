#include "Global.h"

namespace GreyDawn
{
    namespace Global
    {
        std::vector<std::function<void()>>& GetConstructorVector()
        {
            static  std::vector<std::function<void()>> ConstructorVector;
            return ConstructorVector;
        }
        std::vector<std::function<void()>>& GetDestructorVector()
        {
            static  std::vector<std::function<void()>> DestructorVector;
            return DestructorVector;
        }

        void ConstructorGlobalObject()
        {
            for (auto Constructor : GetConstructorVector())
                Constructor();
        }

        void DestructorGlobalObject()
        {
            for (auto Destructor : GetDestructorVector())
                Destructor();
        }
    }
}