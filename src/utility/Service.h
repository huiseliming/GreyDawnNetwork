﻿#pragma once
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
         * start and run the service
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

        static DWORD WINAPI ServiceControlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

        void Service::ReportServiceStatus();

        int Main();

    protected:

        HANDLE stop_event_ = INVALID_HANDLE_VALUE;


    private:
#ifdef _WIN32
        SERVICE_STATUS_HANDLE serviceStatusHandle;
        SERVICE_STATUS serviceStatus;
#elif __linux__

#else

#endif 
    };

}

