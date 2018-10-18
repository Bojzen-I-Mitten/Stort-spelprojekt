#ifndef THOMAS_SHADER_VARIABLES_INCLUDED
#define THOMAS_SHADER_VARIABLES_INCLUDED

#define THOMAS_MATRIX_P thomas_MatrixP
#define THOMAS_MATRIX_V thomas_MatrixV
#define THOMAS_MATRIX_I_V thomas_MatrixInvV
#define THOMAS_MATRIX_VP thomas_MatrixVP
/*#define THOMAS_MATRIX_M thomas_ObjectToWorld[0]

#define THOMAS_MATRIX_MVP mul(thomas_MatrixVP, thomas_ObjectToWorld[0])
#define THOMAS_MATRIX_MV mul(thomas_MatrixV, thomas_ObjectToWorld[0])
#define THOMAS_MATRIX_T_MV transpose(THOMAS_MATRIX_MV)
#define THOMAS_MATRIX_IT_MV transpose(mul(thomas_WorldToObject[0], thomas_MatrixInvV))*/


// ----------------------------------------------------------------------------


cbuffer ThomasPerFrame
{
	float4 thomas_DeltaTime; // dt, 1/dt, smoothdt, 1/smoothdt
};


cbuffer ThomasPerCamera
{
	//float4 thomas_CameraWorldClipPlanes[6];

	float4x4 thomas_MatrixP;
	float4x4 thomas_MatrixV;
	float4x4 thomas_MatrixInvV;
	float4x4 thomas_MatrixVP;

	float3 _WorldSpaceCameraPos;
	float3 _WorldSpaceCameraDir;
};



// ----------------------------------------------------------------------------

cbuffer ThomasPerObject
{
	float4x4 thomas_ObjectToWorld[50];
	float4x4 thomas_WorldToObject[50];
	float4x4 thomas_Bone_Array[120];
};


// ----------------------------------------------------------------------------

#endif