#pragma once
#include "..\utils\Math.h"

namespace thomas
{
	namespace graphics
	{
		class LightManager
		{
		public:
			/*struct DirectionalLightStruct
			{
				thomas::math::Vector4 lightColor;
				thomas::math::Vector3 lightDirection;
				float padding;
			};

			struct PointLightStruct
			{
				float constantAttenuation;
				float linearAttenuation;
				float quadraticAttenuation;
				float power;
				thomas::math::Vector4 lightColor;
				thomas::math::Vector3 position;
				float padding;		
			};

			struct LightBufferStruct
			{
				int nrOfDirectionalLights;
				int nrOfPointLights;
				int padding1;
				int padding2;
				DirectionalLightStruct directionalLights[3];
				PointLightStruct pointLights[20];
				
			} static s_lightstruct;*/



			/*MATCH ON GPU*/
			struct ConstantBufferForLights
			{
			    unsigned nrOfPointLights;
				unsigned nrOfSpotLights;
				unsigned nrOfDirectionalLights;
				unsigned pad;
			};

			struct LightStruct
			{
				thomas::math::Vector3  color;
				float   intensity;
				thomas::math::Vector3  position;
				float   spotOuterAngle;
				thomas::math::Vector3  direction;
				float   spotInnerAngle;
				thomas::math::Vector3  attenuation;
				float   pad;
			};
			
		public:	
			void Destroy();
			void AddPointLight(const LightStruct &light);
			void AddSpotLight(const LightStruct &light);
			void AddDirectionalLight(const LightStruct &light);
			
			void BindLights();
		private:
			ConstantBufferForLights lightsCounts;
			std::vector<LightStruct> pointLights;
			std::vector<LightStruct> spotLights;
			std::vector<LightStruct> directionalLights;
			std::vector<LightStruct> allLights;
			ID3D11Buffer* s_lightBuffer;
		};
	}
}