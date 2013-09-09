/* This file is part of Into. 
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#ifdef _MSC_VER

#define AVCODEC_DECODE_VIDEO avcodec_decode_video_msvc_hack
#define AV_READ_FRAME av_read_frame_msvc_hack
#define IMGCONVERT imgconvert_msvc_hack

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

int avcodec_decode_video_msvc_hack(AVCodecContext* c,
                                   AVFrame *frame,
                                   int *frameFinished,
                                   uint8_t *buf,
                                   int bufSize);

int av_read_frame_msvc_hack(AVFormatContext *s,
                            AVPacket *pkt);


int imgconvert_msvc_hack(AVPicture *dst,
                         int dst_pix_fmt,
                         const AVPicture *src,
                         int src_pix_fmt,
                         int src_width,
                         int src_height);

#else

#define AVCODEC_DECODE_VIDEO avcodec_decode_video
#define AV_READ_FRAME av_read_frame
#define IMGCONVERT imgconvert

#endif
