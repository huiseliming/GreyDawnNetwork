#pragma once
#ifdef _WIN32
#include <Windows.h>
#elif __linux__

#else

#endif 

#include <memory>
#include <string>
#include <vector>

namespace GreyDawn 
{
    class Service {
    public:
        Service();
        ~Service();
        Service(const Service&) = delete;
        Service(Service&&) = default;
        Service& operator=(const Service&) = delete;
        Service& operator=(Service&&) = default;

        /*
         * install the service 
         */
        bool Install();

        /*
         * uninstall the service
         */
        bool Uninstall();

        /*
         * run the service
         */
        int Start();

        /*
         * override virtual function Run implement service MainLoop
         */
        virtual int Run();

        /*
         * override virtual function Stop stop service
         */
        virtual void Stop();

        /*
         * override virtual function GetServiceName name service
         */
        virtual std::string GetServiceName();

    protected:
        static VOID WINAPI ServiceMain(DWORD dwArgc, LPSTR* lpszArgv);

        static VOID WINAPI ServiceControlHandler(DWORD dwControl);

        void ReportServiceStatus();

        int Main();

    protected:
        HANDLE stop_event_ = INVALID_HANDLE_VALUE;

        static Service* Instance;

    private:
#ifdef _WIN32
        SERVICE_STATUS_HANDLE service_status_handle_;
        SERVICE_STATUS service_status_;
#elif __linux__

#else

#endif 
    };

}

