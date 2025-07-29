//
// Created by Pi on 2025/06/11.
//
#include "app_config.h"
#include "artc_device_helper.h"
#include "artc_logger.h"

#include "wifi/wifi_connect.h"
#include "lwip.h"
#include "syscfg/syscfg_id.h"
#include "event/net_event.h"
#include "os/os_error.h"


// 设置WiFi的ssid及pwd，用于链接网络
#define WIFI_STA_SSID "ssid"
#define WIFI_STA_PWD  "pwd"

extern int storage_device_ready(void);

static const int TickInterval = 10; // 10ms一个tick: https://doc.zh-jieli.com/AC79/zh-cn/master/module_example/system/os_api.html#_CPPv411os_time_dlyi
bool ARTCWaitSDCardReady(int timeoutInSeconds)
{
    int tickCount = timeoutInSeconds * 1000 / TickInterval;
    while (!storage_device_ready() && tickCount-- > 0) {
        os_time_dly(1);
    }
    return storage_device_ready();
}

static void __PrintCurrentWifiInfo()
{
    struct wifi_mode_info info;
    memset(&info, 0, sizeof(struct wifi_mode_info));
    info.mode = STA_MODE;
    wifi_get_mode_cur_info(&info);
    ARTC_LOG_INFO("Current Wifi Info: mode(%d), ssid(%s), pwd(%s)", info.mode, info.ssid ? info.ssid : "", info.pwd ? info.pwd : "");
}

static void __PrintCurrentNetInfo()
{
    struct netif_info netInfo;
    memset(&netInfo, 0, sizeof(struct netif_info));
    lwip_get_netif_info(1, &netInfo);
    ARTC_LOG_INFO("Current Net Info, ip(%x): %d.%d.%d.%d", netInfo.ip, (netInfo.ip >> 0) & 0xFF, (netInfo.ip >> 8) & 0xFF, (netInfo.ip >> 16) & 0xFF, (netInfo.ip >> 24) & 0xFF);
    ARTC_LOG_INFO("Current Net Info, gw(%x): %d.%d.%d.%d", netInfo.gw, (netInfo.gw >> 0) & 0xFF, (netInfo.gw >> 8) & 0xFF, (netInfo.gw >> 16) & 0xFF, (netInfo.gw >> 24) & 0xFF);
    ARTC_LOG_INFO("Current Net Info, netmask(%x): %d.%d.%d.%d", netInfo.netmask, (netInfo.netmask >> 0) & 0xFF, (netInfo.netmask >> 8) & 0xFF, (netInfo.netmask >> 16) & 0xFF, (netInfo.netmask >> 24) & 0xFF);
}

struct sta_ip_info {
    u8 ssid[33];
    u32 ip;
    u32 gw;
    u32 netmask;
    u32 dns;
    u8 gw_mac[6];
    u8 local_mac[6];
    u8 chanel;
};
static void __SetupLanInfo(void)
{
    struct sta_ip_info staIPInfo;
    memset(&staIPInfo, 0, sizeof(struct sta_ip_info));
    syscfg_read(VM_STA_IPADDR_INDEX, (char *)&staIPInfo, sizeof(struct sta_ip_info));

    struct lan_setting lanSetting = {
        .WIRELESS_IP_ADDR0  = (u8)(staIPInfo.ip >> 0),
        .WIRELESS_IP_ADDR1  = (u8)(staIPInfo.ip >> 8),
        .WIRELESS_IP_ADDR2  = (u8)(staIPInfo.ip >> 16),
        .WIRELESS_IP_ADDR3  = (u8)(staIPInfo.ip >> 24),

        .WIRELESS_NETMASK0  = (u8)(staIPInfo.netmask >> 0),
        .WIRELESS_NETMASK1  = (u8)(staIPInfo.netmask >> 8),
        .WIRELESS_NETMASK2  = (u8)(staIPInfo.netmask >> 16),
        .WIRELESS_NETMASK3  = (u8)(staIPInfo.netmask >> 24),

        .WIRELESS_GATEWAY0   = (u8)(staIPInfo.gw >> 0),
        .WIRELESS_GATEWAY1   = (u8)(staIPInfo.gw >> 8),
        .WIRELESS_GATEWAY2   = (u8)(staIPInfo.gw >> 16),
        .WIRELESS_GATEWAY3   = (u8)(staIPInfo.gw >> 24),
    };
    net_set_lan_info(&lanSetting);
}

