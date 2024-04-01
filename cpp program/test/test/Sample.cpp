#include "jni.h"
#include "pch.h"
#include "org_example_Test.h" 
#define _WIN32_DCOM
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <vector>
#include <windows.h>
#include<jni.h>
#include<string>

using namespace std;

#pragma comment(lib, "wbemuuid.lib")

JNIEXPORT jobjectArray JNICALL Java_org_example_Test_getArray(JNIEnv* env , jobject obj){ 
    jclass cls = env->FindClass("java/lang/Object");
    jobjectArray result = env->NewObjectArray(10, cls, NULL);


    HRESULT hres;

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        cout << "Failed to initialize COM library" << endl;
        return nullptr;
    }

    hres = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);

    if (FAILED(hres))
    {
        cout << "Failed to initialize security." << endl;
        CoUninitialize();
        return nullptr;
    }
    IWbemLocator* pLoc = nullptr;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        cout << "Failed to create IWbemLocator object" << endl;
        CoUninitialize();
        return nullptr;
    }

    IWbemServices* pSvc = nullptr;

    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        nullptr,
        nullptr,
        0,
        0,
        0,
        0,
        &pSvc);

    if (FAILED(hres))
    {
        cout << "Could not connect" << endl;
        pLoc->Release();
        CoUninitialize();
        return nullptr;
    }


    hres = CoSetProxyBlanket(
        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE);

    if (FAILED(hres))
    {
        cout << "Could not set proxy blanket" << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return nullptr;
    }

    IEnumWbemClassObject* pEnumerator = nullptr;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_NTLogEvent WHERE Logfile = 'Security'"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        cout << "Query for security events failed." << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return nullptr;
    }

    if (!pEnumerator)
    {
        cout << "Enumerator is null." << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return nullptr;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    int count = 0;
    while (pEnumerator && count < 10)
    {
        LONG eventCode = 0;
        LONG type = 0;
        VARIANT vtEventCode;
        VariantInit(&vtEventCode);

        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;
        hres = pclsObj->Get(L"EventCode", 0, &vtEventCode, 0, 0);
        if (SUCCEEDED(hres) && vtEventCode.vt == VT_I4) {
            eventCode = vtEventCode.lVal;
        }
        else {
            std::wcerr << L"Failed to retrieve EventCode." << std::endl;
        }
        char* sourceName = nullptr ;
        char* timeGenerated = nullptr;
        hres = pclsObj->Get(L"SourceName", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hres)) {
            sourceName = _com_util::ConvertBSTRToString(vtProp.bstrVal);
        }
        else {
            std::wcerr << L"Failed to retrieve SourceName." << std::endl;
        }
       
        jobjectArray obj = env->NewObjectArray(2, cls, NULL);
        env->SetObjectArrayElement(obj, 0, env->NewStringUTF(std::to_string(eventCode).c_str()));
        env->SetObjectArrayElement(obj, 1, env->NewStringUTF(sourceName)); // Set timeGenerated here
        env->SetObjectArrayElement(result, count, obj);
        VariantClear(&vtEventCode);
        delete[] sourceName;
        delete[] timeGenerated;

        pclsObj->Release();
        pclsObj = nullptr;
        count++;
    }

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();
    return result;

}