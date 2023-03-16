#pragma once

namespace alarm_clock {

namespace media {

class MediaListener {
  public:
    virtual void OnTrackPlayStart() = 0;
    virtual void OnTrackPlayEnd() = 0;
};

} // namespace media

} // namespace alarm_clock
