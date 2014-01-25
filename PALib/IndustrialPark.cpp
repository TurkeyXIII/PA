#include "pch.h"
#include "common.h"
#include "IndustrialPark.h"


//
//	Physics logic factory implementation
//
CPhysicsLogicFactory::CPhysicsLogicFactory(void)
{
	//nothing I guess?
}

CPhysicsLogicFactory::~CPhysicsLogicFactory(void)
{
}

Ptr<IPhysicsLogic>	CPhysicsLogicFactory::Create(void)
{
	return new CPhysicsLogic();
}

//
// Rigid body factory implementation
//
CBodyFactory::CBodyFactory(void)
{
}

CBodyFactory::~CBodyFactory(void)
{
}

Ptr<IBody>	CBodyFactory::Create(const std::string* filename)
{
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLError e = doc.LoadFile(filename->c_str());

	if (!e)
	{
		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("body");
		
		const char* type = pRoot->Attribute("type");

		if (!strcmp(type, "rigid"))
		{
			CRigidBody* pBody = new CRigidBody();

			//add all the necessary hitboxes
			tinyxml2::XMLElement* pHitboxElement = pRoot->FirstChildElement("hitbox");
			while (pHitboxElement)
			{
				CHitbox* pHitbox = new CHitbox();
				
				tinyxml2::XMLElement* pVertElement = pHitboxElement->FirstChildElement("vertex");
				while (pVertElement)
				{
					D3DXVECTOR3 v;
					v.x = pVertElement->FloatAttribute("x");
					v.y = pVertElement->FloatAttribute("y");
					v.z = pVertElement->FloatAttribute("z");
					pHitbox->AddVert(v);

					pVertElement = pVertElement->NextSiblingElement("vertex");
				}

				pBody->AddHitbox(pHitbox);
				pHitboxElement = pHitboxElement->NextSiblingElement("hitbox");
			}

			tinyxml2::XMLElement* pModelElement = pRoot->FirstChildElement("model");
			if (pModelElement)
			{
				// load the 3ds file
				const char* filename = pModelElement->Attribute("filename");
				if (filename)
				{
					if (!Load3DS(pBody, filename)) 
					{
						debugf("Error: could not read 3ds file\n");
						return NULL;
					}
				}
				else
				{
					debugf("Error: model element does not contain filename\n");
					return NULL;
				}
			}

			//get the material
			tinyxml2::XMLElement* pMaterialElement = pRoot->FirstChildElement("material");
			if (pMaterialElement)
			{
				CMaterial mat;
				
				float density, youngsModulus;

				if (pMaterialElement->QueryFloatAttribute("density", &density) != tinyxml2::XML_NO_ERROR)
				{
					density = 1.0f;
				}

				if (pMaterialElement->QueryFloatAttribute("youngsmodulus", &youngsModulus) != tinyxml2::XML_NO_ERROR)
				{
					youngsModulus = 4.0f;
				}
				mat.SetDensity(density);
				mat.SetYoungsModulus(youngsModulus);
				pBody->SetMaterial(mat);
			}
			else
			{
				CMaterial mat;
				mat.SetDensity(1.0f);
				mat.SetYoungsModulus(4.0f);
				pBody->SetMaterial(mat);
			}

			return pBody;
		}
		//else if () the other types here
		else 
		{
			return NULL;
		}
		
	}
	else
	{
		//TODO: use an error log or something
		debugf("XML Error: %d\n", e);
		return NULL;
	}
}

