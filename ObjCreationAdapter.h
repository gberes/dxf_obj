#ifndef TEST_CREATIONCLASS_H
#define TEST_CREATIONCLASS_H

#include "dxflib/dl_creationadapter.h"
#include "ObjMaster/VertexElement.h"
#include "ObjMaster/LineElement.h"

#include <unordered_map>
#include <unordered_set>

// The unit used for calculating the CAD values in meters in most cases
// TODO: This only handles common cases though but it is good for now!
#define METER 0.001f

/**
 * This class takes care of the entities read from the file.
 * Usually such a class would probably store the entities.
 * this one just prints some information about them to stdout.
 *
 * @author Andrew Mustun
 */
class ObjCreationAdapter : public DL_CreationAdapter {
private:
	// Always contains the latest vertex number (across all layers)
	// Necessary for lines and faces for proper indexing. Zero means there are no vertices yet, and 1 means the last vertex is the first one!
	int lastVerNo = 0;
	// All kind of mappings and stuffz
	// All layer names
	std::unordered_set<std::string> layerNames;
	// The layer_name -> vertices mapping
	std::unordered_map<std::string, std::vector<ObjMaster::VertexElement>> layersVertices;
	// The layer_name -> lines mapping
	std::unordered_map<std::string, std::vector<ObjMaster::LineElement>> layersLines;

	// Local helper fun - only add edges with this please!
	void addL(int b, int e, std::unordered_map<std::string, std::vector<ObjMaster::LineElement>> &layersLines, std::string layerName);
	// Local helper fun - only add vertices with this please!
	void addV(float x, float y, float z, std::unordered_map<std::string, std::vector<ObjMaster::VertexElement>> &layersVertices, std::string layerName);
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
