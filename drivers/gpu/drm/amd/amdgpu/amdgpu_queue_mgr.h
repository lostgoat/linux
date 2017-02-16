/*
 * Copyright 2017 Valve Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 * 	Andres Rodriguez <andresx7@gmail.com>
 */

#ifndef __AMDGPU_QUEUE_MGR_H__
#define __AMDGPU_QUEUE_MGR_H__

#include "amdgpu.h"

/**
 * amdgpu_queue_mgr_init - init an amdgpu_queue_mgr struct
 *
 * @adev: amdgpu_device pointer
 * @mgr: amdgpu_queue_mgr structure holding queue information
 *
 * Initialize the the selected @mgr (all asics).
 *
 * Returns 0 on success, error on failure.
 */
int amdgpu_queue_mgr_init(struct amdgpu_device *adev,
			  struct amdgpu_queue_mgr *mgr);

/**
 * amdgpu_queue_mgr_fini - de-initialize an amdgpu_queue_mgr struct
 *
 * @adev: amdgpu_device pointer
 * @mgr: amdgpu_queue_mgr structure holding queue information
 *
 * De-initialize the the selected @mgr (all asics).
 *
 * Returns 0 on success, error on failure.
 */
int amdgpu_queue_mgr_fini(struct amdgpu_device *adev,
			  struct amdgpu_queue_mgr *mgr);

/**
 * amdgpu_queue_mgr_map - Map a userspace ring id to an amdgpu_ring
 *
 * @adev: amdgpu_device pointer
 * @mgr: amdgpu_queue_mgr structure holding queue information
 * @hw_ip: HW IP enum
 * @ring: user ring id
 * @our_ring: pointer to mapped amdgpu_ring
 *
 * Map a userspace ring id to an appropriate kernel ring. Different
 * policies are configurable at a HW IP level.
 *
 * Returns 0 on success, error on failure.
 */
int amdgpu_queue_mgr_map(struct amdgpu_device *adev,
			 struct amdgpu_queue_mgr *mgr,
			 int hw_ip, int ring,
			 struct amdgpu_ring **out_ring);
#endif
