#pragma once

#include <alarm/AlarmManager.hpp>
#include <media/MediaManager.hpp>
#include <wifi/UdpProcessor.hpp>

namespace alarm_clock {
namespace wifi {
class DefaultUdpProcessor : public UdpProcessor {
    constexpr const static char *TAG = "udp";

  public:
    DefaultUdpProcessor(alarm_clock::media::MediaManager *mediaManager,
                        alarm_clock::alarm::AlarmManager *alarmManager)
        : _mediaManager{mediaManager}, _alarmManager{alarmManager} {}

    void ProcessMessage(const RequestContext &request,
                        ResponseContext &response) override {

        if (request.opCode == OpCode::kOpSimulateRingtone) {
            if (request.length != 4) {
                response.operationStatus = OperationStatus::kBadRequest;
                return;
            }

            const uint32_t trackId =
                *reinterpret_cast<const uint32_t *>(request.buffer);

            response.operationStatus = OperationStatus::kSuccess;
            _mediaManager->Play(trackId);
            return;
        }

        if (request.opCode == OpCode::kOpListAlarms) {
            if (request.length != 0) {
                response.operationStatus = OperationStatus::kBadRequest;
                return;
            }

            constexpr auto alarmSize = sizeof(alarm_clock::alarm::Alarm);
            const auto alarms = _alarmManager->GetAlarms();
            auto offset = 0U;

            for (auto index = 0; index < alarms->size(); index++) {
                if (false) {
                    continue;
                }

                if (response.availableBytes < alarmSize + 1) {
                    response.operationStatus =
                        OperationStatus::kResponseBodyTooLarge;
                    return;
                }

                response.buffer[offset++] = (uint8_t)index;
                std::memcpy(response.buffer + offset, &alarms->at(index),
                            alarmSize);
                offset += alarmSize;
            }

            response.length = offset;
            response.operationStatus = OperationStatus::kSuccess;
            return;
        }

        if (request.opCode == OpCode::kUpdateAlarm) {
            constexpr auto alarmSize = sizeof(alarm_clock::alarm::Alarm);

            if (request.length != 1 + alarmSize) {
                response.operationStatus = OperationStatus::kBadRequest;
                return;
            }

            const auto alarmIndex = *request.buffer;
            auto alarms = _alarmManager->GetAlarms();

            if (alarmIndex >= alarms->size()) {
                response.operationStatus = OperationStatus::kBadRequest;
                return;
            }

            auto alarm = &alarms->at(alarmIndex);
            std::memcpy(alarm, request.buffer + 1, alarmSize);
            _alarmManager->Commit();

            response.operationStatus = OperationStatus::kSuccess;
            return;
        }

        response.operationStatus = OperationStatus::kNotImplemented;
    }

  private:
    alarm_clock::media::MediaManager *_mediaManager;
    alarm_clock::alarm::AlarmManager *_alarmManager;
};
} // namespace wifi
} // namespace alarm_clock