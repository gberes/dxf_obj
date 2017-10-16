#include "ObjCreationAdapter.h"

#include <iostream>
#include <cstdio>


/**
 * Default constructor.
 */
ObjCreationAdapter::ObjCreationAdapter() {
}

// Local helper fun - only add edges with this please!
void ObjCreationAdapter::addL(int b, int e, std::unordered_map<std::string, std::vector<ObjMaster::LineElement>> &layersLines, std::string layerName) {
	fprintf(stderr, "Added line %d, %d!\n", b, e);
	// Sanity check
	auto evec = layersLines.find(layerName);
	if(evec == layersLines.end()) {
		// Haven't found the layer added earlier - should not happen!
		fprintf(stderr, "Layer %s has geometry (l)", layerName.c_str());
		// Create it right now...
		layersLines[layerName] = std::vector<ObjMaster::LineElement>{};
	}

	// Add this new vertex to the end of that vector
	layersLines[layerName].push_back(ObjMaster::LineElement{b, e});
}

// Local helper fun - only add vertices with this please!
void ObjCreationAdapter::addV(
		float x, float y, float z,
	   	std::unordered_map<std::string, std::vector<ObjMaster::VertexElement>> &layersVertices,
		std::string layerName) {
	// Sanity check
	auto vervec = layersVertices.find(layerName);
	if(vervec == layersVertices.end()) {
		// Haven't found the layer added earlier - should not happen!
		fprintf(stderr, "WARN: layer %s has geometry (v)", layerName.c_str());
		// Create it right now...
		layersVertices[layerName] = std::vector<ObjMaster::VertexElement>{};
	}

	// Add this new vertex to the end of that vector
	layersVertices[layerName].push_back(ObjMaster::VertexElement{x*METER, y*METER, z*METER});

	// Increment the last vertex number
	++lastVerNo;
}

/**
 * Sample implementation of the method which handles layers.
 */
void ObjCreationAdapter::addLayer(const DL_LayerData& data) {
	fprintf(stderr, "LAYER: %s flags: %d\n", data.name.c_str(), data.flags);
	printAttributes();

	// Insert the name of the layer as probably there will be stuff for it
	layerNames.insert(data.name);
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

	// Handle polylines
	if(polyLineState == PolyLineState::FIRST) {
		// No line entity generation yet
		polyLineState = PolyLineState::OTHER;
	} else if(polyLineState == PolyLineState::OTHER) {
		// We should generate a line connecting the last vertex and current!
		// Add a line referencing these two vertexes in the final result
		// The two last vertices are the one we added right now above and the earlier added one with addVertex
		// TODO/FIXME: This way if there are points while the vertexes of a polyline they got part of it too (maybe ok)
		addL(lastVerNo-1, lastVerNo, layersLines, attributes.getLayer());
	}
}

/**
 * Sample implementation of the method which handles line entities.
 */
void ObjCreationAdapter::addLine(const DL_LineData& data) {
	fprintf(stderr, "LINE	 (%6.3f, %6.3f, %6.3f) (%6.3f, %6.3f, %6.3f)\n",
		   data.x1, data.y1, data.z1, data.x2, data.y2, data.z2);
	printAttributes();

	// Add the vertexes
	addV(data.x1, data.y1, data.z1, layersVertices, attributes.getLayer());
	addV(data.x2, data.y2, data.z2, layersVertices, attributes.getLayer());
	// Add a line referencing these two vertextes in the final result
	// The two last vertices are the one we added right now above!
	addL(lastVerNo-1, lastVerNo, layersLines, attributes.getLayer());
}

/**
 * Sample implementation of the method which handles arc entities.
 */
void ObjCreationAdapter::addArc(const DL_ArcData& data) {
	fprintf(stderr, "ARC		 (%6.3f, %6.3f, %6.3f) %6.3f, %6.3f, %6.3f\n",
		   data.cx, data.cy, data.cz,
		   data.radius, data.angle1, data.angle2);
	printAttributes();

	// TODO: Really bad "approximation" of arcs!!! to put a point there!
	addV(data.cx, data.cy, data.cz, layersVertices, attributes.getLayer());
}

