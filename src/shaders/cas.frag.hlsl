Texture2D sourceTex;
SamplerState sourceType;

struct PSInput {
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

const float Contrast = 1.0;

float4 main(PSInput input) : SV_TARGET {
	float3 a = sourceTex.Sample(sourceType, input.uv, int2(-1, -1)).rgb;
    float3 b = sourceTex.Sample(sourceType, input.uv, int2(0, -1)).rgb;
    float3 c = sourceTex.Sample(sourceType, input.uv, int2(1, -1)).rgb;
    float3 d = sourceTex.Sample(sourceType, input.uv, int2(-1, 0)).rgb;
    float3 e = sourceTex.Sample(sourceType, input.uv, int2(0, 0)).rgb;
    float3 f = sourceTex.Sample(sourceType, input.uv, int2(1, 0)).rgb;
    float3 g = sourceTex.Sample(sourceType, input.uv, int2(-1, 1)).rgb;
    float3 h = sourceTex.Sample(sourceType, input.uv, int2(0, 1)).rgb;
    float3 i = sourceTex.Sample(sourceType, input.uv, int2(1, 1)).rgb;
  
	// Soft min and max.
	//  a b c             b
	//  d e f * 0.5  +  d e f * 0.5
	//  g h i             h
    // These are 2.0x bigger (factored out the extra multiply).
    float3 mnRGB = min(min(min(d, e), min(f, b)), h);
    float3 mnRGB2 = min(mnRGB, min(min(a, c), min(g, i)));
    mnRGB += mnRGB2;

    float3 mxRGB = max(max(max(d, e), max(f, b)), h);
    float3 mxRGB2 = max(mxRGB, max(max(a, c), max(g, i)));
    mxRGB += mxRGB2;

    // Smooth minimum distance to signal limit divided by smooth max.
    float3 rcpMRGB = rcp(mxRGB);
    float3 ampRGB = saturate(min(mnRGB, 2.0 - mxRGB) * rcpMRGB);    
    
    // Shaping amount of sharpening.
    ampRGB = rsqrt(ampRGB);
    
    float peak = 8.0 - 3.0 * Contrast;
    float3 wRGB = -rcp(ampRGB * peak);

    float3 rcpWeightRGB = rcp(1.0 + 4.0 * wRGB);

    //                          0 w 0
    //  Filter shape:           w 1 w
    //                          0 w 0  
    float3 window = (b + d) + (f + h);
    float3 outColor = saturate((window * wRGB + e) * rcpWeightRGB);
	return float4(outColor, 1);
}
