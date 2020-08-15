#include "utility/Service.h"
#include "utility/Logger.h"

namespace GreyDawn{

    Service* Service::Instance = nullptr;

    Service::Service()
    {
        stop_event_ = CreateEvent(NULL, TRUE, FALSE, NULL);
        (void)memset(&service_status_, 0, sizeof(service_status_));
        service_status_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        service_status_.dwCurrentState = SERVICE_STOPPED;
        service_status_.dwControlsAccepted = (
            SERVICE_ACCEPT_STOP
            );
        service_status_.dwWin32ExitCode = NO_ERROR;
    }

    Service::~Service()
    {
        if (!CloseHandle(stop_event_))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
    }

    bool Service::Install()
    {
        SC_HANDLE sc_manager_handle = NULL;
        SC_HANDLE service_handle = NULL;
        try{
            sc_manager_handle = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
            if(sc_manager_handle == NULL) 
                throw std::exception("Win32 API OpenSCManagerA return NULL");

            service_handle = CreateServiceA(sc_manager_handle,
                GetServiceName().c_str(),
                GetServiceName().c_str(),
                SERVICE_ALL_ACCESS,
                SERVICE_WIN32_OWN_PROCESS,
                SERVICE_DEMAND_START,
                SERVICE_ERROR_NORMAL,
                GetExecuteFileAbsolutePath().c_str(),
                NULL,
                NULL,
                NULL,
                NULL,
                NULL);
            if(service_handle == NULL) 
                throw std::exception("Win32 API CreateServiceA return NULL");
            GD_LOG_INFO("Service {} installed successfully", GetServiceName().c_str());
        } catch (const std::exception& e) {
            GD_LOG_ERROR("[std::exception>{}]", e.what());
            GD_LOG_OUTPUT_SYSTEM_ERROR();
            if (sc_manager_handle)
                if (!CloseServiceHandle(sc_manager_handle))
                    GD_LOG_OUTPUT_SYSTEM_ERROR();
            if (service_handle)
                if (!CloseServiceHandle(service_handle))
                    GD_LOG_OUTPUT_SYSTEM_ERROR();
            return false;
        } 
        if (!CloseServiceHandle(sc_manager_handle))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        if (!CloseServiceHandle(service_handle))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        return true;
    }

    bool Service::Uninstall()
    {
        SC_HANDLE sc_manager_handle = NULL;
        SC_HANDLE service_handle = NULL;
        try{
            sc_manager_handle = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
            if (sc_manager_handle == NULL) 
                throw std::exception("Win32 API OpenSCManagerA return NULL");

            service_handle = OpenServiceA(sc_manager_handle,
                GetServiceName().c_str(),
                SERVICE_ALL_ACCESS);
            if (service_handle == NULL) 
                throw std::exception("Win32 API OpenServiceA return NULL");

            SERVICE_STATUS service_status;
            if(QueryServiceStatus(service_handle, &service_status)== 0) 
                throw std::exception("Win32 API QueryServiceStatus Failed");
            if (service_status.dwCurrentState == SERVICE_RUNNING) {
                if(ControlService(service_handle, SERVICE_CONTROL_STOP, &service_status)== 0) 
                    throw std::exception("Win32 API ControlService Failed");
                if(service_status.dwCurrentState != NO_ERROR){
                    throw std::exception("service_status.dwCurrentState != NO_ERROR");
                }
                do {
                    if (QueryServiceStatus(service_handle, &service_status) == 0)
                        throw std::exception("Win32 API QueryServiceStatus Failed");
                    Sleep(1000);
                } while (service_status.dwCurrentState != SERVICE_STOPPED);
            }
            if(DeleteService(service_handle) == 0)
                throw std::exception("Win32 API DeleteService Failed");
            GD_LOG_INFO("Service {} uninstalled successfully", GetServiceName().c_str());
        } catch (const std::exception& e) {
            GD_LOG_ERROR("[std::exception>{}]", e.what());
            GD_LOG_OUTPUT_SYSTEM_ERROR();
            if (sc_manager_handle)
                if (!CloseServiceHandle(sc_manager_handle))
                    GD_LOG_OUTPUT_SYSTEM_ERROR();
            if (service_handle)
                if (!CloseServiceHandle(service_handle))
                    GD_LOG_OUTPUT_SYSTEM_ERROR();
            return false;
        }
        if (!CloseServiceHandle(sc_manager_handle))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        if (!CloseServiceHandle(service_handle))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        return true;
    }

    int Service::Start() {
        assert(Service::Instance == nullptr && "Start call only once");
        Service::Instance = this;
        auto name = Service::Instance->GetServiceName();
        SERVICE_TABLE_ENTRYA dispatch_table[] = {
            {(LPSTR)name.data(), (LPSERVICE_MAIN_FUNCTION)Service::ServiceMain},
            {NULL, NULL}
        };
        if (StartServiceCtrlDispatcherA(dispatch_table)) {
            return EXIT_SUCCESS;
        } else {
            GD_LOG_OUTPUT_SYSTEM_ERROR();
            return EXIT_FAILURE;
        }
    }

    //Window Callback
    VOID WINAPI Service::ServiceMain(DWORD dwArgc, LPSTR* lpszArgv) 
    {
        Service::Instance->Main();
    }

    VOID WINAPI Service::ServiceControlHandler(DWORD dwControl) 
    {
        switch (dwControl)
        {
        case SERVICE_CONTROL_STOP: 
            Service::Instance->Stop();
            break;
        default:
            break;
        }
    }

    void Service::ReportServiceStatus() {
        (void)SetServiceStatus(service_status_handle_, &service_status_);
    }

    int Service::Main() {
        service_status_handle_ = RegisterServiceCtrlHandler(
            Service::Instance->GetServiceName().c_str(),
            Service::ServiceControlHandler);
        if (!service_status_handle_)
        {
            return EXIT_FAILURE;
        }
        service_status_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        service_status_.dwServiceSpecificExitCode = 0;
        service_status_.dwCurrentState = SERVICE_RUNNING;
        ReportServiceStatus();
        const auto runResult = Service::Instance->Run();
        if (runResult == 0) {
            service_status_.dwWin32ExitCode = NO_ERROR;
        } else {
            service_status_.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
            service_status_.dwServiceSpecificExitCode = (DWORD)runResult;
        }
        service_status_.dwCurrentState = SERVICE_STOPPED;
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


























