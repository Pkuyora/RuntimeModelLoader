// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "fbxsdk.h"


/**
 * 
 */
class MODELARCHIVE_API FbxLoadLibrary
{
public:

	static void ReadVertex(FbxMesh* pMesh, int ctrlPointIndex, TArray<FVector>& Vertex);

	static void ReadUV(FbxMesh* pMesh, int ctrlPointIndex, int textureUVIndex, TArray<FVector2D>& UVs);

	static void ReadNormal(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, TArray<FVector>& Normals);

	static void ReadTangent(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, FVector& Tangent);

	static void ReadColor(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, TArray<FLinearColor>& Colors);
};

