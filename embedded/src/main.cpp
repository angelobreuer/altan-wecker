#include <wifi/WiFiManager.hpp>
#include <wifi/UdpServer.hpp>
#include <wifi/DefaultUdpProcessor.hpp>
#include <wifi/SntpManager.hpp>
#include <media/MediaManager.hpp>

extern "C" void app_main()
{
    auto mediaManager = std::make_unique<alarm_clock::media::MediaManager>();
    auto wifiManager = std::make_unique<alarm_clock::wifi::WifiManager>();
    auto sntpClient = std::make_unique<alarm_clock::wifi::sntp::SntpClient>();
    auto udpProcessor = std::make_unique<alarm_clock::wifi::DefaultUdpProcessor>(mediaManager.get());
    auto udpServer = std::make_unique<alarm_clock::wifi::UdpServer>(udpProcessor.get());

    wifiManager->Connect();
    sntpClient->Initialize();
    udpServer->Start();

    vTaskDelay(portMAX_DELAY);
}