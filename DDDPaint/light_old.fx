////////////////////////////////////////////////////////////////////////////////
// Filename: light.fx
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;
Texture2D blendTexture;

float4 ambientColor;
float4 diffuseColor;
float3 lightDirection;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState SampleTypeLinearComp
{
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
	ComparisonFunc = NEVER;
};

SamplerState SampleTypePoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

/////////////////////
// BLENDING STATES //
/////////////////////
BlendState AlphaBlendingState
{
    BlendEnable[0] = False;
    DestBlend = INV_SRC_ALPHA;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
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

	 // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);
    
    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;
	float4 blendTextureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	float4 light;
	float4 eye = float4(0.0, 0.0, -7.5, 0.0);
    float4 vhalf;
	float specular;
	float4 specularMaterial = float4(10.0, 10.0, 10.0, 1.0);

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleTypeLinear, input.tex);
	textureColor.a = 1.0f;

	blendTextureColor = blendTexture.Sample(SampleTypePoint, input.tex);

	// If the color is black on the texture then treat this pixel as transparent.
    if(blendTextureColor.r <= 0.00f && blendTextureColor.g <= 0.00f && blendTextureColor.b <= 0.00f)
    {
        blendTextureColor.a = 0.0f;
    }
    else
    {
	// commented out part to use with alpha
        //textureColor = blendTextureColor;
		textureColor = float4(blendTextureColor.rgb * blendTextureColor.a, blendTextureColor.a) + float4(textureColor.rgb * (1.0 - blendTextureColor.a), textureColor.a);
    }

	// Set the default output color to the ambient light value for all pixels.
    color = ambientColor;

	 // Invert the light direction for calculations.
    lightDir = -lightDirection;
	lightDir = normalize(lightDir);

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));

	light = float4(lightDir, 0.0f);
	vhalf = normalize(light + eye);

	specular = dot(input.normal, vhalf);
    specular = pow(specular, 128);

	if(lightIntensity > 0.0f)
    {
	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    /*color = (textureColor * lightIntensity + specular*specularMaterial*lightIntensity);
	} else {
	color = (diffuseColor * textureColor * lightIntensity);
	}*/

	color += (textureColor * lightIntensity + specular*specularMaterial*lightIntensity);
	}

	// Saturate the final light color.
    color = saturate(color);

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    color = color * textureColor;

    return color;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 LightTechnique
{
    pass pass0
    {
	    SetBlendState(AlphaBlendingState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, LightVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, LightPixelShader()));
        SetGeometryShader(NULL);
    }
}