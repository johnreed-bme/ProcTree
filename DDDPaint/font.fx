////////////////////////////////////////////////////////////////////////////////
// Filename: font.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;
float4 pixelColor;
bool useAlpha;


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

/////////////////////
// BLENDING STATES //
/////////////////////
BlendState AlphaBlendingState
{
    BlendEnable[0] = TRUE;
    DestBlend = INV_SRC_ALPHA;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FontVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FontPixelShader(PixelInputType input) : SV_Target
{
    float4 color;
	
	
    // Sample the texture pixel at this location.
    color = shaderTexture.Sample(SampleType, input.tex);

	
    // If the color is black on the texture then treat this pixel as transparent.
    /*if(color.r == 0.0f)
    {
        color.a = 1.0f;
    } else if (color.r < 1.0f){
	
    } else {
		return float4(0,0,0,0);
	}*/

	if(color.r == 1.0f) {
		return float4(0,0,0,0);
	}
	
	// without alpha usage
	if (!useAlpha){
		if (color.a > 0.0){
			color = float4(0.2,0.2,0.2,0.5);
		} else {
			color = float4(0,0,0,0);
		}
    } else {
		// with alpha usage
		if (color.a > 0.0){
			color = float4(0.0,0.0,0.0,color.a/2);
		} else {
			color = float4(0,0,0,0);
		}
	}

	if (input.tex.x >= 0.5 && input.tex.x <= 0.51 && input.tex.y >= 0.5 && input.tex.y <= 0.51 ){
		color = float4(0.4,0,0,1);
	}

    return color;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 FontTechnique
{
    pass pass0
    {
        SetBlendState(AlphaBlendingState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, FontVertexShader()));
        SetPixelShader(CompileShader(ps_5_0, FontPixelShader()));
        SetGeometryShader(NULL);
    }
}