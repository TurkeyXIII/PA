#include "pch.h"
#include "common.h"
#include "Hitbox.h"

using namespace std;

CHitbox::CHitbox(void)
{
	m_lBoundingVert.clear();
	m_lFace.clear();
}

CHitbox::~CHitbox(void)
{
}

float	CHitbox::GetMomentOfInertia(const PAVECTOR* pAxis)
{
	return 0.0f;
}

bool	CHitbox::Contains(const PAVECTOR* pPoint)
{
	return Contains(&DXVector(*pPoint));
}

bool	CHitbox::Contains(const D3DXVECTOR3* pPoint)
{
	// algorithm for if pPoint is bounded by the points described by m_lBoundingVert
//	debugf("pPoint: (%f, %f, %f)\n", pPoint->x, pPoint->y, pPoint->z);

	bool zAbove = false;
	bool zBelow = false;
	for (list<Face>::iterator it = m_lFace.begin(); it != m_lFace.end(); it++)
	{
		bool yAbove = false;
		bool yBelow = false;
		for (int i = 0; i < 3; i++)
		{
			D3DXVECTOR3 *pVert1, *pVert2;
			pVert1 = (*it).m_apVert[i];
			pVert2 = (*it).m_apVert[(i+1)%3];

			if (pPoint->x >= min(pVert1->x, pVert2->x) &&
				pPoint->x <= max(pVert1->x, pVert2->x))
			{
				//we are within x, now check y

				if (pVert1->x == pVert2->x)
				{
					//the line is parallel to y
					if (max(pVert1->y, pVert2->y) >= pPoint->y) yAbove = true;
					if (min(pVert1->y, pVert2->y) <= pPoint->y) yBelow = true;
				}
				else
				{
					float gradient = (pVert1->y - pVert2->y) / (pVert1->x - pVert2->x);
					//what is the y value of the edge at pPoint->x?
					float y = pVert1->y + gradient * (pPoint->x - pVert1->x);

					if (y >= pPoint->y) yAbove = true;
					if (y <= pPoint->y) yBelow = true;
				}

				if (yAbove && yBelow) break; //out of for (int i) loop
			}
		}
		
		if (yAbove && yBelow)
		{
			// the point is within this face in both x and y directions, now check z
			// find the normal to the plane
			D3DXVECTOR3 vec1, vec2, normal;
			D3DXVECTOR3* r0 = (*it).m_apVert[0];
			vec1 = *(*it).m_apVert[1] - *r0;
			vec2 = *(*it).m_apVert[2] - *r0;
			D3DXVec3Cross(&normal, &vec1, &vec2);

			// z at the (x, y) of pPoint can be found with n . (r - r0)
			float z = r0->z - (normal.x * (pPoint->x - r0->x) + normal.y * (pPoint->y - r0->y)) / normal.z;

			if (z >= pPoint->z) zAbove = true;
			if (z <= pPoint->z) zBelow = true;
		}

		if (zAbove && zBelow) return true;
	}

	return false;
}

