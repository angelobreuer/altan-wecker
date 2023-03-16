#include <alarm/AlarmManager.hpp>
#include <media/DefaultMediaListener.hpp>
#include <media/MediaManager.hpp>
#include <wifi/DefaultUdpProcessor.hpp>
#include <wifi/SntpManager.hpp>
#include <wifi/UdpServer.hpp>
#include <wifi/WiFiManager.hpp>

extern "C" void app_main() {
    auto mediaListener =
        std::make_unique<alarm_clock::media::DefaultMediaListener>();

    auto mediaManager =
        std::make_unique<alarm_clock::media::MediaManager>(mediaListener.get());

    auto wifiManager = std::make_unique<alarm_clock::wifi::WifiManager>();
    auto alarmManager = std::make_unique<alarm_clock::alarm::AlarmManager>();
    auto sntpClient = std::make_unique<alarm_clock::wifi::sntp::SntpClient>();

    auto udpProcessor =
        std::make_unique<alarm_clock::wifi::DefaultUdpProcessor>(
            mediaManager.get(), alarmManager.get());

    auto udpServer =
        std::make_unique<alarm_clock::wifi::UdpServer>(udpProcessor.get());

    wifiManager->Connect();
    sntpClient->Initialize();
    udpServer->Start();
    alarmManager->Load();

    vTaskDelay(portMAX_DELAY);
}