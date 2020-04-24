/*
 * Copyright (c) 2012 Jeremy Pepper
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of message_queue nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "message_queue.h"
#include <inttypes.h>
#include <stdlib.h>
//#include <unistd.h>
#include <stdio.h>
//#include <fcntl.h>
#include <errno.h>

//use umm_malloc function
#include "umm_malloc.h"

#include "sysprintf.h"

union padding {
	char chardata;
	short shortdata;
	int intdata;
	long longdata;
	float floatdata;
	double doubledata;
	void *pointerdata;
};

static inline int pad_size(int size) {
	return size % sizeof(union padding) ?
	       (size + (sizeof(union padding) - (size % sizeof(union padding)))) :
		   size;
}

static inline uint32_t round_to_pow2(uint32_t x) {
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}

static inline int max(int x, int y) {
	return x > y ? x : y;
}

int message_queue_init(struct message_queue *queue, int message_size, int max_depth) {
	int i;
	queue->message_size = pad_size(message_size);
	queue->max_depth = round_to_pow2(max_depth);
	queue->memory = umm_malloc(queue->message_size * queue->max_depth);
	if(!queue->memory)
		goto error;
	queue->freelist = umm_malloc(sizeof(void *) * queue->max_depth);
	if(!queue->freelist)
		goto error_after_memory;
	for(i=0;i<queue->max_depth;++i) {
		queue->freelist[i] = (char *)queue->memory + (queue->message_size * i);
		//XPRINTF((6, "queue->freelist = %08x  queue->memory =%08x  	queue->freelist[%d]=%08x\r\n",queue->freelist, queue->memory , i, queue->freelist[i]));
	}
	queue->allocator.blocked_readers = 0;
	queue->allocator.free_blocks = queue->max_depth;
	queue->allocator.allocpos = 0;
	queue->allocator.freepos = 0;
	queue->queue_data = umm_malloc(sizeof(void *) * queue->max_depth);
	if(!queue->queue_data)
		goto error_after_alloc_sem;
	for(i=0;i<queue->max_depth;++i) {
		queue->queue_data[i] = NULL;
	}
	queue->queue.blocked_readers = 0;
	queue->queue.entries = 0;
	queue->queue.readpos = 0;
	queue->queue.writepos = 0;
	return 0;

error_after_queue:
	umm_free(queue->queue_data);
error_after_alloc_sem:
	/*
	if(queue->allocator.sem == &queue->allocator.unnamed_sem) {
		sem_destroy(queue->allocator.sem);
	} else {
		sem_close(queue->allocator.sem);
	}
	*/
error_after_freelist:
	umm_free(queue->freelist);
error_after_memory:
	umm_free(queue->memory);
error:
	return -1;
}

void *message_queue_message_alloc(struct message_queue *queue) {
	if(queue->allocator.free_blocks > 0) {
		unsigned int pos = (queue->allocator.allocpos ) % queue->max_depth;
		void *rv = queue->freelist[pos];
		//void *rv = (char *)queue->memory + (queue->message_size * pos);
		//XPRINTF((6, "rv = %08x  queue->memory =%08x  queue->freelist[%d] =%08x \r\n", rv, queue->memory, pos, queue->freelist[pos]));
		queue->allocator.free_blocks--;
		queue->allocator.allocpos++;

		/*
		while(!rv) {
				//usleep(10); __sync_synchronize();
				rv = queue->freelist[pos];
			}
			queue->freelist[pos] = NULL;
		*/	
		
		return rv;
	}

	//queue->allocator.free_blocks++;
	return NULL;
}

void *message_queue_message_alloc_blocking(struct message_queue *queue) {
	void *rv = message_queue_message_alloc(queue);
	return rv;
}

