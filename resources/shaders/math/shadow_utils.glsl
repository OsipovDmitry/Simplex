float Hamburger4MSMShadowIntensity(
	in vec4 _4Moments,
	in float FragmentDepth,
	in float DepthBias,
	in float MomentBias)
{
	// Bias input data to avoid artifacts
	const vec4 b = mix(_4Moments, vec4(0.5f), MomentBias);
	vec3 z;
	z[0] = FragmentDepth - DepthBias;
	
	// Compute a Cholesky factorization of the Hankel matrix B storing only
	// non-trivial entries or related products
	const float L32D22 = -b[0] * b[1] + b[2];
	const float D22 = -b[0] * b[0] + b[1];
	const float SquaredDepthVariance = -b[1] * b[1] + b[3];
	const float D33D22 = dot(vec2(SquaredDepthVariance, -L32D22), vec2(D22, L32D22));
	const float InvD22 = 1.0f / D22;
	const float L32 = L32D22 * InvD22;
	
	// Obtain a scaled inverse image of bz=(1,z[0] ,z[0]*z[0])^T
	vec3 c = vec3(1.0f, z[0], z[0]*z[0]);
	
	// Forward substitution to solve L*c1=bz
	c[1] -= b.x;
	c[2] -= b.y + L32 * c[1];
	
	// Scaling to solve D*c2=c1
	c[1] *= InvD22;
	c[2] *= D22 / D33D22;
	
	// Backward substitution to solve L^T*c3=c2
	c[1] -= L32 * c[2];
	c[0] -= dot(c.yz, b.xy);
	
	// Solve the quadratic equation c[0]+c[1]*z+c[2]*z^2 to obtain solutions z[1]
	// and z[2]
	const float p = c[1] / c[2];
	const float q = c[0] / c[2];
	const float D = ((p * p) / 4.0f) - q;
	const float r = sqrt(D);
	z[1] = -(p / 2.0f) - r;
	z[2] = -(p / 2.0f) + r;
	
	// Construct a solution composed of three Dirac−deltas and evaluate its CDF
	const vec4 Switch = (z[2] < z[0]) ?
		vec4(z[1], z[0], 1.0f, 1.0f) :
		((z[1] < z[0]) ? vec4(z[0], z[1], 0.0f, 1.0f) : vec4(0.0f));
	const float Quotient = (Switch[0] * z[2] - b[0] * (Switch[0] + z[2]) + b[1]) /
		((z[2] - Switch[1]) * (z[0] - z[1]));
	
	return 1.0f - clamp(Switch[2] + Switch[3] * Quotient, 0.0f, 1.0f);
}

float Hausdorff4MSMShadowIntensity(
	in vec4 _4Moments,
	in float FragmentDepth,
	in float DepthBias,
	in float MomentBias)
{
	// Bias input data to avoid artifacts
	const vec4 b = mix(_4Moments, vec4(0.5f), MomentBias);
	vec3 z;
	z[0] = FragmentDepth - DepthBias;
	
	// Compute a Cholesky factorization of the Hankel matrix B storing only
	// non-trivial entries or related products
	const float L32D22 = -b[0] * b[1] + b[2];
	const float D22 = -b[0] * b[0] + b[1];
	const float SquaredDepthVariance = -b[1] * b[1] + b[3];
	const float D33D22 = dot(vec2(SquaredDepthVariance, -L32D22), vec2(D22, L32D22));
	const float InvD22 = 1.0f / D22;
	const float L32 = L32D22 * InvD22;

	// Obtain a scaled inverse image of bz=(1,z[0] ,z[0]*z[0])^T
	vec3 c = vec3(1.0f, z[0], z[0]*z[0]);
	
	// Forward substitution to solve L*c1=bz
	c[1] -= b.x;
	c[2] -= b.y + L32 * c[1];

	// Scaling to solve D*c2=c1
	c[1] *= InvD22;
	c[2] *= D22 / D33D22;

	// Backward substitution to solve L^T*c3=c2
	c[1] -= L32 * c[2];
	c[0] -= dot(c.yz, b.xy);

	// Solve the quadratic equation c[0]+c[1]*z+c[2]*z^2 to obtain solutions z[1]
	// and z[2]
	const float p = c[1] / c[2];
	const float q = c[0] / c[2];
	const float D = ((p * p) / 4.0f) - q;
	const float r = sqrt(D);
	z[1] = -(p / 2.0f) - r;
	z[2] = -(p / 2.0f) + r;
	
	// result
	float OutShadowIntensity;
	
	if ((z[1] < 0.0f) || (z[2] > 1.0f)) // Use a solution made of four deltas if the solution with three deltas is invalid
	{
		const float zFree = ((b[2] - b[1]) * z[0] + b[2] - b[3]) /
			((b[1] - b[0]) * z[0] + b[1] - b[2]);
		const float w1Factor = (z[0] > zFree) ? 1.0f : 0.0f;
		OutShadowIntensity =
			(b[1] - b[0] + (b[2] - b[0] -
				(zFree + 1.0f) * (b[1] - b[0])) * (zFree - w1Factor - z[0]) / (z[0] * (z[0] - zFree))) /
			(zFree - w1Factor) + 1.0f - b[0];
	}
	else // Use the solution with three deltas
	{
		const vec4 Switch = (z[2] < z[0]) ?
			vec4(z[1], z[0], 1.0f, 1.0f) :
			((z[1] < z[0]) ? vec4(z[0], z[1], 0.0f, 1.0f) : vec4(0.0f));
		const float Quotient = (Switch[0] * z[2] - b[0] * (Switch[0] + z[2]) + b[1]) /
			((z[2] - Switch[1]) * (z[0] - z[1]));
		OutShadowIntensity = Switch[2] + Switch[3] * Quotient;
	}
	
	return 1.0f - clamp(OutShadowIntensity, 0.0f, 1.0f);
}