/**
 * Sample implementation of the method which handles circle entities.
 */
void ObjCreationAdapter::addCircle(const DL_CircleData& data) {
	fprintf(stderr, "CIRCLE	 (%6.3f, %6.3f, %6.3f) %6.3f\n",
		   data.cx, data.cy, data.cz,
		   data.radius);
	printAttributes();

	// TODO: it would be nice to at least build a 4-line square where the circle is...
	addV(data.cx, data.cy, data.cz, layersVertices, attributes.getLayer());
}


/**
 * Sample implementation of the method which handles polyline entities.
 */
void ObjCreationAdapter::addPolyline(const DL_PolylineData& data) {
	fprintf(stderr, "POLYLINE \n");
	fprintf(stderr, "flags: %d\n", (int)data.flags);
	printAttributes();

	// TODO: set some random flag that the following vertices will be one polyline?
	polyLineState = PolyLineState::FIRST;
}

// POLY ENDS HERE (normally)
void ObjCreationAdapter::endSequence() {
	polyLineState = PolyLineState::NONE;
}

// POLY ENDS HERE (not so normal however)
void ObjCreationAdapter::endBlock() {
	polyLineState = PolyLineState::NONE;
}

/**
 * Sample implementation of the method which handles vertices.
 */
void ObjCreationAdapter::addVertex(const DL_VertexData& data) {
	fprintf(stderr, "VERTEX	 (%6.3f, %6.3f, %6.3f) %6.3f\n",
		   data.x, data.y, data.z,
		   data.bulge);
	printAttributes();

	// Add the vertex
	addV(data.x, data.y, data.z, layersVertices, attributes.getLayer());

	// Handle polylines
	if(polyLineState == PolyLineState::FIRST) {
		// No line entity generation yet
		polyLineState = PolyLineState::OTHER;
	} else if(polyLineState == PolyLineState::OTHER) {
		// We should generate a line connecting the last vertex and current!
		// Add a line referencing these two vertexes in the final result
		// The two last vertices are the one we added right now above and the earlier added one with addVertex
		// TODO/FIXME: This way if there are points while the vertexes of a polyline they got part of it too (maybe ok)
		addL(lastVerNo-1, lastVerNo, layersLines, attributes.getLayer());
	}
}

void ObjCreationAdapter::add3dFace(const DL_3dFaceData& data) {
	fprintf(stderr, "3DFACE\n");
	for (int i=0; i<4; i++) {
		fprintf(stderr, "   corner %d: %6.3f %6.3f %6.3f\n", 
			i, data.x[i], data.y[i], data.z[i]);
		
		// Add this vertex to the list
		addV(data.x[i], data.y[i], data.z[i], layersVertices, attributes.getLayer());
	}
	printAttributes();

	// TODO: Create a face in for the 3 or 4 vertices (at least for triangles!!!)
	// TODO: maybe always triangulate???
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

// TODO: when I have time for it, these should go in the Objmaster as saving an Obj and here we would build the Obj and not the string maybe
void ObjCreationAdapter::sysOutAll() {
	for(auto layerName : layerNames) {

		// print the name of the layer as object names
		printf("o %s\n", layerName.c_str());

		// Generate the vertices for that layer
	   	std::vector<ObjMaster::VertexElement> layerVertices = layersVertices[layerName];
		for(auto v : layerVertices) {
			//printf("v %f %f %f\n", v.x, v.y, v.z);
			printf("%s\n", v.asText().c_str());
		}

		// Generate the lines for that layer
	   	std::vector<ObjMaster::LineElement> layerLines = layersLines[layerName];
		for(auto l : layerLines) {
			//printf("l %d %d\n", l.bVindex, l.eVindex);
			printf("%s\n", l.asText().c_str());
		}
	}
}

// vim: tabstop=4 noexpandtab shiftwidth=4 softtabstop=4
