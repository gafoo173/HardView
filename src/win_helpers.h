#ifndef WIN_HELPERS_H
#define WIN_HELPERS_H

#ifdef _WIN32
#include <Wbemidl.h>
#include <wtypes.h>
#include <stdio.h>

char* _get_wmi_string_property(IWbemClassObject* pclsObj, const WCHAR* name);
ULONG _get_wmi_ulong_property(IWbemClassObject* pclsObj, const WCHAR* name);
LONGLONG _get_wmi_longlong_property(IWbemClassObject* pclsObj, const WCHAR* name);
int _get_wmi_bool_property(IWbemClassObject* pclsObj, const WCHAR* name);
HRESULT _initialize_wmi(IWbemLocator **pLoc, IWbemServices **pSvc);
void _cleanup_wmi(IWbemLocator *pLoc, IWbemServices *pSvc, IEnumWbemClassObject *pEnumerator, IWbemClassObject *pclsObj);

#endif // _WIN32

#endif // WIN_HELPERS_H 