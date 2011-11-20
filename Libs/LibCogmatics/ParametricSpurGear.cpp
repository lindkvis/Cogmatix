#include "StdAfx.h"

#include "ParametricSpurGear.h"
#include "CogException.h"

namespace LibCogmatics
{
	ParametricSpurGear::ParametricSpurGear(short numberOfTeeth, Length depth, Length axisDiameter, Length module)
		: _numberOfTeeth(numberOfTeeth), _depth(depth), _axisDiameter(axisDiameter), _module(module)
	{
		EXCEPT_IF (numberOfTeeth <= 0, CogException::BadParameter, "Wrong Number of Teeth");
		EXCEPT_IF (depth < fabs(epsilon), CogException::BadParameter, "Bad depth");
EXCEPT_IF (module < fabs(epsilon), CogException::BadParameter, "Bad module");
		

		// Derived parameters
		_rootDiameter = double(_numberOfTeeth - 2) * module;
		_outsideDiameter = double(_numberOfTeeth + 2) * module;
		_pitchDiameter = double(_numberOfTeeth) * module; 
		_toothThicknessBottom = 0.5 * pi * module;
		_toothThicknessTop = 0.5 * _toothThicknessBottom; // guess

		EXCEPT_IF (_rootDiameter < fabs(epsilon), CogException::BadParameter, "Bad root diameter");
		EXCEPT_IF (axisDiameter >= _rootDiameter, CogException::BadParameter, "Axis diameter too small");

		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		//osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		setVertexArray (vertices);	
	//	setNormalArray (normals);
		//setNormalBinding(BIND_PER_PRIMITIVE);

		// Create twice as many flat edges as we need teeth (i.e. both the tooth and the space between)
		int numberOfEdges = 2*_numberOfTeeth;

		double z = -_depth.value();
		Vec O (0., 0., 0.);
		Vec Oz (0., 0., z);
		Vec NO (0., 0., 1.);
		Vec NOz (0., 0., -1.);
		// Edges (Quadstrips)
		for (short i=0; i < numberOfEdges+1; ++i)
		{
			double x0 = _rootDiameter.value() * cos (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double y0 = _rootDiameter.value() * sin (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double x1 = _rootDiameter.value() * cos (2.*pi * double ((i+1) % numberOfEdges)/numberOfEdges);
			double y1 = _rootDiameter.value() * sin (2.*pi * double ((i+1) % numberOfEdges)/numberOfEdges);

			Vec p0 (x0, y0, 0.);
			Vec p1 (x0, y0, z);
			vertices->push_back(p0);
			vertices->push_back(p1);			
			// the space in between.
			Vec p0n (x1, y1, 0.);
			Vec p1n (x1, y1, z);
			Vec n = (p1-p0) ^ (p0n-p0);
			n.normalize();
			//normals->push_back (n);

			if (i % 2 == 0)
			{
				// The tooth. Denendum part.
				Vec p2 = p0 + n * _module.value();
				Vec p3 = p1 + n * _module.value();
				Vec p8 = p0n + n * _module.value();
				Vec p9 = p1n + n * _module.value();

				// The tooth. Addendum part.
				Vec p4 = p2 + n * _module.value(); 
				Vec p5 = p3 + n * _module.value(); 
				Vec p6 = p8 + n * _module.value();
				Vec p7 = p9 + n * _module.value();

				Vec t = p6 - p4;
				p4 += t*0.25;
				p6 -= t*0.25;
				p5 += t*0.25;
				p7 -= t*0.25;
				vertices->push_back(p2);
				vertices->push_back(p3);
				vertices->push_back(p4);
				vertices->push_back(p5);
				vertices->push_back(p6);
				vertices->push_back(p7);
				vertices->push_back(p8);
				vertices->push_back(p9);
			}
			else
			{ 
			}
		}	
		int nOuterEdgeVertices = vertices->size();
		addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 0, nOuterEdgeVertices));
		// Internal edges (quadstrips)
		for (short i=0; i < numberOfEdges+1; ++i)
		{
			double x0 = _axisDiameter.value() * cos (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double y0 = _axisDiameter.value() * sin (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double x1 = _axisDiameter.value() * cos (2.*pi * double ((i+1) % numberOfEdges)/numberOfEdges);
			double y1 = _axisDiameter.value() * sin (2.*pi * double ((i+1) % numberOfEdges)/numberOfEdges);

			Vec p0 (x0, y0, 0.);
			Vec p1 (x0, y0, z);
			Vec p3 (x1, y1, 0.);
			Vec n = (p1-p0) ^ (p3-p0);
			n.normalize();
			n *= -1;
			vertices->push_back(p0);
			vertices->push_back(p1);			
//			normals->push_back (n);
		}	
		int nInnerEdgeVertices = 2*(numberOfEdges+1); 
		addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, nOuterEdgeVertices, nInnerEdgeVertices));
		// End surfaces
		for (short i=0; i < numberOfEdges+1; ++i)
		{
			double x0 = _axisDiameter.value() * cos (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double y0 = _axisDiameter.value() * sin (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double x1 = _rootDiameter.value() * cos (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double y1 = _rootDiameter.value() * sin (2.*pi * double (i % numberOfEdges)/numberOfEdges);

			Vec p0 (x0, y0, 0.);
			Vec p1 (x1, y1, 0.);
			vertices->push_back(p0);
			vertices->push_back(p1);
	//		normals->push_back(NO);
		}
		int nTopSurface = 2*(numberOfEdges+1); 
		addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, nOuterEdgeVertices+nInnerEdgeVertices, nTopSurface));
		// End surfaces
		for (short i=0; i < numberOfEdges+1; ++i)
		{
			double x0 = _axisDiameter.value() * cos (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double y0 = _axisDiameter.value() * sin (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double x1 = _rootDiameter.value() * cos (2.*pi * double (i % numberOfEdges)/numberOfEdges);
			double y1 = _rootDiameter.value() * sin (2.*pi * double (i % numberOfEdges)/numberOfEdges);

			Vec p0 (x0, y0, z);
			Vec p1 (x1, y1, z);
			vertices->push_back(p0);
			vertices->push_back(p1);
//			normals->push_back(NOz);
		} 
		int nBottomSurface = nTopSurface;
		addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, nOuterEdgeVertices+nInnerEdgeVertices+nTopSurface, nBottomSurface));
		
		int nStart=vertices->size();
	
		// End caps for the teeth
		for (short j=0; j < 2; ++j)
		{
			for (short i=0; i < numberOfEdges+1; i+=2)
			{
				double x0 = _rootDiameter.value() * cos (2.*pi * double (i % numberOfEdges)/numberOfEdges);
				double y0 = _rootDiameter.value() * sin (2.*pi * double (i % numberOfEdges)/numberOfEdges);
				double x1 = _rootDiameter.value() * cos (2.*pi * double ((i+1) % numberOfEdges)/numberOfEdges);
				double y1 = _rootDiameter.value() * sin (2.*pi * double ((i+1) % numberOfEdges)/numberOfEdges);

				Vec p0 (x0, y0, z*j);
				Vec p5 (x1, y1, z*j);

				Vec n = (Vec(x1, y1, 0) - Vec(x0, y0, 0)) ^ (Vec(x0, y0, 0) - Vec(x0, y0, z));
				n.normalize();

				// The tooth. Denendum part.
				Vec p1 = p0 + n * _module.value();
				Vec p4 = p5 + n * _module.value();

				// The tooth. Addendum part.
				Vec p2 = p1 + n * _module.value();
				Vec p3 = p4 + n * _module.value();

				Vec t = p3 - p2;
				p2 += t * 0.25;
				p3 -= t * 0.25;
				vertices->push_back(p0);
				vertices->push_back(p1);
				vertices->push_back(p2);
				vertices->push_back(p3);
				vertices->push_back(p4);
				vertices->push_back(p5);
				addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, nStart, vertices->size()-nStart));
				nStart = vertices->size();
				//normals->push_back(j == 0 ? NO : NOz);
			}
		}
	}


	ParametricSpurGear::~ParametricSpurGear(void)
	{
	}
}