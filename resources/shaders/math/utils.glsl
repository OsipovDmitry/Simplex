#include<constants.glsl>

void swap(inout float t0, inout float t1)
{
	const float swapped = t0;
	t0 = t1;
	t1 = swapped;
}

vec3 orth(in uint orthID)
{
	vec3 result = vec3(0.0f);
	result[orthID] = 1.0f;
	return result;
}

vec2 screenQuadVertexZO(in uint vertexID)
{
	return vec2(float(bitfieldExtract(vertexID, 0, 1)), float(bitfieldExtract(vertexID, 1, 1)));
}

vec2 screenQuadVertex(in uint vertexID)
{
	return 2.0f * screenQuadVertexZO(vertexID) - vec2(1.0f);
}

const uint[64u] NTH_NUMBER_WITH_EVEN_BITS_COUNT = uint[64u](
    0, 3, 5, 6, 9, 10, 12, 15, 17, 18,
    20, 23, 24, 27, 29, 30, 33, 34, 36,
    39, 40, 43, 45, 46, 48, 51, 53, 54,
    57, 58, 60, 63, 65, 66, 68, 71, 72,
    75, 77, 78, 80, 83, 85, 86, 89, 90,
    92, 95, 96, 99, 101, 102, 105, 106, 108,
    111, 113, 114, 116, 119, 120, 123, 125, 126);

uvec2 boundingBoxEdgeIndices(in uint ID)
{
	const uint e = NTH_NUMBER_WITH_EVEN_BITS_COUNT[ID / 3u];
	return uvec2(e, e ^ (1u << (ID % 3u)));
}

float gauss(in float x, in float StandardDeviation)
{
	return exp(-(x * x) / (2.0f * StandardDeviation * StandardDeviation)) / (StandardDeviation * ROOT_TWO_PI);
}

float NO2ZO(in float value) { return 0.5f * value + 0.5f; }
vec2 NO2ZO(in vec2 value) { return 0.5f * value + vec2(0.5f); }
vec3 NO2ZO(in vec3 value) { return 0.5f * value + vec3(0.5f); }

float ZO2NO(in float value) { return 2.0f * value - 1.0f; }
vec2 ZO2NO(in vec2 value) { return 2.0f * value - vec2(1.0f); }
vec3 ZO2NO(in vec3 value) { return 2.0f * value - vec3(1.0f); }