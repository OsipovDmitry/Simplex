#include<clip_space.glsl>
#include<constants.glsl>
#include<line_segment.glsl>
#include<plane.glsl>
#include<transform.glsl>

struct Frustum
{
	Transform viewTransform;
	ClipSpace clipSpace;
	vec2 ZRange;
	
	Transform cachedViewTransformInverted;
	mat4x4 cachedProjectionMatrix;
	mat4x4 cachedProjectionMatrixInverted;
	mat4x4 cachedViewProjectionMatrix;
	mat4x4 cachedViewProjectionMatrixInverted;
	vec3 cachedPoints[FRUSTUM_POINTS_COUNT]; // in world space
	Plane cachedPlanes[FRUSTUM_PLANES_COUNT]; // in world space
};

Frustum makeFrustum(in Transform vt, in ClipSpace cp, in vec2 Z)
{
	const Transform vtInv = transformInverted(vt);
	const mat4x4 pm = clipSpaceProjectionMatrix(cp, Z);
	const mat4x4 pmInv = inverse(pm);
	const mat4x4 vpm = pm * transformMat4x4(vt);
	const mat4x4 vpmInv = inverse(vpm);
	
	vec3[FRUSTUM_POINTS_COUNT] points;
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
	{
		const vec3 v = vec3(
			float(bitfieldExtract(i, 0, 1)),
			float(bitfieldExtract(i, 1, 1)),
			float(bitfieldExtract(i, 2, 1)));
		points[i] = projectPoint(vpmInv, 2.0f * v - vec3(1.0f));
	}
	
	Plane[FRUSTUM_PLANES_COUNT] planes;
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
    {
        const float sign = float(i % 2) * 2.0f - 1.0f;
        vec4 coefs;
        for (uint j = 0; j < 4u; ++j)
            coefs[j] = vpm[j][3u] - sign * vpm[j][i/2];
        planes[i] = makePlane(coefs);
    }
	
	return Frustum(vt, cp, Z, vtInv, pm, pmInv, vpm, vpmInv, points, planes);
}

Transform frustumViewTransform(in Frustum f)
{
	return f.viewTransform;
}

ClipSpace frustumClipSpace(in Frustum f)
{
	return f.clipSpace;
}

vec2 frustumZRange(in Frustum f)
{
	return f.ZRange;
}

Transform frustumViewTransformInverted(in Frustum f)
{
	return f.cachedViewTransformInverted;
}

mat4x4 frustumProjectionMatrix(in Frustum f)
{
	return f.cachedProjectionMatrix;
}

mat4x4 frustumProjectionMatrixInverted(in Frustum f)
{
	return f.cachedProjectionMatrixInverted;
}

mat4x4 frustumViewProjectionMatrix(in Frustum f)
{
	return f.cachedViewProjectionMatrix;
}

mat4x4 frustumViewProjectionMatrixInverted(in Frustum f)
{
	return f.cachedViewProjectionMatrixInverted;
}

vec3 frustumPoint(in Frustum f, in uint ID)
{
	return f.cachedPoints[ID];
}

vec3[FRUSTUM_POINTS_COUNT] frustumPoints(in Frustum f)
{
	return f.cachedPoints;
}

Plane frustumPlane(in Frustum f, in uint ID)
{
	return f.cachedPlanes[ID];
}

Plane[FRUSTUM_PLANES_COUNT] frustumPlanes(in Frustum f)
{
	return f.cachedPlanes;
}

LineSegment frustumEdge(in Frustum f, in uint ID)
{
	const uvec2 edgeIndices = boundingBoxEdgeIndices(ID);
	return makeLineSegment(
		frustumPoint(f, edgeIndices[0u]),
		frustumPoint(f, edgeIndices[1u]));
}

