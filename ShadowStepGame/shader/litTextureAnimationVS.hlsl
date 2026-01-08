#include "common.hlsl"

PS_IN main(in VS_IN input)
{
    PS_IN output;

    //positoin=============================
    // スキニングの処理
    float4 Pos = float4(input.pos.xyz, 1.0f);
    float4 normal = float4(input.nrm.xyz, 0.0); // 入力法線をfloat4に変換(w=0)
    
    // ボーンウェイトの合計を計算
    //float totalWeight = input.boneWeight.x + input.boneWeight.y + input.boneWeight.z + input.boneWeight.w;
    
    // ウェイトがある場合のみスキニング処理を行う（閾値を使用）
    //if (totalWeight > 0.001f)
    {
        float4x4 Comb = (float4x4) 0;
        for (int i = 0; i < 4; i++)
        {
            // ウェイトが0より大きく、インデックスが有効な場合のみ処理
            if (input.boneWeight[i] > 0.0f && input.boneIndex[i] >= 0)
            {
                // 重みを計算しながら行列生成
                Comb += BoneMatrix[input.boneIndex[i]] * input.boneWeight[i];
            }
        }
        
        Pos = mul(Comb, Pos);
        
        Comb._41 = 0.0f; // 移動成分を除去
        Comb._42 = 0.0f;
        Comb._43 = 0.0f;
        Comb._44 = 1.0f;
        
        normal = mul(Comb, normal);
    }
	    
    // ワールド、ビュー、プロジェクション行列を掛け合わせて座標変換を行う
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    output.pos = mul(Pos, wvp);
	
	//color=============================
	// 法線ベクトルの変換と正規化を行う
	// 入力された法線をワールド空間に変換し、正規化する
    float4 wolrdNormal = mul(normal, World); // 法線をワールド空間に変換
    wolrdNormal = normalize(wolrdNormal); // 法線ベクトルを正規化
	
	// 光源方向と法線ベクトルの内積を計算して拡散光を求める
    float d = -dot(Light.Direction.xyz, wolrdNormal.xyz); // 光の方向と法線の内積を計算
	
    d = saturate(d); // dの値を0~1にクランプ
	
    output.col.xyz = input.col.xyz * d * Light.Diffuse.xyz; // 拡散光の影響を乗算
    output.col.xyz += input.col.xyz * Light.Ambient.xyz; // アンビエント光を加算
    output.col.xyz += Material.Emission.xyz; // Emissionを加算
    output.col.a = input.col.a * Material.Diffuse.a; // アルファ値
	
	//texture=============================
	// テクスチャ座標はそのまま使用
    output.tex = input.tex;
	
    return output;
}
