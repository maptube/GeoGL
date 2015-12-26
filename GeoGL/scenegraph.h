/*
 * scenegraph.h
 *
 *  Created on: 21 May 2015
 *      Author: richard
 */

#pragma once

//separated scenegraph typedef so we don't end up with nasty circular dependencies on globe where it was originally defined

#include <vector>

//forward references
class Object3D;

////////////////////////////////////////////////////////////////

typedef std::vector<Object3D*> SceneGraphType;

////////////////////////////////////////////////////////////////

