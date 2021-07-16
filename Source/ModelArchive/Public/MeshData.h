// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "MeshData.generated.h"




/**
 * 
 */
USTRUCT(Blueprintable, Category = "Mesh Data")
struct MODELARCHIVE_API FSingleMeshData
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		TArray<FVector> Vertices;

	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> Normals;

	UPROPERTY(BlueprintReadWrite)
		TArray<FVector2D> UVs;

	UPROPERTY(BlueprintReadWrite)
		TArray<int32> Triangeles;

	//UPROPERTY(BlueprintReadWrite)
		TArray<FProcMeshTangent> Tangents;

	UPROPERTY(BlueprintReadWrite)
		TArray<FLinearColor> Colors;

	friend FArchive& operator<<(FArchive& arch, FSingleMeshData& Data);

};



USTRUCT(Blueprintable, Category = "Mesh Data")
struct MODELARCHIVE_API FMeshData
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		TArray<FSingleMeshData> Datas;

	friend FArchive& operator<<(FArchive& arch, FMeshData& Data);

};

FArchive& operator<<(FArchive& arch, FProcMeshTangent& Tangent);
