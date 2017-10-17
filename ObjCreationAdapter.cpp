#include "ObjCreationAdapter.h"

#include <iostream>
#include <cstdio>


/**
 * Default constructor.
 */
ObjCreationAdapter::ObjCreationAdapter() {
}

// Local helper fun - only add edges with this please!
void ObjCreationAdapter::addL(int b, int e, std::string layerName) {
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
void ObjCreationAdapter::addV(float x, float y, float z) {
	// Add this new vertex to the end of that vector
	allVertices.push_back(ObjMaster::VertexElement{x*METER, y*METER, z*METER});

	// Increment the last vertex number
	++lastVerNo;
}

void ObjCreationAdapter::setVariableString(const std::string &key, const std::string &value, int code) {
	fprintf(stderr, "=== String variable %s=%s (code:%d)\n", key.c_str(), value.c_str(), code);
	stringVariables[key] = value;
}

void ObjCreationAdapter::setVariableVector(const std::string &key, double v1, double v2, double v3, int code){
	fprintf(stderr, "=== Vector variable %s=[%f, %f, %f] (code:%d)\n", key.c_str(), v1, v2, v3, code);
	// Handle all variables for later
	auto val = std::make_tuple(v1, v2, v3);
	vectorVariables[key] = val;

	// Handle those that are nearly always needed
	handleCommonVecVars(key, val);
}

void ObjCreationAdapter::handleCommonVecVars(const std::string &key, std::tuple<double, double, double> val) {
	if(key == "$EXTMIN") {
		// EXTMIN
		extmin = val;
		extmin_m = std::make_tuple(std::get<0>(val)*METER, std::get<1>(val)*METER, std::get<2>(val)*METER);
		fprintf(stderr, "extmin found to be: [%f, %f, %f]\n", std::get<0>(val), std::get<1>(val), std::get<2>(val));
	} else if(key == "$EXTMAX") {
		// EXTMAX
		extmax = val;
		extmax_m = std::make_tuple(std::get<0>(val)*METER, std::get<1>(val)*METER, std::get<2>(val)*METER);
		fprintf(stderr, "extmax found to be: [%f, %f, %f]\n", std::get<0>(val), std::get<1>(val), std::get<2>(val));
	}
}
	
bool ObjCreationAdapter::checkExtentsFor(float x, float y, float z) {
	// min
	auto minx = std::get<0>(extmin);
	auto miny = std::get<1>(extmin);
	auto minz = std::get<2>(extmin);
	// max
	auto maxx = std::get<0>(extmax);
	auto maxy = std::get<1>(extmax);
	auto maxz = std::get<2>(extmax);

	fprintf(stderr, ">>> DEBUG >>> minx, miny, minz = %f, %f, %f\n", minx, miny, minz);
	fprintf(stderr, ">>> DEBUG >>> maxx, maxy, maxz = %f, %f, %f\n", maxx, maxy, maxz);
	fprintf(stderr, ">>> DEBUG >>> x, y, z = %f, %f, %f\n", x, y, z);

	// true if we are in the clipped area - otherwise false
	return ((minx <= x) && (x <= maxx)) &&
		((miny <= y) && (y <= maxy)) &&
		((minz <= z) && (z <= maxz));
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

	// Check for boundaries
	if(!checkExtentsFor(data.x, data.y, data.z)) {
		fprintf(stderr, "POINT outside of extents!\n");
		return;
	}

	// Add the vertexes
	addV(data.x, data.y, data.z);

	// Handle polylines
	if(polyLineState == PolyLineState::FIRST) {
		// No line entity generation yet
		polyLineState = PolyLineState::OTHER;
	} else if(polyLineState == PolyLineState::OTHER) {
		// We should generate a line connecting the last vertex and current!
		// Add a line referencing these two vertexes in the final result
		// The two last vertices are the one we added right now above and the earlier added one with addVertex
		// TODO/FIXME: This way if there are points while the vertexes of a polyline they got part of it too (maybe ok)
		addL(lastVerNo-1, lastVerNo, attributes.getLayer());
	}
}

/**
 * Sample implementation of the method which handles line entities.
 */
void ObjCreationAdapter::addLine(const DL_LineData& data) {
	fprintf(stderr, "LINE	 (%6.3f, %6.3f, %6.3f) (%6.3f, %6.3f, %6.3f)\n",
		   data.x1, data.y1, data.z1, data.x2, data.y2, data.z2);
	printAttributes();

	// Check for boundaries
	if(!checkExtentsFor(data.x1, data.y1, data.z1) || !checkExtentsFor(data.x2, data.y2, data.z2)) {
		fprintf(stderr, "LINE outside of extents!\n");
		return;
	}

	// Add the vertexes
	fprintf(stderr, "lastVerNo - 1: %d\n", lastVerNo);
	addV(data.x1, data.y1, data.z1);
	fprintf(stderr, "lastVerNo - 2: %d\n", lastVerNo);
	addV(data.x2, data.y2, data.z2);
	fprintf(stderr, "lastVerNo - 3: %d\n", lastVerNo);
	// Add a line referencing these two vertextes in the final result
	// The two last vertices are the one we added right now above!
	addL(lastVerNo-1, lastVerNo, attributes.getLayer());
}

/**
 * Sample implementation of the method which handles arc entities.
 */
void ObjCreationAdapter::addArc(const DL_ArcData& data) {
	fprintf(stderr, "ARC		 (%6.3f, %6.3f, %6.3f) %6.3f, %6.3f, %6.3f\n",
		   data.cx, data.cy, data.cz,
		   data.radius, data.angle1, data.angle2);
	printAttributes();

	// Check for boundaries
	if(!checkExtentsFor(data.cx, data.cy, data.cz)) {
		fprintf(stderr, "ARC outside of extents!\n");
		return;
	}

	// TODO: Really bad "approximation" of arcs!!! to put a point there!
	addV(data.cx, data.cy, data.cz);
}

/**
 * Sample implementation of the method which handles circle entities.
 */
void ObjCreationAdapter::addCircle(const DL_CircleData& data) {
	fprintf(stderr, "CIRCLE	 (%6.3f, %6.3f, %6.3f) %6.3f\n",
		   data.cx, data.cy, data.cz,
		   data.radius);
	printAttributes();

	// Check for boundaries
	if(!checkExtentsFor(data.cx, data.cy, data.cz)) {
		fprintf(stderr, "CIRCLE outside of extents!\n");
		return;
	}

	// TODO: it would be nice to at least build a 4-line square where the circle is...
	addV(data.cx, data.cy, data.cz);
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

	// Check for boundaries
	if(!checkExtentsFor(data.x, data.y, data.z)) {
		fprintf(stderr, "VERTEX outside of extents!\n");
		return;
	}

	// Add the vertex
	addV(data.x, data.y, data.z);

	// Handle polylines
	if(polyLineState == PolyLineState::FIRST) {
		// No line entity generation yet
		polyLineState = PolyLineState::OTHER;
	} else if(polyLineState == PolyLineState::OTHER) {
		// We should generate a line connecting the last vertex and current!
		// Add a line referencing these two vertexes in the final result
		// The two last vertices are the one we added right now above and the earlier added one with addVertex
		// TODO/FIXME: This way if there are points while the vertexes of a polyline they got part of it too (maybe ok)
		//addL(lastVerNo-1, lastVerNo, layersLines, attributes.getLayer());
	}
}

void ObjCreationAdapter::add3dFace(const DL_3dFaceData& data) {
	fprintf(stderr, "3DFACE\n");
	for (int i=0; i<4; i++) {
		fprintf(stderr, "   corner %d: %6.3f %6.3f %6.3f\n", 
			i, data.x[i], data.y[i], data.z[i]);
		
		// Add this vertex to the list
		addV(data.x[i], data.y[i], data.z[i]);
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
	// Generate the vertices (for all layers)
	for(auto v : allVertices) {
		//printf("v %f %f %f\n", v.x, v.y, v.z);
		printf("%s\n", v.asText().c_str());
	}

	// Generate connections like faces and lines
	for(auto layerName : layerNames) {

		// print the name of the layer as object names
		printf("o %s\n", layerName.c_str());


		// Generate the lines for that layer
	   	std::vector<ObjMaster::LineElement> layerLines = layersLines[layerName];
		for(auto l : layerLines) {
			//printf("l %d %d\n", l.bVindex, l.eVindex);
			printf("%s\n", l.asText().c_str());
		}
	}
}

// vim: tabstop=4 noexpandtab shiftwidth=4 softtabstop=4
