#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace alarm_clock {
namespace wifi {
enum class OpCode {
    kOpSimulateRingtone,
    kOpListAlarms,
    kUpdateAlarm,
    kPing,
};

enum class OperationStatus {
    kSuccess,
    kNotImplemented,
    kBadRequest,
    kRequestBodyTooLarge,
    kResponseBodyTooLarge,
};

struct RequestContext {
    OpCode const opCode;
    uint32_t const correlationId;
    const uint8_t *const buffer;
    size_t const length;
};

struct ResponseContext {
    uint8_t *const buffer;
    size_t const availableBytes;
    OperationStatus operationStatus;
    size_t length;
};

class UdpProcessor {
  public:
    virtual void ProcessMessage(const RequestContext &request,
                                ResponseContext &response) = 0;
};
} // namespace wifi
} // namespace alarm_clock