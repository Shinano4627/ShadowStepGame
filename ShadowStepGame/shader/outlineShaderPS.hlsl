#include "common.hlsl"

float4 main(in PS_IN input) : SV_Target
{
    return float4(1.0, 1.0, 0.0, 1.0);
    //return input.col;
}
