#include "audio_processing_bridge.h"

#include <algorithm>

namespace flutter_webrtc_plugin {

AudioProcessingAdapter& AudioProcessingAdapter::Instance() {
  static AudioProcessingAdapter instance;
  return instance;
}

void AudioProcessingAdapter::AddProcessor(
    RTCAudioProcessing::CustomProcessing* processor) {
  std::lock_guard<std::mutex> lock(mutex_);
  processors_.push_back(processor);
  if (initialized_) {
    processor->Initialize(sample_rate_hz_, num_channels_);
  }
}

void AudioProcessingAdapter::RemoveProcessor(
    RTCAudioProcessing::CustomProcessing* processor) {
  std::lock_guard<std::mutex> lock(mutex_);
  processors_.erase(std::remove(processors_.begin(), processors_.end(), processor),
                    processors_.end());
}

void AudioProcessingAdapter::Initialize(int sample_rate_hz, int num_channels) {
  std::lock_guard<std::mutex> lock(mutex_);
  sample_rate_hz_ = sample_rate_hz;
  num_channels_ = num_channels;
  initialized_ = true;
  for (auto* p : processors_) {
    p->Initialize(sample_rate_hz, num_channels);
  }
}

void AudioProcessingAdapter::Process(int num_bands, int num_frames,
                                     int buffer_size, float* buffer) {
  std::lock_guard<std::mutex> lock(mutex_);
  for (auto* p : processors_) {
    p->Process(num_bands, num_frames, buffer_size, buffer);
  }
}

void AudioProcessingAdapter::Reset(int new_rate) {
  std::lock_guard<std::mutex> lock(mutex_);
  sample_rate_hz_ = new_rate;
  for (auto* p : processors_) {
    p->Reset(new_rate);
  }
}

void AudioProcessingAdapter::Release() {
  std::lock_guard<std::mutex> lock(mutex_);
  for (auto* p : processors_) {
    p->Release();
  }
  processors_.clear();
  initialized_ = false;
}

}  // namespace flutter_webrtc_plugin
