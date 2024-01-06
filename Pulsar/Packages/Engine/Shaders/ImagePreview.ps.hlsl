#include "PostProcessing.inc.hlsl"


cbuffer Properties : register(b0, space3)
{
    float4 _CheckerColorA;
    float4 _CheckerColorB;
    float  _GridSize;
    int    _Flags;
}

#define FLAGS_SRGB 1
#define FLAGS_EnableCheckerBackground 2
#define FLAGS_CHANNEL_R 4
#define FLAGS_CHANNEL_G 8
#define FLAGS_CHANNEL_B 16
#define FLAGS_CHANNEL_A 32

Texture2D _Image : register(t0, space3);


float4 main(PPVSOutput v2f) : SV_TARGET
{
    return _CheckerColorA;
    float4 output = _Image.Sample(DefaultSampler, v2f.TexCoord0);

    if (_Flags & FLAGS_EnableCheckerBackground)
    {
        float2 channels = saturate(fmod((TargetBuffer.Resolution / _GridSize), 2));
        float crossArea = 1-saturate(channels.x + channels.y);
        float blackArea = channels.x * channels.y;
        float4 checker = (_CheckerColorA, _CheckerColorB, crossArea + blackArea);
        output = lerp(checker, output, output.a);
    }

    if (_Flags & FLAGS_SRGB)
    {
        output = float4(pow(output.xyz, 2.2), 1);
    }

    return output;
}