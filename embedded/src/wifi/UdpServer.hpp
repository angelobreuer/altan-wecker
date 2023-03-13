#pragma once

#include <array>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "esp_log.h"

#include <wifi/UdpProcessor.hpp>

namespace alarm_clock
{
    namespace wifi
    {
        static constexpr const std::array<uint8_t, 7> UdpPacketHeader = {{0x88, 0x86, 0x96, 0x77, 0x7F, 0x7F, 0x66}};

        class UdpServer
        {
        public:
            constexpr const static uint16_t PORT = 8999;
            constexpr const static char *TAG = "udp";

            UdpServer(UdpProcessor *processor) : _task{nullptr}, _processor{processor}
            {
            }

            void Start()
            {
                if (_task)
                {
                    return;
                }

                xTaskCreate(udp_server_task, "udp_server", 4096, _processor, 5, &_task);
            }

            void Stop()
            {
                if (!_task)
                {
                    return;
                }

                vTaskDelete(_task);
                _task = nullptr;
            }

        private:
            TaskHandle_t _task;
            UdpProcessor *_processor;

            static void udp_server_task(void *pvParameters)
            {
                std::array<uint8_t, 128> receiveBuffer;
                std::array<uint8_t, 128> sendBuffer;
                SocketHandle socketHandle;
                sockaddr_in destinationAddress{};

                auto processor = static_cast<UdpProcessor *>(pvParameters);

                while (1)
                {
                    destinationAddress.sin_addr.s_addr = htonl(INADDR_ANY);
                    destinationAddress.sin_family = AF_INET;
                    destinationAddress.sin_port = htons(PORT);

                    bind(socketHandle.GetHandle(), (struct sockaddr *)&destinationAddress, sizeof(destinationAddress));

                    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
                    socklen_t socklen = sizeof(source_addr);

                    while (1)
                    {
                        auto bytesReceived = recvfrom(socketHandle.GetHandle(), receiveBuffer.data(), receiveBuffer.size() - 1, 0, (struct sockaddr *)&source_addr, &socklen);

                        if (bytesReceived <= 0)
                        {
                            break;
                        }

                        if (bytesReceived < 12 || strncmp((const char *)UdpPacketHeader.data(), (const char *)receiveBuffer.data(), UdpPacketHeader.size()) != 0)
                        {
                            ESP_LOGE(TAG, "Malformed packed received.");
                            continue;
                        }

                        uint8_t opCode = receiveBuffer.data()[7];
                        uint32_t correlationId = *reinterpret_cast<uint32_t *>(&receiveBuffer.data()[8]);

                        RequestContext request = {
                            .opCode = (OpCode)opCode,
                            .correlationId = correlationId,
                            .buffer = receiveBuffer.data() + 12,
                            .length = (size_t)(bytesReceived - (ssize_t)12),
                        };

                        ResponseContext response = {
                            .buffer = sendBuffer.data() + 12,
                            .availableBytes = sendBuffer.size() - 12,
                            .operationStatus = OperationStatus::SUCCESS,
                            .length = 0,
                        };

                        processor->ProcessMessage(request, response);

                        memcpy(sendBuffer.data(), receiveBuffer.data(), 12);
                        sendBuffer.data()[7] = (uint8_t)response.operationStatus;

                        auto sendResult = sendto(socketHandle.GetHandle(), sendBuffer.data(), response.length + 12, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));

                        if (sendResult < 0)
                        {
                            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                            break;
                        }
                    }
                }

                vTaskDelete(NULL);
            }

        private:
            struct SocketHandle
            {
            public:
                SocketHandle()
                {
                    _handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

                    if (_handle < 0)
                    {
                        ESP_ERROR_CHECK(ESP_ERR_INVALID_STATE);
                    }
                }

                ~SocketHandle()
                {
                    shutdown(_handle, 0);
                    close(_handle);
                }

                int GetHandle() const { return _handle; }

            private:
                int _handle;
            };
        };
    }
}