void	CHitbox::AddVert(const D3DXVECTOR3& v)
{
	// if this vert is entirely bounded by the hitbox already, adding it won't increase the convex shape any
	if (Contains(&v)) return;

	// this list 'owns' the vector, which should be deleted when the list is cleared/deleted
	m_lBoundingVert.push_front(v);

	// fewer than 3 verts -> there are no faces yet
	if (m_lBoundingVert.size() < 3) return;

	// special case: we now have just enough to create a double-sided polyhedron with zero volume
	if (m_lBoundingVert.size() == 3)
	{
		assert(m_lFace.size() == 0);

		list<D3DXVECTOR3>::iterator it = m_lBoundingVert.begin();

		Face face;
		for (int i = 0; i < 3; i++)
		{
			face.m_apVert[i] = &(*(it++));
		}
		face.CalculateNormal();

		// copy it to the list
		m_lFace.push_front(face);
		// and get it's iterator
		list<Face>::iterator faceIt = m_lFace.begin();

		// flip the original so it's 'facing' the other direction
		D3DXVECTOR3* temp = face.m_apVert[1];
		face.m_apVert[1] = face.m_apVert[2];
		face.m_apVert[2] = temp;
		face.m_normal = -face.m_normal;

		// link this face to the first one
		for (int i = 0; i < 3; i++)
		{
			face.m_aitAdjFace[i] = faceIt;
		}

		m_lFace.push_front(face);

		// now link the first face to the new one
		for (int i = 0; i < 3; i++)
		{
			(*faceIt).m_aitAdjFace[i] = m_lFace.begin();
		}

		return;
	}

	// this is a pointer to the vector in the list, to be added to the FACE object
	D3DXVECTOR3* pNewVert = &(*(m_lBoundingVert.begin()));

	// general case: we have a polyhedron with at least 2 faces already, and need to extend it
	{
		// find the closest face; we'll split it into three
		list<Face>::iterator faceItOld;
		float maxQuality = -FLT_MAX;

		for (list<Face>::iterator it = m_lFace.begin(); it != m_lFace.end(); it++)
		{
			if ((*it).m_valid)
			{
				D3DXVECTOR3 facePos(0.0f, 0.0f, 0.0f);
				for (int i = 0; i < 3; i++)
				{
					facePos += *(*it).m_apVert[i];
				}
				facePos /= 3;
				D3DXVECTOR3 pointDirection = v - facePos;

				float quality = D3DXVec3Dot(&(*it).m_normal, &pointDirection);

				if (quality > maxQuality)
				{
					faceItOld = it;
					maxQuality = quality;
				}
			}
		}

		// debugging
		/*
		debugf("Replacing face ");
		for (int i = 0; i < 3; i++)
		{
			debugf("(%f, %f, %f)\t", (*faceItOld).m_apVert[i]->x, (*faceItOld).m_apVert[i]->y, (*faceItOld).m_apVert[i]->z);
		}
		debugf("\n");
		*/

		Face face[3];

		for (int i = 0; i < 3; i++)
		{
			face[i].m_apVert[0] = (*faceItOld).m_apVert[i];
			face[i].m_apVert[1] = (*faceItOld).m_apVert[(i+1)%3];
			face[i].m_apVert[2] = pNewVert;

			face[i].CalculateNormal();
			
			// link the new face to the one that the old face was adjacent to
			face[i].m_aitAdjFace[0] = (*faceItOld).m_aitAdjFace[i];
			//and back again: find the old face in the adjacent face's adjacency list and replace it with the new face
			m_lFace.push_front(face[i]);
			list<Face>::iterator faceItNew = m_lFace.begin();
			(*face[i].m_aitAdjFace[0]).ReplaceAdjFace(faceItOld, faceItNew);
			
		}

		m_lFace.erase(faceItOld);

		//arrange these in reverse order, because the third face was pushed onto the list last
		list<Face>::iterator faceIt[3];
		faceIt[0] = m_lFace.begin();
		faceIt[2] = faceIt[0]++;
		faceIt[1] = faceIt[0]++;

		for (int i = 0; i < 3; i++)
		{
			(*faceIt[i]).m_aitAdjFace[1] = faceIt[(i+1)%3];
			(*faceIt[i]).m_aitAdjFace[2] = faceIt[(i+2)%3];
		}

		//debugging
		/*
		debugf("Shape before fixing:\n");
		for (list<Face>::iterator it = m_lFace.begin(); it != m_lFace.end(); it++)
		{
			debugf("Face:\t");
			for (int i = 0; i < 3; i++)
			{
				debugf("(%f, %f, %f)\t", (*it).m_apVert[i]->x, (*it).m_apVert[i]->y, (*it).m_apVert[i]->z);
			}
			debugf("\n");
		}
		*/

		for (int i = 0; i < 3; i++)
		{
			FixFace(faceIt[i]);
		}

		for (list<Face>::iterator it = m_lFace.begin(); it != m_lFace.end();)
		{
			if (!(*it).m_valid) 
			{
				list<Face>::iterator oldIt = it;
				it++;
				m_lFace.erase(oldIt);
			}
			else
			{
				it++;
			}
		}
		
		//debugging
		/*
		debugf("Whole shape:\n");
		for (list<Face>::iterator it = m_lFace.begin(); it != m_lFace.end(); it++)
		{
			debugf("Face:\t");
			for (int i = 0; i < 3; i++)
			{
				debugf("(%f, %f, %f)\t", (*it).m_apVert[i]->x, (*it).m_apVert[i]->y, (*it).m_apVert[i]->z);
			}
			debugf("\n");
		}
		*/
		
	}
}

