#include<constants.glsl>
#include<line.glsl>
#include<plane.glsl>
#include<range.glsl>
#include<transform.glsl>

vec3 frustumCalculatePoint(in mat4x4 projectionMatrixInverted, in uint ID)
{
	const vec3 v = vec3(
		float(bitfieldExtract(ID, 0, 1)),
		float(bitfieldExtract(ID, 1, 1)),
		float(bitfieldExtract(ID, 2, 1)));
	return projectPoint(projectionMatrixInverted, 2.0f * v - vec3(1.0f));
}

Plane frustumCalculatePlane(in mat4x4 projectionMatrix, in uint ID)
{
	const float sign = float(ID % 2) * 2.0f - 1.0f;
	vec4 coefs;
	for (uint j = 0; j < 4u; ++j)
		coefs[j] = projectionMatrix[j][3u] - sign * projectionMatrix[j][ID/2];
	return makePlane(coefs);
}

vec3 frustumCalculateSideEdgeAbsDirection(in mat4x4 projectionMatrixInverted)
{
	const vec3 p0 = projectPoint(projectionMatrixInverted, vec3(1.0f, 1.0f, -1.0f));
	const vec3 p1 = projectPoint(projectionMatrixInverted, vec3(1.0f, 1.0f, +1.0f));
	return normalize(p1 - p0);
}

Line frustumCalculateFaceNormalLine(in mat4x4 projectionMatrix, in uint ID)
{
	const float sign = float(ID % 2) * 2.0f - 1.0f;
	
	vec4 coefs;
	for (uint j = 0; j < 4u; ++j)
		coefs[j] = projectionMatrix[j][3u] - sign * projectionMatrix[j][ID/2];
		
	const Plane p = makePlane(coefs);
	
	return makeLine(planeAnyPoint(p), planeNormal(p));
}

Range frustumProjectOnLine(in vec3 fPoints[FRUSTUM_POINTS_COUNT], in Line l)
{
	Range result = makeEmptyRange();
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
		result = rangeExpand(result, lineProjectOn(l, fPoints[i]));
	return result;
}

bool frustumIsPointInside(in Plane fPlanes[FRUSTUM_PLANES_COUNT], in vec3 p)
{
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
	{
		if (distanceToPlane(fPlanes[i], p) < 0.0f)
			return false;
	}
	
	return true;
}

vec3 frustumClosestPoint(in mat4x4 projectionMatrixInverted, in vec3 v)
{
	vec3 test = v;

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

	const vec3 minBound = projectPoint(projectionMatrixInverted, vec3(1.0f, 1.0f, -1.0f));
	const vec3 maxBound = projectPoint(projectionMatrixInverted, vec3(1.0f, 1.0f, 1.0f));
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

	return closest;
}