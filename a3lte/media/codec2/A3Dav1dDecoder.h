// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <dav1d/dav1d.h>
#include <log/log.h>
#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <deque>

enum A3Dav1dStatus { kA3Dav1dStatusOk, kA3Dav1dStatusNothingToDequeue, kA3Dav1dStatusError };
namespace a3dav1d {
enum ImageFormat { kImageFormatYuv420, kImageFormatMonochrome400, kImageFormatUnsupported };
struct DecoderSettings { int threads = 4; };
struct DecoderBuffer {
    int displayed_width[3]{}, displayed_height[3]{};
    ptrdiff_t stride[3]{};
    uint8_t* plane[3]{};
    int bitdepth = 8;
    ImageFormat image_format = kImageFormatUnsupported;
    int64_t user_private_data = 0;
};
class Decoder {
    Dav1dContext* context_ = nullptr;
    Dav1dPicture current_{};
    std::deque<Dav1dPicture> ready_;
    DecoderBuffer buffer_{};
public:
    ~Decoder() { Flush(); if (context_) dav1d_close(&context_); }
    A3Dav1dStatus Init(const DecoderSettings* settings) {
        Dav1dSettings config;
        dav1d_default_settings(&config);
        config.n_threads = std::max(1, std::min(settings->threads, 4));
        config.max_frame_delay = 2;
        config.frame_size_limit = 4096 * 4096;
        config.all_layers = 0;
        return dav1d_open(&context_, &config) == 0 ? kA3Dav1dStatusOk : kA3Dav1dStatusError;
    }
    A3Dav1dStatus EnqueueFrame(const uint8_t* input, size_t size, int64_t id, void*) {
        Dav1dData data{};
        uint8_t* storage = dav1d_data_create(&data, size);
        if (!storage) return kA3Dav1dStatusError;
        memcpy(storage, input, size);
        data.m.timestamp = id;
        while (data.sz) {
            int result = dav1d_send_data(context_, &data);
            if (result == DAV1D_ERR(EAGAIN)) {
                Dav1dPicture picture{};
                result = dav1d_get_picture(context_, &picture);
                // Invisible AV1 frames can advance the decoder without producing a picture.
                // Retry the retained packet after EAGAIN, just as the dav1d API loop requires.
                if (result == DAV1D_ERR(EAGAIN)) continue;
                if (result == 0) { if (ready_.size() >= 32) { dav1d_picture_unref(&picture); dav1d_data_unref(&data); return kA3Dav1dStatusError; } ready_.push_back(picture); continue; }
            }
            if (result < 0) { ALOGE("dav1d input error %d, size=%zu frame=%lld", result, data.sz, (long long)id); dav1d_data_unref(&data); return kA3Dav1dStatusError; }
        }
        dav1d_data_unref(&data);
        return kA3Dav1dStatusOk;
    }
    A3Dav1dStatus DequeueFrame(const DecoderBuffer** output) {
        *output = nullptr;
        dav1d_picture_unref(&current_);
        if (!ready_.empty()) { current_ = ready_.front(); ready_.pop_front(); }
        else {
            int result = dav1d_get_picture(context_, &current_);
            if (result == DAV1D_ERR(EAGAIN)) return kA3Dav1dStatusNothingToDequeue;
            if (result < 0) { ALOGE("dav1d output error %d", result); return kA3Dav1dStatusError; }
        }
        if (current_.p.bpc != 8 && current_.p.bpc != 10) {
            dav1d_picture_unref(&current_);
            return kA3Dav1dStatusError;
        }
        buffer_.bitdepth = current_.p.bpc;
        buffer_.image_format = current_.p.layout == DAV1D_PIXEL_LAYOUT_I420 ? kImageFormatYuv420
                : current_.p.layout == DAV1D_PIXEL_LAYOUT_I400 ? kImageFormatMonochrome400
                : kImageFormatUnsupported;
        buffer_.user_private_data = current_.m.timestamp;
        for (int p = 0; p < 3; ++p) {
            buffer_.plane[p] = static_cast<uint8_t*>(current_.data[p]);
            buffer_.stride[p] = current_.stride[p ? 1 : 0];
            buffer_.displayed_width[p] = p ? (current_.p.w + 1) / 2 : current_.p.w;
            buffer_.displayed_height[p] = p ? (current_.p.h + 1) / 2 : current_.p.h;
        }
        *output = &buffer_;
        return kA3Dav1dStatusOk;
    }
    A3Dav1dStatus SignalEOS() { return kA3Dav1dStatusOk; }
    void Flush() {
        dav1d_picture_unref(&current_);
        for (auto& frame : ready_) dav1d_picture_unref(&frame);
        ready_.clear();
        if (context_) dav1d_flush(context_);
    }
};
} // namespace a3dav1d
