#ifndef TEST_CREATIONCLASS_H
#define TEST_CREATIONCLASS_H

#include "dxflib/dl_creationadapter.h"
#include "ObjMaster/VertexElement.h"

#include <unordered_map>

// The unit used for calculating the CAD values in meters in most cases
// TODO: This only handles common cases though but it is good for now!
#define METER 0.001

/**
 * This class takes care of the entities read from the file.
 * Usually such a class would probably store the entities.
 * this one just prints some information about them to stdout.
 *
 * @author Andrew Mustun
 */
class ObjCreationAdapter : public DL_CreationAdapter {
private:
	// TODO: all kind of mappings and stuffz
	// The layer_name -> vertices mapping
	std::unordered_map<std::string, std::vector<ObjMaster::VertexElement>> layersVertices;
public:
	ObjCreationAdapter();

	virtual void addLayer(const DL_LayerData& data);
	virtual void addPoint(const DL_PointData& data);
	virtual void addLine(const DL_LineData& data);
	virtual void addArc(const DL_ArcData& data);
	virtual void addCircle(const DL_CircleData& data);
	virtual void addPolyline(const DL_PolylineData& data);
	virtual void addVertex(const DL_VertexData& data);
	virtual void add3dFace(const DL_3dFaceData& data);

	void printAttributes();

	// Writes all collected values out to sysout
	void sysOutAll();
};

#endif

// vim: tabstop=4 noexpandtab shiftwidth=4 softtabstop=4