vec3 frustumClosestPoint(in Frustum f, in vec3 v)
{
	vec3 test = transformPoint(f.viewTransform, v);

	bool rSignChange = false;
	if (test[0u] < 0.0f)
	{
		rSignChange = true;
		test[0u] = -test[0u];
	}

	bool uSignChange = false;
	if (test[1u] < 0.0f)
	{
		uSignChange = true;
		test[1u] = -test[1u];
	}

	test[2u] = -test[2u];

	const vec3 minBound = projectPoint(f.cachedProjectionMatrixInverted, vec3(1.0f, 1.0f, -1.0f));
	const vec3 maxBound = projectPoint(f.cachedProjectionMatrixInverted, vec3(1.0f, 1.0f, 1.0f));
	const float fDRatio = maxBound[2u] / minBound[2u];

	const float rmin = minBound[0u];
	const float rmax = maxBound[0u];
	const float umin = minBound[1u];
	const float umax = maxBound[1u];
	const float dmin = -minBound[2u];
	const float dmax = -maxBound[2u];
	const float rminSqr = rmin * rmin;
	const float uminSqr = umin * umin;
	const float dminSqr = dmin * dmin;
	const float minRDDot = rminSqr + dminSqr;
	const float minUDDot = uminSqr + dminSqr;
	const float minRUDDot = rminSqr + minUDDot;
	const float maxRDDot = fDRatio * minRDDot;
	const float maxUDDot = fDRatio * minUDDot;
	const float maxRUDDot = fDRatio * minRUDDot;

	vec3 closest;
	float rDot, uDot, rdDot, udDot, rudDot, rEdgeDot, uEdgeDot, t;
	if (test[2u] >= dmax)
	{
		if (test[0u] <= rmax)
		{
			if (test[1u] <= umax)
			{
				// F-face
				closest[0u] = test[0u];
				closest[1u] = test[1u];
				closest[2u] = dmax;
			}
			else
			{
				// UF-edge
				closest[0u] = test[0u];
				closest[1u] = umax;
				closest[2u] = dmax;
			}
		}
		else
		{
			if (test[1u] <= umax)
			{
				// LF-edge
				closest[0u] = rmax;
				closest[1u] = test[1u];
				closest[2u] = dmax;
			}
			else
			{
				// LUF-vertex
				closest[0u] = rmax;
				closest[1u] = umax;
				closest[2u] = dmax;
			}
		}
	}
	else if (test[2u] <= dmin)
	{
		if (test[0u] <= rmin)
		{
			if (test[1u] <= umin)
			{
				// N-face
				closest[0u] = test[0u];
				closest[1u] = test[1u];
				closest[2u] = dmin;
			}
			else
			{
				udDot = umin * test[1u] + dmin * test[2u];
				if (udDot >= maxUDDot)
				{
					// UF-edge
					closest[0u] = test[0u];
					closest[1u] = umax;
					closest[2u] = dmax;
				}
				else if (udDot >= minUDDot)
				{
					// U-face
					uDot = dmin * test[1u] - umin * test[2u];
					t = uDot / minUDDot;
					closest[0u] = test[0u];
					closest[1u] = test[1u] - t * dmin;
					closest[2u] = test[2u] + t * umin;
				}
				else
				{
					// UN-edge
					closest[0u] = test[0u];
					closest[1u] = umin;
					closest[2u] = dmin;
				}
			}
		}
		else
		{
			if (test[1u] <= umin)
			{
				rdDot = rmin * test[0u] + dmin * test[2u];
				if (rdDot >= maxRDDot)
				{
					// LF-edge
					closest[0u] = rmax;
					closest[1u] = test[1u];
					closest[2u] = dmax;
				}
				else if (rdDot >= minRDDot)
				{
					// L-face
					rDot = dmin * test[0u] - rmin * test[2u];
					t = rDot / minRDDot;
					closest[0u] = test[0u] - t * dmin;
					closest[1u] = test[1u];
					closest[2u] = test[2u] + t * rmin;
				}
				else
				{
					// LN-edge
					closest[0u] = rmin;
					closest[1u] = test[1u];
					closest[2u] = dmin;
				}
			}
			else
			{
				rudDot = rmin * test[0u] + umin * test[1u] + dmin * test[2u];
				rEdgeDot = umin * rudDot - minRUDDot * test[1u];
				if (rEdgeDot >= 0.0f)
				{
					rdDot = rmin * test[0u] + dmin * test[2u];
					if (rdDot >= maxRDDot)
					{
						// LF-edge
						closest[0u] = rmax;
						closest[1u] = test[1u];
						closest[2u] = dmax;
					}
					else if (rdDot >= minRDDot)
					{
						// L-face
						rDot = dmin * test[0u] - rmin * test[2u];
						t = rDot / minRDDot;
						closest[0u] = test[0u] - t * dmin;
						closest[1u] = test[1u];
						closest[2u] = test[2u] + t * rmin;
					}
					else
					{
						// LN-edge
						closest[0u] = rmin;
						closest[1u] = test[1u];
						closest[2u] = dmin;
					}
				}
				else
				{
					uEdgeDot = rmin * rudDot - minRUDDot * test[0u];
					if (uEdgeDot >= 0.0f)
					{
						udDot = umin * test[1u] + dmin * test[2u];
						if (udDot >= maxUDDot)
						{
							// UF-edge
							closest[0u] = test[0u];
							closest[1u] = umax;
							closest[2u] = dmax;
						}
						else if (udDot >= minUDDot)
						{
							// U-face
							uDot = dmin * test[1u] - umin * test[2u];
							t = uDot / minUDDot;
							closest[0u] = test[0u];
							closest[1u] = test[1u] - t * dmin;
							closest[2u] = test[2u] + t * umin;
						}
						else
						{
							// UN-edge
							closest[0u] = test[0u];
							closest[1u] = umin;
							closest[2u] = dmin;
						}
					}
					else
					{
						if (rudDot >= maxRUDDot)
						{
							// LUF-vertex
							closest[0u] = rmax;
							closest[1u] = umax;
							closest[2u] = dmax;
						}
						else if (rudDot >= minRUDDot)
						{
							// LU-edge
							t = rudDot / minRUDDot;
							closest[0u] = t * rmin;
							closest[1u] = t * umin;
							closest[2u] = t * dmin;
						}
						else
						{
							// LUN-vertex
							closest[0u] = rmin;
							closest[1u] = umin;
							closest[2u] = dmin;
						}
					}
				}
			}
		}
	}
	else
	{
		rDot = dmin * test[0u] - rmin * test[2u];
		uDot = dmin * test[1u] - umin * test[2u];
		if (rDot <= 0.0f)
		{
			if (uDot <= 0.0f)
			{
				// point inside frustum
				closest = test;
			}
			else
			{
				udDot = umin * test[1u] + dmin * test[2u];
				if (udDot >= maxUDDot)
				{
					// UF-edge
					closest[0u] = test[0u];
					closest[1u] = umax;
					closest[2u] = dmax;
				}
				else
				{
					// U-face
					t = uDot / minUDDot;
					closest[0u] = test[0u];
					closest[1u] = test[1u] - t * dmin;
					closest[2u] = test[2u] + t * umin;
				}
			}
		}
		else
		{
			if (uDot <= 0.0f)
			{
				rdDot = rmin * test[0u] + dmin * test[2u];
				if (rdDot >= maxRDDot)
				{
					// LF-edge
					closest[0u] = rmax;
					closest[1u] = test[1u];
					closest[2u] = dmax;
				}
				else
				{
					// L-face
					t = rDot / minRDDot;
					closest[0u] = test[0u] - t * dmin;
					closest[1u] = test[1u];
					closest[2u] = test[2u] + t * rmin;
				}
			}
			else
			{
				rudDot = rmin * test[0u] + umin * test[1u] + dmin * test[2u];
				rEdgeDot = umin * rudDot - minRUDDot * test[1u];
				if (rEdgeDot >= 0.0f)
				{
					rdDot = rmin * test[0u] + dmin * test[2u];
					if (rdDot >= maxRDDot)
					{
						// LF-edge
						closest[0u] = rmax;
						closest[1u] = test[1u];
						closest[2u] = dmax;
					}
					else // assert( rdDot >= minRDDot )
					{
						// L-face
						t = rDot / minRDDot;
						closest[0u] = test[0u] - t * dmin;
						closest[1u] = test[1u];
						closest[2u] = test[2u] + t * rmin;
					}
				}
				else
				{
					uEdgeDot = rmin * rudDot - minRUDDot * test[0u];
					if (uEdgeDot >= 0.0f)
					{
						udDot = umin * test[1u] + dmin * test[2u];
						if (udDot >= maxUDDot)
						{
							// UF-edge
							closest[0u] = test[0u];
							closest[1u] = umax;
							closest[2u] = dmax;
						}
						else // assert( udDot >= minUDDot )
						{
							// U-face
							t = uDot / minUDDot;
							closest[0u] = test[0u];
							closest[1u] = test[1u] - t * dmin;
							closest[2u] = test[2u] + t * umin;
						}
					}
					else
					{
						if (rudDot >= maxRUDDot)
						{
							// LUF-vertex
							closest[0u] = rmax;
							closest[1u] = umax;
							closest[2u] = dmax;
						}
						else // assert( rudDot >= minRUDDot )
						{
							// LU-edge
							t = rudDot / minRUDDot;
							closest[0u] = t * rmin;
							closest[1u] = t * umin;
							closest[2u] = t * dmin;
						}
					}
				}
			}
		}
	}

	if (rSignChange)
		closest[0u] = -closest[0u];
	if (uSignChange)
		closest[1u] = -closest[1u];
	closest[2u] *= -1;

	return transformPoint(f.cachedViewTransformInverted, closest);
}

bool frustumIsPointInside(in Frustum f, in vec3 v)
{
	bool inside = true;
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
	{
		inside = (distanceToPlane(frustumPlane(f, i), v) > 0.0f);
		
		if (!inside)
			break;
	}
	
	return inside;
}

Frustum transformFrustum(in Frustum f, in Transform t)
{
	return makeFrustum(transformMult(f.viewTransform, transformInverted(t)), f.clipSpace, f.ZRange);
}