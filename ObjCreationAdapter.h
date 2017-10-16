#ifndef TEST_CREATIONCLASS_H
#define TEST_CREATIONCLASS_H

#include "dxflib/dl_creationadapter.h"
#include "ObjMaster/VertexElement.h"
#include "ObjMaster/LineElement.h"

#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <limits>

// The unit used for calculating the CAD values in meters in most cases
// TODO: This only handles common cases though but it is good for now!
#define METER 0.001f

// Represent polyline states
enum class PolyLineState {
	/** There is no polyline going on now */
   	NONE,
	/** Waiting for the first point or vertex of a polyline */
   	FIRST,
	/** Waiting for the non-first point or vertex of a polyline */
   	OTHER
};

/**
 * @orig_author Andrew Mustun
 */
class ObjCreationAdapter : public DL_CreationAdapter {
private:
	// The borders of the drawing area - useful for clipping and stuff
	std::tuple<double, double, double> extmin = std::make_tuple(std::numeric_limits<double>::min(), std::numeric_limits<double>::min(), std::numeric_limits<double>::min());
	std::tuple<double, double, double> extmax = std::make_tuple(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	
	// We are not in a polyline in the beginning of course
	PolyLineState polyLineState = PolyLineState::NONE;
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
	// String variables (key->value) mapping
	std::unordered_map<std::string, std::string> stringVariables;
	// Vector(3) variables (key->value) mapping
	std::unordered_map<std::string, std::tuple<double, double, double>> vectorVariables;

	// Local helper fun - only add edges with this please!
	void addL(int b, int e, std::unordered_map<std::string, std::vector<ObjMaster::LineElement>> &layersLines, std::string layerName);
	// Local helper fun - only add vertices with this please!
	void addV(float x, float y, float z, std::unordered_map<std::string, std::vector<ObjMaster::VertexElement>> &layersVertices, std::string layerName);
	/** Handles common vector variable saving for those we are nearly always interested in */
	void handleCommonVecVars(const std::string &key, std::tuple<double, double, double> val);
public:
	ObjCreationAdapter();

	// Variables...
	virtual void setVariableString(const std::string &key, const std::string &value, int code) override;
	virtual void setVariableVector(const std::string &key, double v1, double v2, double v3, int code) override;

	// Various drawing elements
	virtual void addLayer(const DL_LayerData& data) override;
	virtual void addPoint(const DL_PointData& data) override;
	virtual void addLine(const DL_LineData& data) override;
	virtual void addArc(const DL_ArcData& data) override;
	virtual void addCircle(const DL_CircleData& data) override;
	virtual void addPolyline(const DL_PolylineData& data) override;
	virtual void endSequence() override;	// POLY ENDS HERE (normally)
	virtual void endBlock() override;	// POLY ENDS HERE (not so normal however)
	virtual void addVertex(const DL_VertexData& data) override;
	virtual void add3dFace(const DL_3dFaceData& data) override;

	void printAttributes();

	// Writes all collected values out to sysout
	void sysOutAll();
};

#endif

// vim: tabstop=4 noexpandtab shiftwidth=4 softtabstop=4
