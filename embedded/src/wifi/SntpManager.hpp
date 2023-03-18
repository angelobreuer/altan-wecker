#include "esp_log.h"
#include "esp_sntp.h"

namespace alarm_clock {
namespace wifi {
namespace sntp {
static const char *TAG = "sntp";

class SntpClient {

    static void time_sync_notification_cb(timeval *currentTime) {
        ESP_LOGI(TAG, "Notification of a time synchronization event: %s",
                 ctime(&currentTime->tv_sec));
    }

  public:
    void Initialize() {
        ESP_LOGI(TAG, "Initializing SNTP");
        setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", true);
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_set_time_sync_notification_cb(time_sync_notification_cb);
        sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
        sntp_init();

        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
            ESP_LOGI(TAG, "Waiting for system time to be set...");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
};
} // namespace sntp
} // namespace wifi
} // namespace alarm_clock