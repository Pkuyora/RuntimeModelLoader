// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModelArchiveBPLibrary.h"
#include "MeshData.h"
#include "Misc/FileHelper.h"
#include "FbxLoadLibrary.h"
#include "ModelArchive.h"



FbxManager* UModelArchiveBPLibrary::pFbxManager = nullptr;

UModelArchiveBPLibrary::UModelArchiveBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void UModelArchiveBPLibrary::LoadModel(const FString& FilePath, FMeshData& MeshData, bool& IsSuccess)
{
	IsSuccess = false;
	int index;
	FilePath.FindLastChar('.', index);
	FString FileType = FilePath.Right(FilePath.Len() - index - 1);
	FileType = FileType.ToLower();

	if (FileType == TEXT("obj"))
	{
		TArray<FString> Content;
		if (!FFileHelper::LoadFileToStringArray(Content, *FilePath))
			return;
		LoadObjModel(Content, MeshData, IsSuccess);
	}
	else if (FileType == TEXT("fbx"))
	{
		LoadFbxModel(FilePath, MeshData, IsSuccess);
	}
	return;
}

UProceduralMeshComponent* UModelArchiveBPLibrary::GenerateModel(AActor* Owner,const FMeshData& MeshData)
{
	UProceduralMeshComponent* NewMesh = NewObject<UProceduralMeshComponent>(Owner);
	NewMesh->RegisterComponent();

	auto & Datas = MeshData.Datas;
	for (int i = 0; i < Datas.Num(); i++)
		NewMesh->CreateMeshSection_LinearColor(i,	Datas[i].Vertices, Datas[i].Triangeles, 
													Datas[i].Normals, Datas[i].UVs, 
													Datas[i].Colors, Datas[i].Tangents, true);

	return NewMesh;
}

UProceduralMeshComponent* UModelArchiveBPLibrary::GenerateModel_SingleSection(class AActor* Owner, const FMeshData& MeshData, int Section)
{
	
	if (MeshData.Datas.Num() <= Section)
		return nullptr;

	UProceduralMeshComponent* NewMesh = NewObject<UProceduralMeshComponent>(Owner);
	NewMesh->RegisterComponent();
	auto& Datas = MeshData.Datas;

	NewMesh->CreateMeshSection_LinearColor(Section, Datas[Section].Vertices, Datas[Section].Triangeles, 
													Datas[Section].Normals, Datas[Section].UVs, 
													Datas[Section].Colors, Datas[Section].Tangents, true);
	return NewMesh;
}

void UModelArchiveBPLibrary::LoadObjModel(const TArray<FString>& InContent, FMeshData& Data, bool & IsSuccess)
{
	FSingleMeshData NewData;
	for (auto Line : InContent)
	{
		if (Line.Len() == 0)
			continue;
		TArray<FString> stringArray;
		Line.ParseIntoArray(stringArray, TEXT(" "), false);
		if (stringArray[0] == TEXT("#"))
			continue;
		else if (stringArray[0] == TEXT("v"))
		{
			TArray<float> Vertices;
			for (int i = 1; i < stringArray.Num(); i++)
			{
				//ignore the blank space
				int32 tmp;
				if (stringArray[i].FindChar(' ', tmp))
					continue;

				Vertices.Add(FCString::Atof(*stringArray[i]));
			}

			//check file format
			if (Vertices.Num() != 3)
				return;

			NewData.Vertices.Add(FVector(Vertices[0], Vertices[1], Vertices[2]));
		}
		else if (stringArray[0] == TEXT("f"))
		{
			TArray<int32> t;
			for (int i = 1; i < stringArray.Num(); i++)
			{
				//ignore the blank space
				int32 tmp;
				if (stringArray[i].FindChar(' ', tmp))
					continue;

				TArray<FString> tmpArray;
				stringArray[i].ParseIntoArray(tmpArray, TEXT("/"));
				t.Add(FCString::Atoi(*tmpArray[0]) - 1);
			}

			//check file format
			if (t.Num() < 3)
				return;

			for (int j = 0; j < t.Num() - 2; j++)
			{
				NewData.Triangeles.Add(t[j + 2]);
				NewData.Triangeles.Add(t[j + 1]);
				NewData.Triangeles.Add(t[j]);
			}
		}
		else if (stringArray[0] == TEXT("vn"))
		{
			TArray<float> Normal;
			for (int i = 1; i < stringArray.Num(); i++)
			{
				//ignore the blank space
				int32 tmp;
				if (stringArray[i].FindChar(' ', tmp))
					continue;

				Normal.Add(FCString::Atof(*stringArray[i]));
			}

			//check file format
			if (Normal.Num() != 3)
				return;

			NewData.Normals.Add(FVector(Normal[0], Normal[1], Normal[2]));
		}
		else if (stringArray[0] == TEXT("vt"))
		{
			TArray<float> UV;
			for (int i = 1; i < stringArray.Num(); i++)
			{
				//ignore the blank space
				int32 tmp;
				if (stringArray[i].FindChar(' ', tmp))
					continue;

				UV.Add(FCString::Atof(*stringArray[i]));
			}

			//check file format
			if (UV.Num() != 2)
				return;

			NewData.UVs.Add(FVector2D(UV[0], UV[1]));
		}
	}

	if (NewData.Normals.Num() == 0)
	{
		TArray<FVector> Normals;
		for (int i = 0; i < NewData.Vertices.Num(); i++)
			Normals.Add(FVector::ZeroVector);

		auto& Vertices = NewData.Vertices;
		auto& Triangeles = NewData.Triangeles;
		for (int i = 0; i + 3 < NewData.Triangeles.Num(); i += 3)
		{
			FVector	TriangeleNormal(FVector::CrossProduct((Vertices[Triangeles[i + 2]]-Vertices[Triangeles[i+1]]),
												(Vertices[Triangeles[i]] - Vertices[Triangeles[i + 2]])));
			TriangeleNormal.Normalize();
			for (int j = i; j < i + 3; j++)
				Normals[Triangeles[j]] += TriangeleNormal;
		}

		for (int i = 0; i < Vertices.Num(); i++)
		{
			Normals[i].Normalize();
			NewData.Normals.Add(-Normals[i]);
		}

	}
	Data.Datas.Add(NewData);
	IsSuccess = true;
	return;
}