static int __OnWifiEvent(void *networkCtx, enum WIFI_EVENT event)
{
    switch (event)
    {
        case WIFI_EVENT_MODULE_INIT:
            ARTC_LOG_INFO("WIFI_EVENT_MODULE_INIT");
            break;
        case WIFI_EVENT_MODULE_START:
            ARTC_LOG_INFO("WIFI_EVENT_MODULE_START");
            break;
        case WIFI_EVENT_MODULE_STOP:
            ARTC_LOG_INFO("WIFI_EVENT_MODULE_STOP");
            break;
        case WIFI_EVENT_STA_START:
            ARTC_LOG_INFO("WIFI_EVENT_STA_START");
            break;
        case WIFI_EVENT_STA_STOP:
            ARTC_LOG_INFO("WIFI_EVENT_STA_STOP");
            break;
        case WIFI_EVENT_STA_DISCONNECT:
            ARTC_LOG_INFO("WIFI_EVENT_STA_DISCONNECT");
            break;
        case WIFI_EVENT_STA_SCANNED_SSID:
            ARTC_LOG_INFO("WIFI_EVENT_STA_SCANNED_SSID");
            break;
        case WIFI_EVENT_STA_SCAN_COMPLETED:
            ARTC_LOG_INFO("WIFI_EVENT_STA_SCAN_COMPLETED");
            break;
        case WIFI_EVENT_STA_CONNECT_SUCC:
            ARTC_LOG_INFO("WIFI_EVENT_STA_CONNECT_SUCC");
            __SetupLanInfo();
            __PrintCurrentWifiInfo();
            break;
        case WIFI_EVENT_STA_NETWORK_STACK_DHCP_SUCC:
            ARTC_LOG_INFO("WIFI_EVENT_STA_NETWORK_STACK_DHCP_SUCC");
            //有些使用了加密的路由器刚获取IP地址后前几个包都会没响应，怀疑路由器没转发成功
            void connect_broadcast(void);
            connect_broadcast();
            __PrintCurrentNetInfo();
            break;
        case WIFI_EVENT_STA_CONNECT_TIMEOUT_NOT_FOUND_SSID:
            ARTC_LOG_INFO("WIFI_EVENT_STA_CONNECT_TIMEOUT_NOT_FOUND_SSID");
            break;
        case WIFI_EVENT_STA_CONNECT_ASSOCIAT_FAIL:
            ARTC_LOG_INFO("WIFI_EVENT_STA_CONNECT_ASSOCIAT_FAIL");
            break;
        case WIFI_EVENT_STA_NETWORK_STACK_DHCP_TIMEOUT:
            ARTC_LOG_INFO("WIFI_EVENT_STA_NETWORK_STACK_DHCP_TIMEOUT");
            break;
    }
    return 0;
}

void ARTCSetupWifi(void)
{
    wifi_set_event_callback(__OnWifiEvent);
    wifi_on();
    wifi_set_sta_connect_timeout(30);
    wifi_enter_sta_mode(WIFI_STA_SSID, WIFI_STA_PWD);
}

bool ARTCIsWifiConnected()
{
    return wifi_get_sta_connect_state() == WIFI_STA_NETWORK_STACK_DHCP_SUCC;
}

bool ARTCWaitWifiConnected(int timeoutInSeconds)
{
    int tickCount = timeoutInSeconds * 1000 / TickInterval;
    while (!ARTCIsWifiConnected() && tickCount-- > 0) {
        os_time_dly(1);
    }
    return ARTCIsWifiConnected();
}
