#include "common.hlsl"

PS_IN main(in VS_IN input)
{
    PS_IN output;
	
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    
    float4 pos = input.pos; 
    pos.xyz *= 1.05f; // 5“‚Ù‚Ç‹‘å‰»‚µ‚Ä—ÖŠs‚ğì‚é
    //pos.xyz += input.nrm * 0.1f; // –@ü•ûŒü‚É‰Ÿ‚µo‚·
    output.pos = mul(pos, wvp);
    output.tex = input.tex;
    output.col = input.col;
\
    return output;
}

