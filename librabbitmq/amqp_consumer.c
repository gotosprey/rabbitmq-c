/* vim:set ft=c ts=2 sw=2 sts=2 et cindent: */
/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Portions created by Alan Antonuk are Copyright (c) 2013
 * Alan Antonuk. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */
#include "amqp_consumer.h"

#include <stdlib.h>
#include <string.h>

void amqp_destroy_envelope(amqp_envelope_t *envelope)
{
  if (envelope) {
    amqp_bytes_free(envelope->body);
    free(envelope);
  }
}

int amqp_consume_message(amqp_connection_state_t state, amqp_envelope_t **envelope, struct timeval *timeout)
{
  int res;
  amqp_frame_t frame;
  amqp_channel_t target_channel;
  uint64_t body_received;
  uint64_t body_target;
  char *body_ptr;

read_deliver:
  res = amqp_simple_wait_frame_noblock(state, &frame, timeout);

  if (AMQP_STATUS_OK != res) {
    goto err_out1;
  }

  if (frame.frame_type != AMQP_FRAME_METHOD ||
      frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD) {
      /* stash? */
    goto read_deliver;
  }

  target_channel = frame.channel;
  *envelope = calloc(sizeof(amqp_envelope_t), 1);
  if (NULL == *envelope) {
    res = AMQP_STATUS_NO_MEMORY;
    goto err_out2;
  }
  (*envelope)->delivery = *(amqp_basic_deliver_t*)frame.payload.method.decoded;

read_header:
  res = amqp_simple_wait_frame_noblock(state, &frame, timeout);

  if (AMQP_STATUS_OK != res) {
    goto err_out2;
  }

  if (target_channel != frame.channel ||
      frame.frame_type != AMQP_FRAME_HEADER) {
    /* Stash ? */
    goto read_header;
  }

  (*envelope)->headers = *(amqp_basic_properties_t*)frame.payload.properties.decoded;
  (*envelope)->body = amqp_bytes_malloc(frame.payload.properties.body_size);
  if (NULL == (*envelope)->body.bytes) {
    res = AMQP_STATUS_NO_MEMORY;
    goto err_out2;
  }

  body_target = frame.payload.properties.body_size;
  body_received = 0;
  body_ptr = (*envelope)->body.bytes;

  while (body_received < body_target) {
read_body:
    res = amqp_simple_wait_frame_noblock(state, &frame, timeout);

    if (AMQP_STATUS_OK) {
      goto err_out2;
    }

    if (target_channel != frame.channel ||
        frame.frame_type != AMQP_FRAME_BODY) {
      /* stash ? */
      goto read_body;
    }

    /* TODO: check len to make sure it doesn't overflow our buffer */

    memcpy(body_ptr, frame.payload.body_fragment.bytes, frame.payload.body_fragment.len);

    body_received += frame.payload.body_fragment.len;
    body_ptr += frame.payload.body_fragment.len;
  }

  return AMQP_STATUS_OK;

err_out2:
  amqp_bytes_free((*envelope)->body);
  free(*envelope);

err_out1:
  return res;
}
