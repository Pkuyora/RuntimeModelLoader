// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshData.h"
#include "ModelArchive.h"


FArchive& operator<<(FArchive& arch, FProcMeshTangent& Tangent)
{
	return arch << Tangent.TangentX << Tangent.bFlipTangentY;
}

FArchive& operator<<(FArchive& arch, FSingleMeshData& Data)
{
	return arch << Data.Vertices << Data.Normals << Data.UVs << Data.Triangeles << Data.Colors << Data.Tangents;
}


FArchive& operator<<(FArchive& arch, FMeshData & Data)
{
	return arch << Data.Datas;
}