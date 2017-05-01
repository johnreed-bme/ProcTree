////////////////////////////////////////////////////////////////////////////////
// Filename: Light.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
static const int MAX_BONES = 10;

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
	matrix boneTransformationOffset[MAX_BONES];
	matrix boneInverseTransformationOffset[MAX_BONES];
	float3 boneTransformation[MAX_BONES];
};

struct MatrixData
{
    matrix data;
};

StructuredBuffer<MatrixData> myInstanceData : register(t0);

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float4 matrix1 : TEXCOORD1;
	float4 matrix2 : TEXCOORD2;
	float4 matrix3 : TEXCOORD3;
	float4 matrix4 : TEXCOORD4;
	float4 bone_weights : TEXCOORD5;
	float instanceMatrixOffset : TEXCOORD6;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 color : COLOR;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Update the position of the vertices based on the data for this particular instance.
	
	float4 originalVertexPosition = input.position;

	float4 originalInputNormal, transformedInputNormal;
	originalInputNormal.xyz = input.normal;
	originalInputNormal.w = 0.0f;

	output.position = mul(originalVertexPosition, myInstanceData[input.instanceMatrixOffset].data) * input.bone_weights.x;
	output.position += mul(originalVertexPosition, myInstanceData[input.instanceMatrixOffset+1].data) * input.bone_weights.y;
	output.position += mul(originalVertexPosition, myInstanceData[input.instanceMatrixOffset+2].data) * input.bone_weights.z;

	transformedInputNormal = mul(originalInputNormal, myInstanceData[input.instanceMatrixOffset].data) * input.bone_weights.x;
	transformedInputNormal += mul(originalInputNormal, myInstanceData[input.instanceMatrixOffset+1].data) * input.bone_weights.y;
	transformedInputNormal += mul(originalInputNormal, myInstanceData[input.instanceMatrixOffset+2].data) * input.bone_weights.z;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix.
    output.normal = mul(transformedInputNormal.xyz, (float3x3)worldMatrix);
	//output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	output.color.xyz = input.bone_weights.xyz; 
    
    return output;
}