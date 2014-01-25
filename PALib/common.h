//general header for files which are used often
//and also change often, so shouldn't be precompiled

#include "Material.h"
#include "Hitbox.h"
#include "Body.h"
#include "Surface.h"
#include "RigidBody.h"
#include "PhysicsLogic.h"

#include "Drawable.h"
#include "Mesh.h"
#include "Graphics.h"

void debugf(const char* format, ...);

void assert(bool test);