#ifndef FLUTTER_WEBRTC_AUDIO_PROCESSING_BRIDGE_HXX
#define FLUTTER_WEBRTC_AUDIO_PROCESSING_BRIDGE_HXX

#include <mutex>
#include <vector>

#include "rtc_audio_processing.h"

namespace flutter_webrtc_plugin {

using namespace libwebrtc;

// AudioProcessingAdapter is a singleton chain of RTCAudioProcessing::CustomProcessing
// implementors. It is wired to SetCapturePostProcessing() in FlutterWebRTCBase and
// delegates each audio frame to every registered processor in order.
//
// External plugins (e.g. a noise gate plugin) register their processors via the
// exported C API in flutter_webrtc_audio_processing.h. Add/Remove are safe to call
// from any thread; Process() runs on the real-time audio capture thread and takes a
// shared read-lock that has effectively zero contention after startup.
class AudioProcessingAdapter : public RTCAudioProcessing::CustomProcessing {
 public:
  static AudioProcessingAdapter& Instance();

  // Register a processor to be called during each captured audio frame.
  // If Initialize() has already been called, the processor's Initialize() is
  // invoked immediately so it can set up its internal state.
  void AddProcessor(RTCAudioProcessing::CustomProcessing* processor);

  // Unregister a previously added processor. Safe to call during plugin teardown.
  void RemoveProcessor(RTCAudioProcessing::CustomProcessing* processor);

  // RTCAudioProcessing::CustomProcessing interface — called by WebRTC internals.
  void Initialize(int sample_rate_hz, int num_channels) override;
  void Process(int num_bands, int num_frames, int buffer_size,
               float* buffer) override;
  void Reset(int new_rate) override;
  void Release() override;

 private:
  AudioProcessingAdapter() = default;

  mutable std::mutex mutex_;
  std::vector<RTCAudioProcessing::CustomProcessing*> processors_;

  // Cached so late-registered processors can be initialized immediately.
  int sample_rate_hz_ = 0;
  int num_channels_ = 0;
  bool initialized_ = false;
};

}  // namespace flutter_webrtc_plugin

#endif  // FLUTTER_WEBRTC_AUDIO_PROCESSING_BRIDGE_HXX
