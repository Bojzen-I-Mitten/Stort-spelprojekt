#pragma once
#include "Physics.h"
using namespace System;
using namespace System::Runtime::Serialization;
using namespace System::ComponentModel;
namespace ThomasEngine
{
	[DataContractAttribute]
	public ref class Project
	{
	private:

		String ^ m_path;
		[DataMemberAttribute]
		String ^ m_name;
		[DataMemberAttribute]
		String^ m_relativeResourcePath;
		[DataMemberAttribute]
		String^ m_relativeAssemblyPath;
		
		[DataMemberAttribute]
		String^ m_relativeCurrentScenePath;

		[DataMemberAttribute]
		System::Collections::Generic::List<Layer>^ m_layers;

	internal:
		Project() {};

	public:
		Project(String^ name, String^ absolutePath)
		{
			m_name = name;
			m_path = absolutePath + "\\" + name;
			m_relativeResourcePath = "Assets";
			m_relativeAssemblyPath = "Assembly";
			m_relativeCurrentScenePath = nullptr;
			System::IO::Directory::CreateDirectory(path);
			System::IO::Directory::CreateDirectory(assetPath);
			System::IO::Directory::CreateDirectory(assembly);
			Save();
		}

		static Project^ LoadProject(String^ projectFile) {
			DataContractSerializer^ serializer = gcnew DataContractSerializer(Project::typeid);
			Xml::XmlReader^ file = Xml::XmlReader::Create(projectFile);
			Project^ project = (Project^)serializer->ReadObject(file);
			file->Close();
			project->m_path = IO::Path::GetDirectoryName(projectFile);

			if (project)
				Physics::SetLayers(project->m_layers);

			return project;
		}

		void Save() {
			m_layers = Physics::GetLayers();

			DataContractSerializer^ serializer = gcnew DataContractSerializer(Project::typeid);
			System::IO::FileInfo^ fi = gcnew System::IO::FileInfo(path);
			fi->Directory->Create();
			Xml::XmlWriterSettings^ settings = gcnew Xml::XmlWriterSettings();
			settings->Indent = true;
			Xml::XmlWriter^ file = Xml::XmlWriter::Create(filepath, settings);
			serializer->WriteObject(file, this);
			file->Close();
		}

		property String^ filepath {
			String^ get() { return m_path + "\\" + m_name + ".thomas"; }
		}

		property String^ name
		{
			String^ get() { return m_name; }
		}
		property String^ path
		{
			String^ get() { return m_path; }
		}
		property String^ assetPath
		{
			String^ get() { return m_path + "\\" + m_relativeResourcePath; }
		}
		property String^ assembly
		{
			String^ get() { return  m_path + "\\" + m_relativeAssemblyPath; }
		}

		property String^ currentScenePath
		{
			String^ get();
			void set(String^ scene);
		}
	};
}
