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
 * Authors: Andres Rodriguez
 */

#include "amdgpu_ring.h"
#include "amdgpu_queue_mgr.h"

static int amdgpu_queue_mapper_init(struct amdgpu_queue_mapper *mapper,
				    int hw_ip)
{
	if (!mapper)
		return -EINVAL;

	if (hw_ip > AMDGPU_MAX_IP_NUM)
		return -EINVAL;

	mapper->hw_ip = hw_ip;
	mutex_init(&mapper->lock);

	memset(mapper->queue_map, 0, sizeof(mapper->queue_map));

	return 0;
}

static struct amdgpu_ring *get_cached_map(struct amdgpu_queue_mapper *mapper,
					  int ring)
{
	return mapper->queue_map[ring];
}

static int update_cached_map(struct amdgpu_queue_mapper *mapper,
			     int ring, struct amdgpu_ring *pring)
{
	if (WARN_ON(mapper->queue_map[ring])) {
		DRM_ERROR("Un-expected ring re-map\n");
		return -EINVAL;
	}

	mapper->queue_map[ring] = pring;

	return 0;
}

static int amdgpu_identity_map(struct amdgpu_device *adev,
			       struct amdgpu_queue_mapper *mapper,
			       int ring,
			       struct amdgpu_ring **out_ring)
{
	switch (mapper->hw_ip) {
	case AMDGPU_HW_IP_GFX:
		*out_ring = &adev->gfx.gfx_ring[ring];
		break;
	case AMDGPU_HW_IP_COMPUTE:
		*out_ring = &adev->gfx.compute_ring[ring];
		break;
	case AMDGPU_HW_IP_DMA:
		*out_ring = &adev->sdma.instance[ring].ring;
		break;
	case AMDGPU_HW_IP_UVD:
		*out_ring = &adev->uvd.ring;
		break;
	case AMDGPU_HW_IP_VCE:
		*out_ring = &adev->vce.ring[ring];
		break;
	default:
		*out_ring = NULL;
		DRM_ERROR("unknown HW IP type: %d\n", mapper->hw_ip);
		return -EINVAL;
	}

	return update_cached_map(mapper, ring, *out_ring);
}

int amdgpu_queue_mgr_init(struct amdgpu_device *adev,
			  struct amdgpu_queue_mgr *mgr)
{
	int i;

	if (!adev || !mgr)
		return -EINVAL;

	memset(mgr, 0, sizeof(*mgr));

	for (i = 0; i < AMDGPU_MAX_IP_NUM; ++i)
		amdgpu_queue_mapper_init(&mgr->mapper[i], i);

	return 0;
}

int amdgpu_queue_mgr_fini(struct amdgpu_device *adev,
			  struct amdgpu_queue_mgr *mgr)
{
	return 0;
}

int amdgpu_queue_mgr_map(struct amdgpu_device *adev,
			 struct amdgpu_queue_mgr *mgr,
			 int hw_ip, int ring,
			 struct amdgpu_ring **out_ring)
{
	int r;
	struct amdgpu_queue_mapper *mapper = &mgr->mapper[hw_ip];

	if (!adev || !mgr || !out_ring)
		return -EINVAL;

	if (hw_ip >= AMDGPU_MAX_IP_NUM)
		return -EINVAL;

	if (ring >= AMDGPU_MAX_RINGS)
		return -EINVAL;

	r = amdgpu_ring_is_valid_index(adev, hw_ip, ring);
	if (r)
		return r;

	mutex_lock(&mapper->lock);

	*out_ring = get_cached_map(mapper, ring);
	if (*out_ring) {
		/* cache hit */
		r = 0;
		goto out_unlock;
	}

	switch (mapper->hw_ip) {
	case AMDGPU_HW_IP_GFX:
	case AMDGPU_HW_IP_COMPUTE:
	case AMDGPU_HW_IP_DMA:
	case AMDGPU_HW_IP_UVD:
	case AMDGPU_HW_IP_VCE:
		r = amdgpu_identity_map(adev, mapper, ring, out_ring);
		break;
	default:
		*out_ring = NULL;
		r = -EINVAL;
		DRM_ERROR("unknown HW IP type: %d\n", mapper->hw_ip);
	}

out_unlock:
	mutex_unlock(&mapper->lock);
	return r;
}
