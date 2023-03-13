#pragma once

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <freertos/event_groups.h>
#include <cstring>
#include "lwip/apps/sntp.h"
#include "WiFiConfig.hpp"

namespace alarm_clock
{
    namespace wifi
    {
        static const char *TAG = "wifi";

#define GOT_IPV4_BIT BIT(0)
#define CONNECTED_BITS (GOT_IPV4_BIT)

        class WifiManager
        {
        public:
            void Connect()
            {
                ESP_ERROR_CHECK(nvs_flash_init());
                ESP_ERROR_CHECK(esp_netif_init());
                ESP_ERROR_CHECK(esp_event_loop_create_default());

                if (s_connect_event_group != NULL)
                {
                    ESP_ERROR_CHECK(ESP_ERR_INVALID_STATE);
                }

                s_connect_event_group = xEventGroupCreate();
                Start(WIFI_SSID, WIFI_PASSPHRASE);

                ESP_LOGI(TAG, "Waiting for IP");

                xEventGroupWaitBits(s_connect_event_group, CONNECTED_BITS, true, true, portMAX_DELAY);

                ESP_LOGI(TAG, "Connected to %s", s_connection_name);
                ESP_LOGI(TAG, "IPv4 address: " IPSTR, IP2STR(&s_ip_addr));
            }

            void Start(const char *ssid, const char *passphrase)
            {
                wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
                ESP_ERROR_CHECK(esp_wifi_init(&cfg));

                esp_netif_config_t netif_config = ESP_NETIF_DEFAULT_WIFI_STA();
                esp_netif_t *netif = esp_netif_new(&netif_config);

                esp_netif_attach_wifi_station(netif);
                esp_wifi_set_default_wifi_sta_handlers();

                s_example_esp_netif = netif;

                ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &DoHandleDisconnect, this));
                ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &DoHandleStationGotIp, this));
                ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

                wifi_config_t wifi_config;

                memset(&wifi_config, 0, sizeof(wifi_config));
                if (ssid)
                {
                    strncpy((char *)wifi_config.sta.ssid, ssid, strlen(ssid));
                }

                if (passphrase)
                {
                    strncpy((char *)wifi_config.sta.password, passphrase, strlen(passphrase));
                }

                ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
                ESP_ERROR_CHECK(esp_wifi_start());
                ESP_ERROR_CHECK(esp_wifi_connect());

                s_connection_name = ssid;
            }

            void Disconnect()
            {
                if (s_connect_event_group == NULL)
                {
                    ESP_ERROR_CHECK(ESP_ERR_INVALID_STATE);
                }

                vEventGroupDelete(s_connect_event_group);
                s_connect_event_group = NULL;

                Stop();

                ESP_LOGI(TAG, "Disconnected from %s", s_connection_name);
                s_connection_name = NULL;
            }

            void Stop()
            {
                ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &DoHandleDisconnect));
                ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &DoHandleStationGotIp));

                esp_err_t err = esp_wifi_stop();

                if (err == ESP_ERR_WIFI_NOT_INIT)
                {
                    return;
                }

                ESP_ERROR_CHECK(err);
                ESP_ERROR_CHECK(esp_wifi_deinit());
                ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(s_example_esp_netif));
                esp_netif_destroy(s_example_esp_netif);
                s_example_esp_netif = NULL;
            }

        private:
            EventGroupHandle_t s_connect_event_group;
            esp_ip4_addr_t s_ip_addr;
            const char *s_connection_name;
            esp_netif_t *s_example_esp_netif = NULL;

            void HandleDisconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
            {
                ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
                esp_err_t err = esp_wifi_connect();
                if (err == ESP_ERR_WIFI_NOT_STARTED)
                {
                    return;
                }
                ESP_ERROR_CHECK(err);
            }

            static void DoHandleDisconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
            {
                ((WifiManager *)arg)->HandleDisconnect(arg, event_base, event_id, event_data);
            }

            void HandleStationGotIp(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
            {
                ESP_LOGI(TAG, "Got IP event!");
                ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
                memcpy(&s_ip_addr, &event->ip_info.ip, sizeof(s_ip_addr));
                xEventGroupSetBits(s_connect_event_group, GOT_IPV4_BIT);
            }

            static void DoHandleStationGotIp(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
            {
                ((WifiManager *)arg)->HandleStationGotIp(arg, event_base, event_id, event_data);
            }
        };
    }
}