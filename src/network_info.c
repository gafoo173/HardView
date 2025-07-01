#include "network_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#include <Wbemidl.h>
#include <wtypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#else
#include "linux_helpers.h"
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#endif

#define INITIAL_JSON_BUFFER_SIZE 2048
#define MAX_INFO_LEN 1024

char* get_network_info_json() {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* current_net_json_part = NULL;
    char* full_json_string = (char*)malloc(INITIAL_JSON_BUFFER_SIZE);
    size_t current_buffer_size = INITIAL_JSON_BUFFER_SIZE;
    size_t current_len = 0;
    ULONG uReturn = 0;

    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for Network JSON buffer.\"}");
    strcpy(full_json_string, "{\"network_adapters\": [");
    current_len = strlen(full_json_string);

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        free(full_json_string);
        return _create_json_string("{\"error\": \"Failed to initialize WMI for network info: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Description, MACAddress, IPAddress, DNSHostName, IPEnabled FROM Win32_NetworkAdapterConfiguration WHERE IPEnabled = TRUE");
    if (!query_lang || !query) {
        SysFreeString(query_lang); SysFreeString(query);
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        free(full_json_string);
        return _create_json_string("{\"error\": \"Memory allocation failed for WMI query strings.\"}");
    }

    hr = pSvc->lpVtbl->ExecQuery(
        pSvc, query_lang, query,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator
    );
    SysFreeString(query_lang);
    SysFreeString(query);

    if (FAILED(hr)) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        free(full_json_string);
        return _create_json_string("{\"error\": \"Failed to execute WMI query for NetworkAdapter: 0x%lx\"}", hr);
    }

    int first_adapter = 1;
    while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        char* description = _get_wmi_string_property(pclsObj, L"Description");
        char* mac_address = _get_wmi_string_property(pclsObj, L"MACAddress");
        char* dns_host_name = _get_wmi_string_property(pclsObj, L"DNSHostName");

        // Handle IPAddress array (VT_ARRAY | VT_BSTR)
        VARIANT vtIpAddresses;
        VariantInit(&vtIpAddresses);
        BSTR ip_addresses_bstr_array_json = SysAllocString(L"[]");
        HRESULT hr_ip = pclsObj->lpVtbl->Get(pclsObj, L"IPAddress", 0, &vtIpAddresses, 0, 0);
        if (SUCCEEDED(hr_ip) && (vtIpAddresses.vt == (VT_ARRAY | VT_BSTR)) && vtIpAddresses.parray) {
            SAFEARRAY *sa = vtIpAddresses.parray;
            BSTR *bstr_array;
            hr_ip = SafeArrayAccessData(sa, (void HUGEP* FAR*)&bstr_array);
            if (SUCCEEDED(hr_ip)) {
                long lbound, ubound;
                SafeArrayGetLBound(sa, 1, &lbound);
                SafeArrayGetUBound(sa, 1, &ubound);
                size_t est_ip_list_len = (ubound - lbound + 1) * (46 + 4) + 2;
                char* ip_list_buffer = (char*)malloc(est_ip_list_len);
                if (ip_list_buffer) {
                    strcpy(ip_list_buffer, "[");
                    size_t current_ip_list_len = strlen(ip_list_buffer);
                    for (long i = lbound; i <= ubound; i++) {
                        if (i > lbound) {
                            strcat(ip_list_buffer, ",");
                            current_ip_list_len += 1;
                        }
                        char* current_ip_str = NULL;
                        int current_ip_len = WideCharToMultiByte(CP_UTF8, 0, bstr_array[i], -1, NULL, 0, NULL, NULL);
                        if (current_ip_len > 0) {
                            current_ip_str = (char*)malloc(current_ip_len);
                            if (current_ip_str) {
                                WideCharToMultiByte(CP_UTF8, 0, bstr_array[i], -1, current_ip_str, current_ip_len, NULL, NULL);
                                if (current_ip_list_len + strlen(current_ip_str) + 3 < est_ip_list_len) {
                                    strcat(ip_list_buffer, "\"");
                                    strcat(ip_list_buffer, current_ip_str);
                                    strcat(ip_list_buffer, "\"");
                                    current_ip_list_len += strlen(current_ip_str) + 2;
                                }
                                free(current_ip_str);
                            }
                        }
                    }
                    strcat(ip_list_buffer, "]");
                    int wide_len = MultiByteToWideChar(CP_UTF8, 0, ip_list_buffer, -1, NULL, 0);
                    if (wide_len > 0) {
                        WCHAR* wide_ip_list = (WCHAR*)malloc(wide_len * sizeof(WCHAR));
                        if (wide_ip_list) {
                            MultiByteToWideChar(CP_UTF8, 0, ip_list_buffer, -1, wide_ip_list, wide_len);
                            SysFreeString(ip_addresses_bstr_array_json);
                            ip_addresses_bstr_array_json = SysAllocString(wide_ip_list);
                            free(wide_ip_list);
                        }
                    }
                    free(ip_list_buffer);
                }
                SafeArrayUnaccessData(sa);
            }
        }
        VariantClear(&vtIpAddresses);
        current_net_json_part = _create_json_string(
            "%s{\"description\": \"%s\", \"mac_address\": \"%s\", \"ip_addresses\": %S, \"dns_host_name\": \"%s\"}",
            first_adapter ? "" : ",", description, mac_address, ip_addresses_bstr_array_json, dns_host_name
        );
        first_adapter = 0;
        free(description); free(mac_address); free(dns_host_name);
        SysFreeString(ip_addresses_bstr_array_json);
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        if (current_net_json_part) {
            size_t part_len = strlen(current_net_json_part);
            if (current_len + part_len + 3 >= current_buffer_size) {
                current_buffer_size += part_len + INITIAL_JSON_BUFFER_SIZE;
                char* temp = (char*)realloc(full_json_string, current_buffer_size);
                if (!temp) {
                    free(full_json_string); free(current_net_json_part);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{\"error\": \"Memory re-allocation failed for Network JSON buffer.\"}");
                }
                full_json_string = temp;
            }
            strcat(full_json_string, current_net_json_part);
            current_len += part_len;
            free(current_net_json_part);
            current_net_json_part = NULL;
        }
    }
    strcat(full_json_string, "]}");
    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return full_json_string;
