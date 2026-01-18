void swap(inout float t0, inout float t1)
{
	const float tmp = t0;
	t0 = t1;
	t1 = tmp;
}