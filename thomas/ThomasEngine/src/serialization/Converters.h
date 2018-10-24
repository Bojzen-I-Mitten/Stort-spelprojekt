#pragma once

using namespace Newtonsoft::Json;
using namespace Newtonsoft::Json::Linq;
namespace ThomasEngine
{
	ref class ResourceConverter : JsonConverter
	{
	public:
		// Inherited via JsonConverter
		virtual void WriteJson(Newtonsoft::Json::JsonWriter ^writer, System::Object ^value, Newtonsoft::Json::JsonSerializer ^serializer) override;
		virtual System::Object ^ ReadJson(Newtonsoft::Json::JsonReader ^reader, System::Type ^objectType, System::Object ^existingValue, Newtonsoft::Json::JsonSerializer ^serializer) override;
		virtual bool CanConvert(System::Type ^objectType) override;
	};

	ref class PrefabConverter : JsonConverter
	{
	private:
		int m_skipWrite;
	public:
		property bool CanWrite
		{
			bool get() override;
		}
		PrefabConverter();
		// Inherited via JsonConverter
		virtual void WriteJson(Newtonsoft::Json::JsonWriter ^writer, System::Object ^value, Newtonsoft::Json::JsonSerializer ^serializer) override;
		virtual System::Object ^ ReadJson(Newtonsoft::Json::JsonReader ^reader, System::Type ^objectType, System::Object ^existingValue, Newtonsoft::Json::JsonSerializer ^serializer) override;
		virtual bool CanConvert(System::Type ^objectType) override;
	};

	ref class ComponentConverter : JsonConverter
	{
	public:
		property bool CanWrite
		{
			bool get() override { return false; }
		}
		
		virtual System::Object ^ ReadJson(Newtonsoft::Json::JsonReader ^reader, System::Type ^objectType, System::Object ^existingValue, Newtonsoft::Json::JsonSerializer ^serializer) override;
		virtual bool CanConvert(System::Type ^objectType) override;

		// Inherited via JsonConverter
		virtual void WriteJson(Newtonsoft::Json::JsonWriter ^writer, System::Object ^value, Newtonsoft::Json::JsonSerializer ^serializer) override;
	};
}
