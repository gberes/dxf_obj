/*
 * @file test_creationclass.cpp
 */

/*****************************************************************************
**	$Id: test_creationclass.cpp 8865 2008-02-04 18:54:02Z andrew $
**
**	This is part of the dxflib library
**	Copyright (C) 2001 Andrew Mustun
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU Library General Public License as
**	published by the Free Software Foundation.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU Library General Public License for more details.
**
**	You should have received a copy of the GNU Library General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/

#include "ObjCreationAdapter.h"

#include <iostream>
#include <cstdio>


/**
 * Default constructor.
 */
ObjCreationAdapter::ObjCreationAdapter() {
}

// TODO: when I have time for it, these should go in the Objmaster as saving an Obj and here we would build the Obj and not the string maybe
void ObjCreationAdapter::sysOutAll() {
	for(auto layer : layersVertices) {
		std::string layerName = layer.first;

		// print the name of the layer
		printf("o %s\n", layerName.c_str());

		std::vector<ObjMaster::VertexElement> layerVertices = layer.second;
		//For other stuff acccess it like this: std::vector<ObjMaster::VertexElement> layerVertices = layerVertices[layerName];
		for(auto v : layerVertices) {
			printf("v %f %f %f\n", v.x*METER, v.y*METER, v.z*METER);
		}
	}
}

// Local helper fun
static inline void addV(
		float x, float y, float z,
	   	std::unordered_map<std::string, std::vector<ObjMaster::VertexElement>> &layersVertices,
		std::string layerName) {
	// Sanity check
	auto vervec = layersVertices.find(layerName);
	if(vervec == layersVertices.end()) {
		// Haven't found the layer added earlier - should not happen!
		fprintf(stderr, "WARN: layer %s has geometry (v) but was not defined earlier!", layerName.c_str());
		// Create it right now...
		layersVertices[layerName] = std::vector<ObjMaster::VertexElement>{};
	}

	// Add this new vertex to the end of that vector
	layersVertices[layerName].push_back(ObjMaster::VertexElement{x, y, z});
}

/**
 * Sample implementation of the method which handles layers.
 */
void ObjCreationAdapter::addLayer(const DL_LayerData& data) {
	fprintf(stderr, "LAYER: %s flags: %d\n", data.name.c_str(), data.flags);
	printAttributes();

	// Add an empty vector for da layerz
	layersVertices[data.name] = std::vector<ObjMaster::VertexElement>();
}

/**
 * Sample implementation of the method which handles point entities.
 */
void ObjCreationAdapter::addPoint(const DL_PointData& data) {
	fprintf(stderr, "POINT	 (%6.3f, %6.3f, %6.3f)\n",
		   data.x, data.y, data.z);
	printAttributes();

	// Add the vertexes
	addV(data.x, data.y, data.z, layersVertices, attributes.getLayer());
}

/**
 * Sample implementation of the method which handles line entities.
 */
void ObjCreationAdapter::addLine(const DL_LineData& data) {
	fprintf(stderr, "LINE	 (%6.3f, %6.3f, %6.3f) (%6.3f, %6.3f, %6.3f)\n",
		   data.x1, data.y1, data.z1, data.x2, data.y2, data.z2);
	printAttributes();
}

/**
 * Sample implementation of the method which handles arc entities.
 */
void ObjCreationAdapter::addArc(const DL_ArcData& data) {
	fprintf(stderr, "ARC		 (%6.3f, %6.3f, %6.3f) %6.3f, %6.3f, %6.3f\n",
		   data.cx, data.cy, data.cz,
		   data.radius, data.angle1, data.angle2);
	printAttributes();
}

/**
 * Sample implementation of the method which handles circle entities.
 */
void ObjCreationAdapter::addCircle(const DL_CircleData& data) {
	fprintf(stderr, "CIRCLE	 (%6.3f, %6.3f, %6.3f) %6.3f\n",
		   data.cx, data.cy, data.cz,
		   data.radius);
	printAttributes();
}


/**
 * Sample implementation of the method which handles polyline entities.
 */
void ObjCreationAdapter::addPolyline(const DL_PolylineData& data) {
	fprintf(stderr, "POLYLINE \n");
	fprintf(stderr, "flags: %d\n", (int)data.flags);
	printAttributes();
}


/**
 * Sample implementation of the method which handles vertices.
 */
void ObjCreationAdapter::addVertex(const DL_VertexData& data) {
	fprintf(stderr, "VERTEX	 (%6.3f, %6.3f, %6.3f) %6.3f\n",
		   data.x, data.y, data.z,
		   data.bulge);
	printAttributes();

	// Add the vertexes
	addV(data.x, data.y, data.z, layersVertices, attributes.getLayer());
}


void ObjCreationAdapter::add3dFace(const DL_3dFaceData& data) {
	fprintf(stderr, "3DFACE\n");
	for (int i=0; i<4; i++) {
		fprintf(stderr, "   corner %d: %6.3f %6.3f %6.3f\n", 
			i, data.x[i], data.y[i], data.z[i]);
	}
	printAttributes();
}


void ObjCreationAdapter::printAttributes() {
	fprintf(stderr, "  Attributes: Layer: %s, ", attributes.getLayer().c_str());
	fprintf(stderr, " Color: ");
	if (attributes.getColor()==256)	{
		fprintf(stderr, "BYLAYER");
	} else if (attributes.getColor()==0) {
		fprintf(stderr, "BYBLOCK");
	} else {
		fprintf(stderr, "%d", attributes.getColor());
	}
	fprintf(stderr, " Width: ");
	if (attributes.getWidth()==-1) {
		fprintf(stderr, "BYLAYER");
	} else if (attributes.getWidth()==-2) {
		fprintf(stderr, "BYBLOCK");
	} else if (attributes.getWidth()==-3) {
		fprintf(stderr, "DEFAULT");
	} else {
		fprintf(stderr, "%d", attributes.getWidth());
	}
	fprintf(stderr, " Type: %s\n", attributes.getLinetype().c_str());
}
	
// vim: tabstop=4 noexpandtab shiftwidth=4 softtabstop=4