void UModelArchiveBPLibrary::LoadFbxModel(const FString& InPath,FMeshData& Data, bool& IsSuccess)
{
	if (!pFbxManager)
		if (!CreateManager())
			return;
	FbxImporter* pFbxImporter = FbxImporter::Create(pFbxManager, "FbxManager");
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, ("NewScene"));

	bool importStatus = pFbxImporter->Initialize(TCHAR_TO_ANSI(*InPath), -1, pFbxManager->GetIOSettings());
	if (!importStatus)
		return;

	importStatus = pFbxImporter->Import(pFbxScene);
	if (!importStatus)
		return;

	pFbxImporter->Destroy();

	if (FbxNode* pRoot = pFbxScene->GetRootNode())
		ProcessNode(pRoot, Data, IsSuccess);
	return;
}

bool UModelArchiveBPLibrary::CreateManager()
{
	pFbxManager = FbxManager::Create();
	if (!pFbxManager)
		return false;
	FbxIOSettings* FIos = FbxIOSettings::Create(pFbxManager, IOSROOT);
	pFbxManager->SetIOSettings(FIos);
	// Load plug-ins from the executable directory
	//FbxString lExtension = "dll";
	//FbxString lPath = FbxGetApplicationDirectory();
	//Manager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());
	return true;
}

void UModelArchiveBPLibrary::ProcessNode(FbxNode* pNode, FMeshData & mData, bool& IsSuccess)
{
	
	if (pNode->GetNodeAttribute() &&
		pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eMesh &&
		FString(pNode->GetNodeAttribute()->GetName()).Find("UCX")==-1)
	{
		FSingleMeshData Data;
		FbxMesh* pMesh = pNode->GetMesh();
		int TriangleCount = pMesh->GetPolygonCount();
		int VertexCounter = 0;
		
		for (int i = 0; i < TriangleCount; i++)
		{
			Data.Triangeles.Add(VertexCounter + 2);
			Data.Triangeles.Add(VertexCounter + 1);
			Data.Triangeles.Add(VertexCounter + 0);
			for (int j = 0; j < 3; j++)
			{
				int ctrlPointIndex = pMesh->GetPolygonVertex(i, j);

				FbxLoadLibrary::ReadVertex(pMesh, ctrlPointIndex, Data.Vertices);
				// Read the color of each vertex
				FbxLoadLibrary::ReadColor(pMesh, ctrlPointIndex, VertexCounter, Data.Colors);
				// Read the UV of each vertex
				FbxLoadLibrary::ReadUV(pMesh, ctrlPointIndex, pMesh->GetTextureUVIndex(i, j), Data.UVs);
				// Read the normal of each vertex
				FbxLoadLibrary::ReadNormal(pMesh, ctrlPointIndex, VertexCounter, Data.Normals);
				// Read the tangent of each vertex
				FVector Tangent;
				FbxLoadLibrary::ReadTangent(pMesh, ctrlPointIndex, VertexCounter, Tangent);
				Data.Tangents.Add(FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
				VertexCounter++;
			}
		}
		mData.Datas.Add(Data);
		IsSuccess = true;
	}
	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		ProcessNode(pNode->GetChild(i),mData, IsSuccess);
	}
}