bool		CBodyFactory::Load3DS(IBody* pBody, const char* filename)
{
	WORD chunkID;
	DWORD chunkLength;

	char objectName[256];
	long fileLength;

	debugf("Attempting to open file %s\n", filename);

	FILE* pfile;
	fopen_s(&pfile, filename, "rb");

	if (pfile)
	{
		debugf("pfile exists\n");

		fseek(pfile, 0, SEEK_END);
		fileLength = ftell(pfile);
		fseek(pfile, 0, SEEK_SET);

		while (ftell(pfile) < fileLength)
		{
			fread(&chunkID, sizeof(WORD), 1, pfile);
			fread(&chunkLength, sizeof(DWORD), 1, pfile);

			switch (chunkID)
			{
			case 0x4d4d: //main chunk
				{
					//just let it move on to the next chunk
				}
				break;

			case 0x3d3d: //3D Editor Chunk: ancestor of important data, but doen't contain much itself
				{
					//move along, nothing to see here
				}
				break;

			case 0x4000: //Object Block: parent of position, light and camera information about the object
				{					
					//contains the object name. Read it until the null terminate is found
					int i = 0;
					do
					{
						fread(&objectName[i++], 1, 1, pfile);
					} 
					while (objectName[i-1] && i < 256);
				}
				break;

			case 0x4100: //Triangular Mesh: parent of important data
				{
					//no data
				}
				break;

			case 0x4110: //Vertices List: Finally something interesting
				{
					// for now, put them all in one hitbox
					CHitbox* pHitbox = new CHitbox();
					unsigned short nVerts;
					fread(&nVerts, sizeof(unsigned short), 1, pfile);

					D3DXVECTOR3 v;

					for (int i = 0; i < nVerts; i++)
					{
						// the position vector is 3 floats long
						fread(&v.x, sizeof(float), 1, pfile);
						fread(&v.y, sizeof(float), 1, pfile);
						fread(&v.z, sizeof(float), 1, pfile);

						// the normal vector for the vert must be zeroed now so that it can be added to when averaging poly normals
						pHitbox->AddVert(v);
					}
					pBody->AddHitbox(pHitbox);
				}
				break;

			//case 0x4120: //Faces Description: normals for each polygon
			//	{
			//		
			//		if (lMesh.empty())
			//		{
			//			//haven't had case 0x4000 yet, something's gone wrong
			//			MessageBoxA(hWnd, "Error: Faces Description before object creation", "DrawableObject()", MB_OK);
			//			fclose(pfile);
			//			return;
			//		}
			//		Mesh* pMesh = lMesh.back();
			//		pMesh->ReadPolygons(pfile);
			//	}
			//	break;

			//case 0x4130: //Face material
			//	{
			//		if (lMesh.empty())
			//		{
			//			//haven't had case 0x4000 yet, something's gone wrong
			//			MessageBoxA(hWnd, "Error: Faces Description before object creation", "DrawableObject()", MB_OK);
			//			fclose(pfile);
			//			return;
			//		}
			//		Mesh* pMesh = lMesh.back();
			//		pMesh->ReadMaterial(pfile);
			//	}
			//	break;

			//case 0x4140: //mapping coordinates
			//	{

			//		if (lMesh.empty())
			//		{
			//			//haven't had case 0x4000 yet, something's gone wrong
			//			MessageBoxA(hWnd, "Error: Mapping Coordinates before object creation", "DrawableObject()", MB_OK);
			//			fclose(pfile);
			//			return;
			//		}

			//		unsigned short nVertsLocal;

			//		fread(&nVertsLocal, sizeof(unsigned short), 1, pfile);

			//		if (nVertsLocal != lMesh.back()->GetNVerts())
			//		{
			//			MessageBoxA(hWnd, "Error: Mapping Coordinates to wrong number of vertices", "DrawableObject()", MB_OK);
			//			fclose(pfile);
			//			return;
			//		}
			//		lMesh.back()->MapTexCoords(pfile);
			//	}
			//	break;

			//case 0xAFFF: //beginning of the material block
			//	{
			//		//no data
			//	}
			//	break;

			//case 0xA000: //material name
			//	{
			//		char name[MAT_NAME_MAX];
			//		int i = 0;

			//		Material* pMat = new Material();
			//		lMat.push_back(pMat);

			//		do
			//		{
			//			fread(&name[i++], 1, 1, pfile);
			//		} 
			//		while (name[i-1] && i < MAT_NAME_MAX);

			//		pMat->SetName(name);
			//	}
			//	break;

			//case 0xA010: //material ambient colour
			//	{
			//		if (lMat.empty())
			//		{
			//			MessageBoxA(hWnd, "Error: ambient color with empty material list", "DrawableObject()", MB_OK);
			//			fseek(pfile, chunkLength - 6, SEEK_CUR);
			//			break;
			//		}
			//		
			//		D3DCOLORVALUE ambColor = ReadColorChunk(pfile);
			//		lMat.back()->SetAmbient(ambColor);
			//		
			//		//there shouldn't be more than one color chunk here
			//		//if there is, it'll run into the default condition below and be skipped
			//	}
			//	break;

			//case 0xA020: //material diffuse color
			//	{
			//		if (lMat.empty())
			//		{
			//			MessageBoxA(hWnd, "Error: diffuse color with empty material list", "DrawableObject()", MB_OK);
			//			fseek(pfile, chunkLength - 6, SEEK_CUR);
			//			break;
			//		}
			//		
			//		D3DCOLORVALUE diffColor = ReadColorChunk(pfile);
			//		lMat.back()->SetDiffuse(diffColor);
			//		
			//		//there shouldn't be more than one color chunk here
			//		//if there is, it'll run into the default condition below and be skipped
			//	}

			//case 0xA200: //texture map 1 parent chunk
			//case 0xA33A: //texture map 2 parent chunk
			//	break;

			//case 0xA300: //texure filename
			//	{
			//		if (lMat.empty())
			//		{
			//			MessageBoxA(hWnd, "Error: texture filename with empty material list", "DrawableObject()", MB_OK);
			//			fseek(pfile, chunkLength - 6, SEEK_CUR);
			//			break;
			//		}

			//		//textures filenames must be in 8.3 format
			//		char texFilename[13];
			//		int i = 0;

			//		do
			//		{
			//			fread(&texFilename[i++], 1, 1, pfile);
			//		} 
			//		while (texFilename[i-1] && i < 13);


			//		lMat.back()->SetTexture(texFilename);
			//	}
			//	break;

			//case 0xA354: //V scale
			//	{
			//		if (lMat.empty())
			//		{
			//			MessageBoxA(hWnd, "Error: V scale with empty material list", "DrawableObject()", MB_OK);
			//			fseek(pfile, chunkLength - 6, SEEK_CUR);
			//			break;
			//		}

			//		float vScale;
			//		fread(&vScale, sizeof(float), 1, pfile);
			//		lMat.back()->SetVScale(vScale);

			//	}
			//	break;

			//case 0xA356: //U scale
			//	{
			//		if (lMat.empty())
			//		{
			//			MessageBoxA(hWnd, "Error: U scale with empty material list", "DrawableObject()", MB_OK);
			//			fseek(pfile, chunkLength - 6, SEEK_CUR);
			//			break;
			//		}

			//		float uScale;
			//		fread(&uScale, sizeof(float), 1, pfile);
			//		lMat.back()->SetUScale(uScale);

			//	}
			//	break;

			//case 0xA358: //U offset
			//	{
			//		if (lMat.empty())
			//		{
			//			MessageBoxA(hWnd, "Error: U offset with empty material list", "DrawableObject()", MB_OK);
			//			fseek(pfile, chunkLength - 6, SEEK_CUR);
			//			break;
			//		}

			//		float uOff;
			//		fread(&uOff, sizeof(float), 1, pfile);
			//		lMat.back()->SetUOffset(uOff);
			//	}
			//	break;

			//case 0xA35A: //U offset
			//	{
			//		if (lMat.empty())
			//		{
			//			MessageBoxA(hWnd, "Error: U offset with empty material list", "DrawableObject()", MB_OK);
			//			fseek(pfile, chunkLength - 6, SEEK_CUR);
			//			break;
			//		}

			//		float vOff;
			//		fread(&vOff, sizeof(float), 1, pfile);
			//		lMat.back()->SetVOffset(vOff);
			//	}
			//	break;

			default: //anything that's not supported, we skip over
				{
					fseek(pfile, chunkLength - 6, SEEK_CUR); //chunklength includes the chunk header, which is 6 bytes long
				}
			} // end switch (chunkID)

		} // end while (ftell(pfile) < fileLength)

		fclose(pfile);

		return true;
	} //end if(pfile)
	else
	{
		return false;
	}
}