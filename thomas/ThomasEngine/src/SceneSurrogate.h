#pragma once

namespace ThomasEngine
{
	public ref class SceneSurrogate : System::Runtime::Serialization::IDataContractSurrogate
	{
	public:
		virtual System::Type ^ GetDataContractType(System::Type ^type);
		virtual System::Object ^ GetObjectToSerialize(System::Object ^obj, System::Type ^targetType);
		virtual System::Object ^ GetDeserializedObject(System::Object ^obj, System::Type ^targetType);
		virtual System::Object ^ GetCustomDataToExport(System::Reflection::MemberInfo ^memberInfo, System::Type ^dataContractType);
		virtual System::Object ^ GetCustomDataToExport(System::Type ^clrType, System::Type ^dataContractType);
		virtual void GetKnownCustomDataTypes(System::Collections::ObjectModel::Collection<System::Type ^> ^customDataTypes);
		virtual System::Type ^ GetReferencedTypeOnImport(System::String ^typeName, System::String ^typeNamespace, System::Object ^customData);
		virtual System::CodeDom::CodeTypeDeclaration ^ ProcessImportedType(System::CodeDom::CodeTypeDeclaration ^typeDeclaration, System::CodeDom::CodeCompileUnit ^compileUnit);
	};
	[System::Runtime::Serialization::DataContractAttribute]
	public ref class SceneResource
	{
	public:
		[System::Runtime::Serialization::DataMemberAttribute]
		System::String^ path;
		SceneResource(System::String^ resourcePath) { path = resourcePath; }
	};
}