void message_queue_message_free(struct message_queue *queue, void *message) {
/*
	unsigned int pos = __sync_fetch_and_add(&queue->allocator.freepos, 1) % queue->max_depth;
	void *cur = queue->freelist[pos];
	while(cur) {
		usleep(10); __sync_synchronize();
		cur = queue->freelist[pos];
	}
	queue->freelist[pos] = message;
	__sync_fetch_and_add(&queue->allocator.free_blocks, 1);
	if(queue->allocator.blocked_readers) {
		__sync_fetch_and_add(&queue->allocator.blocked_readers, -1);
		sem_post(queue->allocator.sem);
	}
*/
	unsigned int pos = queue->allocator.freepos % queue->max_depth;
	void *cur = queue->freelist[pos];
	queue->allocator.freepos++;
	/*
	while(cur) {
		//usleep(10); __sync_synchronize();
		cur = queue->freelist[pos];
	}
	*/
	queue->freelist[pos] = message;
	//__sync_fetch_and_add(&queue->allocator.free_blocks, 1);
	queue->allocator.free_blocks++;
	if(queue->allocator.blocked_readers) {
		//__sync_fetch_and_add(&queue->allocator.blocked_readers, -1);
		queue->allocator.blocked_readers--;
		//sem_post(queue->allocator.sem);
	}
}

void message_queue_write(struct message_queue *queue, void *message) {
	/*
	unsigned int pos = __sync_fetch_and_add(&queue->queue.writepos, 1) % queue->max_depth;
	void *cur = queue->queue_data[pos];
	while(cur) {
		usleep(10); __sync_synchronize();
		cur = queue->queue_data[pos];
	}
	queue->queue_data[pos] = message;
	__sync_fetch_and_add(&queue->queue.entries, 1);
	if(queue->queue.blocked_readers) {
		__sync_fetch_and_add(&queue->queue.blocked_readers, -1);
		sem_post(queue->queue.sem);
	}
	*/
	unsigned int pos = queue->queue.writepos % queue->max_depth;
	void *cur = queue->queue_data[pos];
	queue->queue.writepos++;
	/*
	while(cur) {
		//usleep(10); __sync_synchronize();
		cur = queue->queue_data[pos];
	}
	*/
	queue->queue_data[pos] = message;
	// __sync_fetch_and_add(&queue->queue.entries, 1);
	queue->queue.entries++;

	if(queue->queue.blocked_readers) {
		queue->queue.blocked_readers--;
		//sem_post(queue->queue.sem);
	}
}

void *message_queue_tryread(struct message_queue *queue) {
	/*
	if(__sync_fetch_and_add(&queue->queue.entries, -1) > 0) {
		unsigned int pos = __sync_fetch_and_add(&queue->queue.readpos, 1) % queue->max_depth;
		void *rv = queue->queue_data[pos];
		while(!rv) {
			usleep(10); __sync_synchronize();
			rv = queue->queue_data[pos];
		}
		queue->queue_data[pos] = NULL;
		return rv;
	}
	__sync_fetch_and_add(&queue->queue.entries, 1);
	return NULL;
	*/
	if(queue->queue.entries > 0) {
		// unsigned int pos = __sync_fetch_and_add(&queue->queue.readpos, 1) % queue->max_depth;
		unsigned int pos = queue->queue.readpos % queue->max_depth;

		void *rv = queue->queue_data[pos];
		
		queue->queue.entries--;
		queue->queue.readpos++;
		
		/*
		while(!rv) {
			//usleep(10); __sync_synchronize();
			rv = queue->queue_data[pos];
		}
		*/
		queue->queue_data[pos] = NULL;
		return rv;
	}
	// __sync_fetch_and_add(&queue->queue.entries, 1);
	queue->queue.entries++;	
	return NULL;
}

void *message_queue_read(struct message_queue *queue) {

	void *rv = message_queue_tryread(queue);

	return rv;
}

void message_queue_destroy(struct message_queue *queue) {

	umm_free(queue->queue_data);
	umm_free(queue->freelist);
	umm_free(queue->memory);
	
}
