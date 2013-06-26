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
#ifndef librabbitmq_amqp_consumer_h
#define librabbitmq_amqp_consumer_h

#include "amqp.h"

AMQP_BEGIN_DECLS

typedef struct amqp_envelope_t_ {
    amqp_channel_t channel;
    amqp_basic_deliver_t delivery;
    amqp_basic_properties_t headers;
    amqp_bytes_t body;
} amqp_envelope_t;

AMQP_PUBLIC_FUNCTION
void AMQP_CALL amqp_destroy_envelope(amqp_envelope_t *envelope);

AMQP_PUBLIC_FUNCTION
int AMQP_CALL amqp_consume_message(amqp_connection_state_t state, amqp_envelope_t **envelope, struct timeval *timeout);

AMQP_END_DECLS

#endif /* librabbitmq_amqp_consumer_h */