void	CHitbox::Face::CalculateNormal()
{
	D3DXVECTOR3 vec1 = *m_apVert[2] - *m_apVert[0];
	D3DXVECTOR3 vec2 = *m_apVert[1] - *m_apVert[0];
	D3DXVECTOR3 normal;
	D3DXVec3Cross(&normal, &vec1, &vec2);
	D3DXVec3Normalize(&m_normal, &normal);
}

void	CHitbox::Face::ReplaceAdjFace(std::list<Face>::iterator current, std::list<Face>::iterator replacement)
{
	for (int i = 0; i < 3; i++)
	{
		if (m_aitAdjFace[i] == current)
		{
			m_aitAdjFace[i] = replacement;
			return;
		}
	}
	assert(false);
}

void	CHitbox::FixFace(list<Face>::iterator thisFaceIt)
{
	if (!(*thisFaceIt).m_valid) return;

	D3DXVECTOR3 cross, dot, edge;
	bool isBadEdge[3] = {0};
	int nBadEdges = 0;
	for (int i = 0; i < 3; i++)
	{
		edge = *(*thisFaceIt).m_apVert[(i+1)%3] - *(*thisFaceIt).m_apVert[i];
		D3DXVec3Cross(&cross, &((*(*thisFaceIt).m_aitAdjFace[i]).m_normal), &edge);
		if (D3DXVec3Dot(&cross, &(*thisFaceIt).m_normal) > 0.00001f)	//must be slightly above zero to allow for floating point imprecision
		{
			isBadEdge[i] = true;
			nBadEdges++;
		}
	}

	// debugging
	/*
	debugf("Checking Face:\t");
	for (int i = 0; i < 3; i++)
	{
		debugf("(%f, %f, %f)\t", (*thisFaceIt).m_apVert[i]->x, (*thisFaceIt).m_apVert[i]->y, (*thisFaceIt).m_apVert[i]->z);
	}
	debugf("\n");
	*/

	// need to detect the case where there are two bad edges but they aren't adjacent to each other
	// fix only one as though it were a single, then let the recursion handle the other
	if (nBadEdges == 2)
	{
		int i = 0;
		list<Face>::iterator badFaceIt[2];
		for (int j = 0; j < 3; j++)
		{
			if (isBadEdge[j]) badFaceIt[i++] = (*thisFaceIt).m_aitAdjFace[j];
		}
		if (!(*badFaceIt[0]).IsAdjTo(badFaceIt[1])) nBadEdges = 1;
	}

	switch (nBadEdges)
	{
	case 0:
		//debugf("OK\n");
		return;
	case 1:
		{
			list<Face>::iterator badFaceIt;
			int i;
			for (i = 0; i < 3; i++)
			{
				if (isBadEdge[i]) 
				{
					badFaceIt = (*thisFaceIt).m_aitAdjFace[i];
					break;
				}
			}

			int j;
			for (j = 0; j < 3; j++)
			{
				if ((*badFaceIt).m_aitAdjFace[j] == thisFaceIt) break;
			}
			// debugging
			/*
			debugf("single angle with face:\t");
			for (int i = 0; i < 3; i++)
			{
				debugf("(%f, %f, %f)\t", (*badFaceIt).m_apVert[i]->x, (*badFaceIt).m_apVert[i]->y, (*badFaceIt).m_apVert[i]->z);
			}
			debugf("\n");
			*/

			Face newFace0, newFace1;
			// assign the vertices of the new faces
			newFace0.m_apVert[0] = (*thisFaceIt).m_apVert[(i+2)%3];
			newFace1.m_apVert[0] = (*thisFaceIt).m_apVert[(i+2)%3];

			newFace0.m_apVert[1] = (*badFaceIt).m_apVert[(j+2)%3];
			newFace1.m_apVert[2] = (*badFaceIt).m_apVert[(j+2)%3];
			
			newFace0.m_apVert[2] = (*thisFaceIt).m_apVert[(i+1)%3];
			newFace1.m_apVert[1] = (*thisFaceIt).m_apVert[i];

			newFace0.CalculateNormal();
			newFace1.CalculateNormal();

			// push the new faces onto the list and get the iterator for each
			m_lFace.push_front(newFace0);
			list<Face>::iterator newFaceIt0 = m_lFace.begin();

			m_lFace.push_front(newFace1);
			list<Face>::iterator newFaceIt1 = m_lFace.begin();

			// now crosslink the new faces with their adjacent faces
			(*newFaceIt0).m_aitAdjFace[0] = newFaceIt1;
			(*newFaceIt1).m_aitAdjFace[2] = newFaceIt0;

			list<Face>::iterator tempFaceIt = (*thisFaceIt).m_aitAdjFace[(i+1)%3];
			(*newFaceIt0).m_aitAdjFace[2] = tempFaceIt;
			(*tempFaceIt).ReplaceAdjFace(thisFaceIt, newFaceIt0);

			tempFaceIt = (*badFaceIt).m_aitAdjFace[(j+2)%3];
			(*newFaceIt0).m_aitAdjFace[1] = tempFaceIt;
			(*tempFaceIt).ReplaceAdjFace(badFaceIt, newFaceIt0);

			tempFaceIt = (*badFaceIt).m_aitAdjFace[(j+1)%3];
			(*newFaceIt1).m_aitAdjFace[1] = tempFaceIt;
			(*tempFaceIt).ReplaceAdjFace(badFaceIt, newFaceIt1);

			tempFaceIt = (*thisFaceIt).m_aitAdjFace[(i+2)%3];
			(*newFaceIt1).m_aitAdjFace[0] = tempFaceIt;
			(*tempFaceIt).ReplaceAdjFace(thisFaceIt, newFaceIt1);

			// finally, remove the old redundant faces from the list
			(*thisFaceIt).m_valid = false;
			(*badFaceIt).m_valid = false;

			// debugging
			/*
			debugf("split into\n");
			debugf("newFace0:\t");
			for (int i = 0; i < 3; i++)
			{
				debugf("(%f, %f, %f)\t", (*newFaceIt0).m_apVert[i]->x, (*newFaceIt0).m_apVert[i]->y, (*newFaceIt0).m_apVert[i]->z);
			}
			debugf("\n");
			debugf("newFace1:\t");
			for (int i = 0; i < 3; i++)
			{
				debugf("(%f, %f, %f)\t", (*newFaceIt1).m_apVert[i]->x, (*newFaceIt1).m_apVert[i]->y, (*newFaceIt1).m_apVert[i]->z);
			}
			debugf("\n");
			*/

			FixFace(newFaceIt0);
			FixFace(newFaceIt1);
		}
		return;
	case 2:
		{
			// replace the three faces with a single face covering all of them
			Face newFace;
			int firstBadEdge;
			list<Face>::iterator badFaceIt[3];
			badFaceIt[0] = thisFaceIt;
			if (isBadEdge[0] && isBadEdge[1]) 
			{
				firstBadEdge = 0;
			}
			else if (isBadEdge[1] && isBadEdge[2]) 
			{
				firstBadEdge = 1;
			}
			else if (isBadEdge[2] && isBadEdge[0])
			{
				firstBadEdge = 2;
			}

			badFaceIt[1] = (*thisFaceIt).m_aitAdjFace[firstBadEdge];
			badFaceIt[2] = (*thisFaceIt).m_aitAdjFace[(firstBadEdge+1)%3];

			// debugging
			/*
			debugf("Double angle with:\n");
			debugf("badFace[1]:\t");
			for (int i = 0; i < 3; i++)
			{
				debugf("(%f, %f, %f)\t", (*badFaceIt[1]).m_apVert[i]->x, (*badFaceIt[1]).m_apVert[i]->y, (*badFaceIt[1]).m_apVert[i]->z);
			}
			debugf("\n");
			debugf("badFace[2]:\t");
			for (int i = 0; i < 3; i++)
			{
				debugf("(%f, %f, %f)\t", (*badFaceIt[2]).m_apVert[i]->x, (*badFaceIt[2]).m_apVert[i]->y, (*badFaceIt[2]).m_apVert[i]->z);
			}
			debugf("\n");
			*/

			int i;
			for (i = 0; i < 3; i++)
			{
				if ((*badFaceIt[1]).m_aitAdjFace[i] == thisFaceIt) break;
			}
			int j;
			for (j = 0; j < 3; j++)
			{
				if ((*badFaceIt[2]).m_aitAdjFace[j] == thisFaceIt) break;
			}

			newFace.m_apVert[0] = (*thisFaceIt).m_apVert[firstBadEdge];
			newFace.m_apVert[2] = (*thisFaceIt).m_apVert[(firstBadEdge+2)%3];
			newFace.m_apVert[1] = (*badFaceIt[1]).m_apVert[(i+2)%3];

			newFace.CalculateNormal();

			m_lFace.push_front(newFace);
			list<Face>::iterator newFaceIt = m_lFace.begin();

			list<Face>::iterator tempFaceIt = (*thisFaceIt).m_aitAdjFace[(firstBadEdge+2)%3];
			assert(tempFaceIt != badFaceIt[1]);
			assert(tempFaceIt != badFaceIt[2]);
			(*newFaceIt).m_aitAdjFace[2] = tempFaceIt;
			(*tempFaceIt).ReplaceAdjFace(thisFaceIt, newFaceIt);

			tempFaceIt = (*badFaceIt[1]).m_aitAdjFace[(i+1)%3];
			assert(tempFaceIt != badFaceIt[0]);
			assert(tempFaceIt != badFaceIt[2]);
			(*newFaceIt).m_aitAdjFace[0] = tempFaceIt;
			(*tempFaceIt).ReplaceAdjFace(badFaceIt[1], newFaceIt);

			tempFaceIt = (*badFaceIt[2]).m_aitAdjFace[(j+2)%3];
			assert(tempFaceIt != badFaceIt[0]);
			assert(tempFaceIt != badFaceIt[1]);
			(*newFaceIt).m_aitAdjFace[1] = tempFaceIt;
			(*tempFaceIt).ReplaceAdjFace(badFaceIt[2], newFaceIt);

			// debugging
			/*
			debugf("Covering with:\n");
			debugf("newFace:\t");
			for (int i = 0; i < 3; i++)
			{
				debugf("(%f, %f, %f)\t", (*newFaceIt).m_apVert[i]->x, (*newFaceIt).m_apVert[i]->y, (*newFaceIt).m_apVert[i]->z);
			}
			debugf("\n");
			*/

			for (int k = 0; k < 3; k++)
			{
				(*badFaceIt[k]).m_valid = false;
			}

			FixFace(newFaceIt);
		}
		return;
	case 3:
	default:
		assert(false);
	}
	return;
}

bool	CHitbox::Face::IsAdjTo(list<Face>::iterator test)
{
	for (int i = 0; i < 3; i++)
	{
		if (m_aitAdjFace[i] == test) return true;
	}
	return false;
}