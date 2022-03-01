// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <utils_gapi/stream_source.hpp>
#include <opencv2/gapi/streaming/meta.hpp>

namespace custom {
CommonCapSrc::CommonCapSrc(std::shared_ptr<ImagesCapture>& imagesCapture)
    : cap(imagesCapture) {
    preparation();
}

void CommonCapSrc::preparation() {
    GAPI_Assert(first.empty());
    cv::Mat tmp = cap->read();
    if (!tmp.data) {
        GAPI_Assert(false && "Couldn't grab the first frame");
    }
    first = tmp.clone();
}

bool CommonCapSrc::pull(cv::gapi::wip::Data &data) {
    using namespace std::chrono;
    if (!first_pulled) {
        GAPI_Assert(!first.empty());
        first_pulled = true;
        data = first;
        auto now = high_resolution_clock::now();
        auto end_ts = duration_cast<milliseconds>(now.time_since_epoch()).count();
        data.meta[cv::gapi::streaming::meta_tag::timestamp] = end_ts;
        return true;
    }
    cv::Mat frame = cap->read();
    if (!frame.data) {
        return false;
    }
    data = frame.clone();
    auto now = high_resolution_clock::now();
    auto end_ts = duration_cast<milliseconds>(now.time_since_epoch()).count();
    data.meta[cv::gapi::streaming::meta_tag::timestamp] = end_ts;

    return true;
}

cv::GMetaArg CommonCapSrc::descr_of() const {
    GAPI_Assert(!first.empty());
    return cv::GMetaArg{ cv::descr_of(first) };
}
} // namespace custom
