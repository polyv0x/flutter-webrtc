#ifndef FLUTTER_WEBRTC_AUDIO_PROCESSING_HXX
#define FLUTTER_WEBRTC_AUDIO_PROCESSING_HXX

// Public API for registering RTCAudioProcessing::CustomProcessing
// implementations with flutter_webrtc's AudioProcessingAdapter.
//
// External Windows plugins implement RTCAudioProcessing::CustomProcessing and
// call flutter_webrtc_add_audio_processor() during their plugin init to inject
// in-band audio processing on the WebRTC capture path.
//
// Requirements:
//   - The processor pointer must remain valid until
//     flutter_webrtc_remove_audio_processor() is called or the process exits.
//   - Process() must be real-time safe: no heap allocation, no blocking.

#include "rtc_audio_processing.h"

#ifndef FLUTTER_PLUGIN_EXPORT
#ifdef FLUTTER_PLUGIN_IMPL
#define FLUTTER_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FLUTTER_PLUGIN_EXPORT __declspec(dllimport)
#endif
#endif  // FLUTTER_PLUGIN_EXPORT

#if defined(__cplusplus)
extern "C" {
#endif

// Register a processor to be called on every captured audio frame.
// May be called before or after flutter_webrtc initializes; the adapter
// will replay Initialize() params if already started.
FLUTTER_PLUGIN_EXPORT void flutter_webrtc_add_audio_processor(
    libwebrtc::RTCAudioProcessing::CustomProcessing* processor);

// Unregister a previously registered processor.
FLUTTER_PLUGIN_EXPORT void flutter_webrtc_remove_audio_processor(
    libwebrtc::RTCAudioProcessing::CustomProcessing* processor);

#if defined(__cplusplus)
}  // extern "C"
#endif

#endif  // FLUTTER_WEBRTC_AUDIO_PROCESSING_HXX
