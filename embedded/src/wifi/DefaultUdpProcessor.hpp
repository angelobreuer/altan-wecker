#pragma once

#include <wifi/UdpProcessor.hpp>
#include <media/MediaManager.hpp>

namespace alarm_clock
{
    namespace wifi
    {
        class DefaultUdpProcessor : public UdpProcessor
        {
        public:
            DefaultUdpProcessor(alarm_clock::media::MediaManager *mediaManager)
                : _mediaManager{mediaManager}
            {
            }

            void ProcessMessage(const RequestContext &request, ResponseContext &response) override
            {
                ESP_LOGI("DefaultUdpProcessor", "Processing request (op: %d, length: %d).", (int)request.opCode, (int)request.length);

                if (request.opCode == OpCode::OP_SIMULATE_RINGTONE)
                {
                    if (request.length != 4)
                    {
                        response.operationStatus = OperationStatus::BAD_REQUEST;
                        return;
                    }

                    const uint32_t trackId = *reinterpret_cast<const uint32_t *>(request.buffer);

                    response.operationStatus = OperationStatus::SUCCESS;
                    _mediaManager->Play(trackId);
                }
                else
                {
                    response.operationStatus = OperationStatus::NOT_IMPLEMENTED;
                }
            }

        private:
            alarm_clock::media::MediaManager *_mediaManager;
        };
    }
}