#include "FbxLoadLibrary.h"
#include "fbxsdk.h"
#include "ModelArchive.h"

// Fill out your copyright notice in the Description page of Project Settings.





template<class T>
void SetValue(T* inVector, int option, FVector& outVector)
{
	outVector.X = inVector->GetDirectArray().GetAt(option)[0];
	outVector.Y = inVector->GetDirectArray().GetAt(option)[1];
	outVector.Z = inVector->GetDirectArray().GetAt(option)[2];
}

void FbxLoadLibrary::ReadVertex(FbxMesh* pMesh, int ctrlPointIndex, TArray<FVector>& Vertex)
{
	FbxVector4* pCtrlPoint = pMesh->GetControlPoints();
	Vertex.Add(FVector(pCtrlPoint[ctrlPointIndex][0],
		pCtrlPoint[ctrlPointIndex][1],
		pCtrlPoint[ctrlPointIndex][2]
	));


}

void FbxLoadLibrary::ReadUV(FbxMesh* pMesh, int ctrlPointIndex, int textureUVIndex, TArray<FVector2D>& UVs)
{
	if (pMesh->GetElementUVCount() < 1)
	{
		return;
	}

	FbxGeometryElementUV* pVertexUV = pMesh->GetElementUV(0);
	FVector2D UV;
	switch (pVertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (pVertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			UV.X = pVertexUV->GetDirectArray().GetAt(ctrlPointIndex)[0];
			UV.Y = pVertexUV->GetDirectArray().GetAt(ctrlPointIndex)[1];
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pVertexUV->GetIndexArray().GetAt(ctrlPointIndex);
			UV.X = pVertexUV->GetDirectArray().GetAt(id)[0];
			UV.Y = pVertexUV->GetDirectArray().GetAt(id)[1];
		}
		break;
		default:
			break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (pVertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			UV.X = pVertexUV->GetDirectArray().GetAt(textureUVIndex)[0];
			UV.Y = pVertexUV->GetDirectArray().GetAt(textureUVIndex)[1];
		}
		break;
		default:
			break;
		}
	}
	break;
	}
	UVs.Add(UV);
}

void FbxLoadLibrary::ReadNormal(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, TArray<FVector>& Normals)
{
	if (pMesh->GetElementNormalCount() < 1)
	{
		return;
	}

	FVector Normal;
	FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(0);
	switch (leNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (leNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			SetValue(leNormal, ctrlPointIndex, Normal);
			break;
		case FbxGeometryElement::eIndexToDirect:
			SetValue(leNormal, leNormal->GetIndexArray().GetAt(ctrlPointIndex), Normal);
			break;
		default:
			break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (leNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			SetValue(leNormal, vertexCounter, Normal);
			break;
		case FbxGeometryElement::eIndexToDirect:
			SetValue(leNormal, leNormal->GetIndexArray().GetAt(vertexCounter), Normal);
			break;
		default:
			break;
		}
	}
	break;
	}
	Normals.Add(Normal);
}

void FbxLoadLibrary::ReadTangent(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, FVector& Tangent)
{
	if (pMesh->GetElementTangentCount() < 1)
	{
		return;
	}
	FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(0);
	switch (leTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (leTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			SetValue(leTangent, ctrlPointIndex, Tangent);
			break;
		case FbxGeometryElement::eIndexToDirect:
			SetValue(leTangent, leTangent->GetIndexArray().GetAt(ctrlPointIndex), Tangent);
			break;
		default:
			break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (leTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			SetValue(leTangent, vertexCounter, Tangent);
			break;
		case FbxGeometryElement::eIndexToDirect:
			SetValue(leTangent, leTangent->GetIndexArray().GetAt(vertexCounter), Tangent);
			break;
		default:
			break;
		}
	}
	break;
	}
}

void FbxLoadLibrary::ReadColor(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, TArray<FLinearColor>& Colors)
{
	if (pMesh->GetElementVertexColorCount() < 1)
	{
		return;
	}
	FLinearColor Color;
	FbxGeometryElementVertexColor* pVertexColor = pMesh->GetElementVertexColor(0);
	switch (pVertexColor->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (pVertexColor->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			Color.R = pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mRed;
			Color.G = pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mGreen;
			Color.B = pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mBlue;
			Color.A = pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mAlpha;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pVertexColor->GetIndexArray().GetAt(ctrlPointIndex);
			Color.R = pVertexColor->GetDirectArray().GetAt(id).mRed;
			Color.G = pVertexColor->GetDirectArray().GetAt(id).mGreen;
			Color.B = pVertexColor->GetDirectArray().GetAt(id).mBlue;
			Color.A = pVertexColor->GetDirectArray().GetAt(id).mAlpha;
		}
		break;
		default:
			break;
		}
	}
	break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (pVertexColor->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			Color.R = pVertexColor->GetDirectArray().GetAt(vertexCounter).mRed;
			Color.G = pVertexColor->GetDirectArray().GetAt(vertexCounter).mGreen;
			Color.B = pVertexColor->GetDirectArray().GetAt(vertexCounter).mBlue;
			Color.A = pVertexColor->GetDirectArray().GetAt(vertexCounter).mAlpha;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pVertexColor->GetIndexArray().GetAt(vertexCounter);
			Color.R = pVertexColor->GetDirectArray().GetAt(id).mRed;
			Color.G = pVertexColor->GetDirectArray().GetAt(id).mGreen;
			Color.B = pVertexColor->GetDirectArray().GetAt(id).mBlue;
			Color.A = pVertexColor->GetDirectArray().GetAt(id).mAlpha;
		}
		break;
		default:
			break;
		}
	}
	break;
	}
	Colors.Add(Color);
}


