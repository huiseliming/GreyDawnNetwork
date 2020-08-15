#include "utility/Service.h"
#include "utility/Singleton.h"
#include "utility/Logger.h"

namespace GreyDawn{

    Service::Service()
    {
        stop_event_ = CreateEvent(NULL, TRUE, FALSE, NULL);
        (void)memset(&serviceStatus, 0, sizeof(serviceStatus));
        serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        serviceStatus.dwControlsAccepted = (
            SERVICE_ACCEPT_STOP
            );
        serviceStatus.dwWin32ExitCode = NO_ERROR;
    }
    Service::~Service()
    {
        if (!CloseHandle(stop_event_))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
    }

    int Service::Start() {
        auto name = Singleton<Service>::Instance().GetServiceName();
        SERVICE_TABLE_ENTRYA dispatchTable[] = {
            {(LPSTR)name.data(), ServiceMain},
            {NULL, NULL}
        };
        if (!StartServiceCtrlDispatcherA(dispatchTable)) {
            return EXIT_SUCCESS;
        }
        else {
            return EXIT_FAILURE;
        }
    }

    //Window Callback
    VOID WINAPI Service::ServiceMain(DWORD dwArgc, LPSTR* lpszArgv) {
        Singleton<Service>::Instance().Main();
    }

    DWORD WINAPI Service::ServiceControlHandler(
        DWORD dwControl,
        DWORD dwEventType,
        LPVOID lpEventData,
        LPVOID lpContext
    ) {
        switch (dwControl)
        {
        case SERVICE_CONTROL_STOP: 
            Singleton<Service>::Instance().Stop();
            return NO_ERROR;
        case SERVICE_CONTROL_INTERROGATE: 
            return NO_ERROR;
        default: 
            return ERROR_CALL_NOT_IMPLEMENTED;
        }
    }
    void Service::ReportServiceStatus() {
        (void)SetServiceStatus(serviceStatusHandle, &serviceStatus);
    }

    int Service::Main() {
        serviceStatusHandle = RegisterServiceCtrlHandlerExA(
            Singleton<Service>::Instance().GetServiceName().c_str(),
            ServiceControlHandler,
            NULL
        );
        serviceStatus.dwCurrentState = SERVICE_RUNNING;
        ReportServiceStatus();
        const auto runResult = Singleton<Service>::Instance().Run();
        if (runResult == 0) {
            serviceStatus.dwWin32ExitCode = NO_ERROR;
        }
        else {
            serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
            serviceStatus.dwServiceSpecificExitCode = (DWORD)runResult;
        }
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        ReportServiceStatus();
        return runResult;
    }
    //The default implement for compiled
    int Service::Run() {
        GD_LOG_INFO("{} Running", GetServiceName());
        WaitForSingleObject(stop_event_, INFINITE);
        GD_LOG_INFO("{} Stoped", GetServiceName());
        return EXIT_SUCCESS;
    }
    //The default implement for compiled
    void Service::Stop()
    {
        SetEvent(stop_event_);
    }
    //The default implement for compiled
    std::string Service::GetServiceName()
    {
        return "GreyDawnService";
    }
}


























