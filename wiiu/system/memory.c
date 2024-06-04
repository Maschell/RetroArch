/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include <malloc.h>
#include <string.h>
#include "memory.h"
#include <coreinit/memheap.h>
#include <coreinit/memexpheap.h>
#include <coreinit/memfrmheap.h>
#include <proc_ui/procui.h>

static MEMHeapHandle mem1_heap;
static MEMHeapHandle bucket_heap;

#define GFX_FRAME_HEAP_TAG (0x123DECAF)

bool memoryInitializeMEM1() {
   MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
   uint32_t size;
   void *base;

   if (!MEMRecordStateForFrmHeap(heap, GFX_FRAME_HEAP_TAG)) {
      return false;
   }

   size = MEMGetAllocatableSizeForFrmHeapEx(heap, 4);
   if (!size) {
      return false;
   }

   base = MEMAllocFromFrmHeapEx(heap, size, 4);
   if (!base) {
      return false;
   }

   mem1_heap = MEMCreateExpHeapEx(base, size, 0);
   if (!mem1_heap) {
      return false;
   }
   return true;
}

bool memoryInitializeBucket() {
   MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
   uint32_t size;
   void *base;

   size = MEMGetAllocatableSizeForFrmHeapEx(heap, 4);
   if (!size) {
      return false;
   }

   base = MEMAllocFromFrmHeapEx(heap, size, 4);
   if (!base) {
      return false;
   }

   bucket_heap = MEMCreateExpHeapEx(base, size, 0);
   if (!bucket_heap) {
      return false;
   }
   return true;
}


void destroyMEM1Heap() {
   MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

   if (mem1_heap) {
      MEMDestroyExpHeap(mem1_heap);
      mem1_heap = NULL;
   }

   MEMFreeByStateToFrmHeap(heap, GFX_FRAME_HEAP_TAG);
}

void destroyBucketHeap() {
   MEMHeapHandle foreground = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);

   if (bucket_heap) {
      MEMDestroyExpHeap(bucket_heap);
      bucket_heap = NULL;
   }

   MEMFreeToFrmHeap(foreground, MEM_FRM_HEAP_FREE_ALL);
}

/* some wrappers */

void *MEM2_alloc(unsigned int size, unsigned int align) {
   return memalign(align, size);
}

void MEM2_free(void *ptr) {
   free(ptr);
}

void *MEM1_alloc(unsigned int size, unsigned int align) {
   if (align < 4)
      align = 4;
   return MEMAllocFromExpHeapEx(mem1_heap, size, align);
}

void MEM1_free(void *ptr) {
   if (ptr)
      MEMFreeToExpHeap(mem1_heap, ptr);
}

void *MEMBucket_alloc(unsigned int size, unsigned int align) {
   if (align < 4)
      align = 4;
   return MEMAllocFromExpHeapEx(bucket_heap, size, align);
}

void MEMBucket_free(void *ptr) {
   if (ptr)
      MEMFreeToExpHeap(bucket_heap, ptr);
}
