uniform sampler2DRect u_GBufferDepthMap;
layout (r32f) uniform image2DRect u_OITDepthImage;

void main(void)
{
    ivec2 texelCoord = ivec2(gl_FragCoord.xy);
    float opaqueDepth = texelFetch(u_GBufferDepthMap, texelCoord).r;
    float transparentDepth = imageLoad(u_OITDepthImage, texelCoord).r;
    
    if (transparentDepth == 1.0f)
        discard;
    
    if (gl_FrontFacing)
        if (gl_FragCoord.z < opaqueDepth)
            gl_FragDepth = opaqueDepth - 0.5f;
    
    if (!gl_FrontFacing)
        if (gl_FragCoord.z > transparentDepth)
            gl_FragDepth = opaqueDepth + 0.5f;
}