#else
    struct ifaddrs *ifaddr, *ifa;
    char* full_json_string = (char*)malloc(INITIAL_JSON_BUFFER_SIZE);
    size_t current_buffer_size = INITIAL_JSON_BUFFER_SIZE;
    size_t current_len = 0;
    char* current_net_json_part = NULL;
    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for Network JSON buffer.\"}");
    strcpy(full_json_string, "{\"network_adapters\": [");
    current_len = strlen(full_json_string);
    if (getifaddrs(&ifaddr) == -1) {
        free(full_json_string);
        return strdup("{\"error\": \"Failed to get network interface addresses (getifaddrs error).\"}");
    }
    int first_adapter = 1;
    typedef struct {
        char name[IF_NAMESIZE];
        char mac_address[18];
        char ipv4_addresses[MAX_INFO_LEN];
        char ipv6_addresses[MAX_INFO_LEN];
        int processed;
    } NetAdapterInfo;
    NetAdapterInfo adapters[32];
    int num_adapters = 0;
    for (int i = 0; i < 32; i++) {
        adapters[i].processed = 0;
        strcpy(adapters[i].mac_address, "N/A");
        strcpy(adapters[i].ipv4_addresses, "[");
        strcpy(adapters[i].ipv6_addresses, "[");
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        int found_adapter_idx = -1;
        for (int i = 0; i < num_adapters; i++) {
            if (strcmp(adapters[i].name, ifa->ifa_name) == 0) {
                found_adapter_idx = i;
                break;
            }
        }
        if (found_adapter_idx == -1 && num_adapters < 32) {
            found_adapter_idx = num_adapters++;
            strncpy(adapters[found_adapter_idx].name, ifa->ifa_name, IF_NAMESIZE - 1);
            adapters[found_adapter_idx].name[IF_NAMESIZE - 1] = '\0';
        } else if (found_adapter_idx == -1) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_PACKET) {
            struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
            if (s->sll_halen == 6) {
                snprintf(adapters[found_adapter_idx].mac_address, sizeof(adapters[found_adapter_idx].mac_address),
                         "%02x:%02x:%02x:%02x:%02x:%02x",
                         s->sll_addr[0], s->sll_addr[1], s->sll_addr[2],
                         s->sll_addr[3], s->sll_addr[4], s->sll_addr[5]);
            }
        } else if (ifa->ifa_addr->sa_family == AF_INET) {
            char ip_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr, ip_str, sizeof(ip_str));
            if (strcmp(adapters[found_adapter_idx].ipv4_addresses, "[") != 0) {
                strcat(adapters[found_adapter_idx].ipv4_addresses, ",");
            }
            strcat(adapters[found_adapter_idx].ipv4_addresses, "\"");
            strcat(adapters[found_adapter_idx].ipv4_addresses, ip_str);
            strcat(adapters[found_adapter_idx].ipv4_addresses, "\"");
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            char ip_str[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr, ip_str, sizeof(ip_str));
            if (strcmp(adapters[found_adapter_idx].ipv6_addresses, "[") != 0) {
                strcat(adapters[found_adapter_idx].ipv6_addresses, ",");
            }
            strcat(adapters[found_adapter_idx].ipv6_addresses, "\"");
            strcat(adapters[found_adapter_idx].ipv6_addresses, ip_str);
            strcat(adapters[found_adapter_idx].ipv6_addresses, "\"");
        }
    }
    freeifaddrs(ifaddr);
    for (int i = 0; i < num_adapters; i++) {
        strcat(adapters[i].ipv4_addresses, "]");
        strcat(adapters[i].ipv6_addresses, "]");
        current_net_json_part = _create_json_string(
            "%s{\"name\": \"%s\", \"mac_address\": \"%s\", \"ipv4_addresses\": %s, \"ipv6_addresses\": %s}",
            first_adapter ? "" : ",",
            adapters[i].name,
            adapters[i].mac_address,
            adapters[i].ipv4_addresses,
            adapters[i].ipv6_addresses
        );
        first_adapter = 0;
        if (current_net_json_part) {
            size_t part_len = strlen(current_net_json_part);
            if (current_len + part_len + 3 >= current_buffer_size) {
                current_buffer_size += part_len + INITIAL_JSON_BUFFER_SIZE;
                char* temp = (char*)realloc(full_json_string, current_buffer_size);
                if (!temp) {
                    free(full_json_string); free(current_net_json_part);
                    return strdup("{\"error\": \"Memory re-allocation failed for Network JSON buffer.\"}");
                }
                full_json_string = temp;
            }
            strcat(full_json_string, current_net_json_part);
            current_len += part_len;
            free(current_net_json_part);
            current_net_json_part = NULL;
        }
    }
    strcat(full_json_string, "]}");
    return full_json_string;
#endif
}
