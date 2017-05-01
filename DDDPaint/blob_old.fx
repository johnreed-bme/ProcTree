////////////////////////////////////////////////////////////////////////////////
// Filename: blob.fx
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D texTexture;
Texture2D brushTexture;
bool mousePressed;
float2 mousePos;
float2 screenRes;
float2 brushSize;
float3 brushColor;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
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
PixelInputType BlobVertexShader(VertexInputType input)
{
    PixelInputType output = (PixelInputType)0;

	
	float2 tex = texTexture.Load(int3(mousePos.xy, 0));

	if(tex.x > 0.0f && tex.y > 0.0f)
	input.position.xy += float2((tex.x*screenRes.x-brushSize.x/2), (tex.y*-screenRes.y+brushSize.y/2));
	else
	input.position.xy -= float2(1000.0f, 1000.0f);
	

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.z = 1.0f;
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 BlobPixelShader(PixelInputType input) : SV_Target
{

	float4 brush;


	brush = brushTexture.Sample(SampleType, input.tex);

	saturate(brush);
		

	if ((brush.a < 1.0f)){
		brush = float4(0.0f, 0.0f, 0.0f, 1.0f);
	} else {
		brush = float4(brushColor.x, brushColor.y, brushColor.z, 0.0f);
	}

	
	


    return brush;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 BlobTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, BlobVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, BlobPixelShader()));
        SetGeometryShader(NULL);
    }
}