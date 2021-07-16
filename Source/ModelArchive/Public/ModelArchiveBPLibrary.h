// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MeshData.h"
#include "ModelArchiveBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
namespace fbxsdk
{
class FbxNode;
class FbxManager;
}

using namespace fbxsdk;

UCLASS()
class MODELARCHIVE_API UModelArchiveBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Mesh Data|Load", meta = (ToolTip = "load models from file(fbx|obj)", KeyWords = "Load Model Mesh"))
	static void LoadModel(const FString& FilePath, FMeshData& MeshData, bool& IsSuccess);

	UFUNCTION(BlueprintCallable, Category = "Mesh Data|Generate", meta = (ToolTip = "generate all models of data", KeyWords = "Generate Model Mesh"))
	static class UProceduralMeshComponent* GenerateModel(class AActor* Owner,const FMeshData& MeshData);

	UFUNCTION(BlueprintCallable, Category = "Mesh Data|Generate", meta = (ToolTip = "generate one model of data",KeyWords="Generate Model Mesh"))
	static class UProceduralMeshComponent* GenerateModel_SingleSection(class AActor* Owner, const FMeshData& MeshData,int Section);

	static void LoadObjModel(const TArray<FString>& InContent, FMeshData& Data, bool& IsSuccess);

	static void LoadFbxModel(const FString& InPath, FMeshData& Data, bool& IsSuccess);

	static void ProcessNode(FbxNode* pNode, FMeshData& mData, bool& IsSuccess);

	static bool CreateManager();

	static fbxsdk::FbxManager* pFbxManager;
};
