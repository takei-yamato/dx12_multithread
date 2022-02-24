
// コンスタントバッファ
cbuffer global: register(b0) {
	matrix world;
	matrix viewProj;
	float4 color;

};

// スタティックサンプラ
SamplerState staticSampler : register(s0);

// 頂点シェーダ出力内容
struct VS_OUTPUT {
	float4 pos	: SV_POSITION;
	float4 uv	: TEXCOORD0;
	float4 color: COLOR;
};

//---------------------------------------------------------------------------------
/**
 * @brief
 * 頂点シェーダ
 */
VS_OUTPUT vs(float4 pos : POSITION, float4 uv : TEXCOORD) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = mul(mul(pos, world), viewProj);
	output.uv = uv;
	output.color = color;
	return output;
}

//---------------------------------------------------------------------------------
/**
 * @brief
 * ピクセルシェーダ
 */
float4 ps(VS_OUTPUT input) : SV_Target {
	return input.color;
}