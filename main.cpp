// Changed from DXFLIB examples

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "dxflib/dl_dxf.h"
#include "dxflib/dl_creationadapter.h"

#include "ObjCreationAdapter.h"

void usage();
void convertToObj(char* file);
void testWriting();


/*
 * @brief Main function for DXFLib test program.
 *
 * @param argc Number of delimited items on command line,
 *		including program name.
 * @param argv Pointer to array of command line items
 *
 * @retval 0 if missing input file argument or
 *		file couldn't be opened
 * @retval 1 if file opened
 */
int main(int argc, char** argv) {

	// Check given arguments:
	if (argc<2) {
		usage();
		return 0;
	}

	convertToObj(argv[1]);

	//testWriting();

	return 0;
}



/*
 * @brief Prints error message if file name not specified as command
 * line argument.
 */
void usage() {
	std::cout << "\nUsage: dxf_obj <DXF file>\n\n";
}

void convertToObj(char* file) {
	// Load DXF file into memory:
	std::cerr << "Reading file " << file << "...\n";
	ObjCreationAdapter* creationClass = new ObjCreationAdapter();
	DL_Dxf* dxf = new DL_Dxf();
	if (!dxf->in(file, creationClass)) { // if file open failed
		std::cerr << file << " could not be opened.\n";
		return;
	}
	// Write the collected stuff as obj to sysout!
	creationClass->sysOutAll();
	// Delete stuff
	delete dxf;
	delete creationClass;
}


// Tests DXF writing
void testWriting() {
	DL_Dxf* dxf = new DL_Dxf();
	DL_Codes::version exportVersion = DL_Codes::AC1015;
	DL_WriterA* dw = dxf->out("myfile.dxf", exportVersion);
	if (dw==NULL) {
		printf("Cannot open file 'myfile.dxf' \
			   for writing.");
		// abort function e.g. with return
	}

	dxf->writeHeader(*dw);
	/*
	// int variable:
	dw->dxfString(9, "$INSUNITS");
	dw->dxfInt(70, 4);
	// real (double, float) variable:
	dw->dxfString(9, "$DIMEXE");
	dw->dxfReal(40, 1.25);
	// string variable:
	dw->dxfString(9, "$TEXTSTYLE");
	dw->dxfString(7, "Standard");
	// vector variable:
	dw->dxfString(9, "$LIMMIN");
	dw->dxfReal(10, 0.0);
	dw->dxfReal(20, 0.0);
	*/
	dw->sectionEnd();
	dw->sectionTables();
	dxf->writeVPort(*dw);

	dw->tableLinetypes(3);
	dxf->writeLinetype(*dw, DL_LinetypeData("BYBLOCK", "BYBLOCK", 0, 0, 0.0));
	dxf->writeLinetype(*dw, DL_LinetypeData("BYLAYER", "BYLAYER", 0, 0, 0.0));
	dxf->writeLinetype(*dw, DL_LinetypeData("CONTINUOUS", "Continuous", 0, 0, 0.0));
	dw->tableEnd();

	int numberOfLayers = 3;
	dw->tableLayers(numberOfLayers);

	dxf->writeLayer(*dw,
				   DL_LayerData("0", 0),
				   DL_Attributes(
					   std::string(""),		 // leave empty
					   DL_Codes::black,		   // default color
					   100,					 // default width
					   "CONTINUOUS", 1.0));		  // default line style

	dxf->writeLayer(*dw,
				   DL_LayerData("mainlayer", 0),
				   DL_Attributes(
					   std::string(""),
					   DL_Codes::red,
					   100,
					   "CONTINUOUS", 1.0));

	dxf->writeLayer(*dw,
				   DL_LayerData("anotherlayer", 0),
				   DL_Attributes(
					   std::string(""),
					   DL_Codes::black,
					   100,
					   "CONTINUOUS", 1.0));

	dw->tableEnd();

	dw->tableStyle(1);
	dxf->writeStyle(*dw, DL_StyleData("standard", 0, 2.5, 1.0, 0.0, 0, 2.5, "txt", ""));
	dw->tableEnd();

	dxf->writeView(*dw);
	dxf->writeUcs(*dw);

	dw->tableAppid(1);
	dxf->writeAppid(*dw, "ACAD");
	dw->tableEnd();

	dxf->writeDimStyle(*dw, 1, 1, 1, 1, 1);

	dxf->writeBlockRecord(*dw);
	dxf->writeBlockRecord(*dw, "myblock1");
	dxf->writeBlockRecord(*dw, "myblock2");
	dw->tableEnd();

	dw->sectionEnd();

	dw->sectionBlocks();
	dxf->writeBlock(*dw, DL_BlockData("*Model_Space", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Model_Space");
	dxf->writeBlock(*dw, DL_BlockData("*Paper_Space", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Paper_Space");
	dxf->writeBlock(*dw, DL_BlockData("*Paper_Space0", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Paper_Space0");

	dxf->writeBlock(*dw, DL_BlockData("myblock1", 0, 0.0, 0.0, 0.0));
	// ...
	// write block entities e.g. with dxf->writeLine(), ..
	// ...
	dxf->writeEndBlock(*dw, "myblock1");

	dxf->writeBlock(*dw, DL_BlockData("myblock2", 0, 0.0, 0.0, 0.0));
	// ...
	// write block entities e.g. with dxf->writeLine(), ..
	// ...
	dxf->writeEndBlock(*dw, "myblock2");

	dw->sectionEnd();
	dw->sectionEntities();

	// write all entities in model space:
	dxf->writePoint(
		*dw,
		DL_PointData(10.0,
					 45.0,
					 0.0),
		DL_Attributes("mainlayer", 256, -1, "BYLAYER", 1.0));

	dxf->writeLine(
		*dw,
		DL_LineData(25.0,	// start point
					30.0,
					0.0,
					100.0,	 // end point
					120.0,
					0.0),
		DL_Attributes("mainlayer", 256, -1, "BYLAYER", 1.0));

	dw->sectionEnd();

	dxf->writeObjects(*dw);
	dxf->writeObjectsEnd(*dw);

	dw->dxfEOF();
	dw->close();
	delete dw;
	delete dxf;
}

// vim: tabstop=4 noexpandtab shiftwidth=4 softtabstop=4
