
// Copyright 2019 Google Inc
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of Google Inc. nor the names of its contributors may be
//    used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <string.h>
#include <media_format.h>
#include <android/log.h>

#define MODULE_NAME "MEDIA_FORMAT"
#define LOGV(...) \
  __android_log_print(ANDROID_LOG_VERBOSE, MODULE_NAME, __VA_ARGS__)
#define LOGD(...) \
  __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGI(...) \
  __android_log_print(ANDROID_LOG_INFO, MODULE_NAME, __VA_ARGS__)
#define LOGW(...) \
  __android_log_print(ANDROID_LOG_WARN, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) \
  __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)
#define LOGF(...) \
  __android_log_print(ANDROID_LOG_FATAL, MODULE_NAME, __VA_ARGS__)

/*
 * MEDIA_FORMAT: format: mime: string(video/avc), durationUs: int64(32337133), frame-count: int32(1172), track-id: int32(2)
 * language: string(```), width: int32(1920), height: int32(1080), display-width: int32(1920), display-height: int32(1080),
 * rotation-degrees: int32(90), color-range: int32(1), color-standard: int32(2), color-transfer: int32(3),
 * max-input-size: int32(478468), frame-rate: int32(36), profile: int32(8), level: int32(32768), csd-0: data, csd-1: data}
 */
bool print_media_format(AMediaFormat* fmt) {
    const char *format_string  = NULL;
    if (fmt == NULL) {
        return false;
    }

    format_string = AMediaFormat_toString(fmt);
    LOGD("format: %s", format_string);

    return true;
}

#define KEY_SEPARATOR ','
#define KEY_DELIMITER ':'
#define KEY_SPACE ' '
#define KEY_VALUE_START_TAG '('
#define KEY_VALUE_STOP_TAG ')'

#define MAX_KEY_LEN  64

static bool get_value(const char *key_desc, char *type, char *value) {
    if (!key_desc || !type) return false;

    const char *key_copy = key_desc;

    // skip the key names  including ':'
    while (1) {
        if(*key_copy  == KEY_DELIMITER) {
            key_copy++;
            break;
        }
        if (*key_copy == '\0') return false;

        if(*key_copy == KEY_SEPARATOR) return false;

        ++key_copy;
    }

    // skip the spaces
    while(*key_copy == KEY_SPACE) key_copy++;

    const char *type_start = key_copy;
    while (*key_copy != '\0' && *key_copy != KEY_SEPARATOR && *key_copy != KEY_VALUE_START_TAG) {
        ++key_copy;
    }

    int32_t type_len =  key_copy - type_start;
    if (type_len > MAX_KEY_LEN) return false;
    strncpy(type, type_start, type_len);
    type[type_len - 1] = '\0';

    if (*key_copy == KEY_VALUE_START_TAG) {

        if (!value) return false;
        *value = '\0';

        // TODO: the string type to be handled correctly
        int32_t count = 0;
        ++key_copy;
        while (*key_copy != '\0' && *key_copy != KEY_VALUE_STOP_TAG && *key_copy != KEY_SEPARATOR && count < (MAX_KEY_LEN - 1)) {
            count++;
            *value++ = *key_copy++;
        }
        *value = '\0';
    }
    return true;
}


bool media_format_contains_key(AMediaFormat* fmt, const char * key) {
    const char* format_string = NULL;
    const char* key_substr = NULL;
    if (!fmt || !key) return false;

    format_string = AMediaFormat_toString(fmt);
    if (!fmt)  return false;

    key_substr = strstr(format_string, key);
    if (!key_substr)  return false;

    // we have key substr with format of (frame-rate: int32(36) )
    char value_type[MAX_KEY_LEN];
    char value_str[MAX_KEY_LEN];

    if (get_value(key_substr, value_type, value_str)) {
        LOGD("%s: %s(%s)", key, value_type, value_str);
    }

    return true;

}
