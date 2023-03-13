#include "esp_sntp.h"
#include "esp_log.h"

namespace alarm_clock
{
    namespace wifi
    {
        namespace sntp
        {
            static const char *TAG = "sntp";

            class SntpClient
            {

                static void time_sync_notification_cb(struct timeval *tv)
                {
                    ESP_LOGI(TAG, "Notification of a time synchronization event");
                }

            public:
                void Initialize()
                {
                    ESP_LOGI(TAG, "Initializing SNTP");
                    sntp_setoperatingmode(SNTP_OPMODE_POLL);
                    sntp_setservername(0, "pool.ntp.org");
                    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
                    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
                    sntp_init();

                    time_t now = 0;
                    struct tm timeinfo = {0};
                    int retry = 0;
                    const int retry_count = 15;
                    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
                    {
                        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
                        vTaskDelay(2000 / portTICK_PERIOD_MS);
                    }
                    time(&now);
                    localtime_r(&now, &timeinfo);
                }
            };
        }
    }
}