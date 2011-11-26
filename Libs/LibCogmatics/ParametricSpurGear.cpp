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
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		setVertexArray (vertices);	
		setNormalArray (normals);
		setNormalBinding(BIND_PER_VERTEX);

		// Create twice as many flat edges as we need teeth (i.e. both the tooth and the space between)
		int numberOfEdges = 2*_numberOfTeeth;
		int nStart=0;
		double z = -_depth.value();
		Vec O (0., 0., 0.);
		Vec Oz (0., 0., z);
		Vec NO (0., 0., 1.);
		Vec NOz (0., 0., -1.);
		// Edges (Quadstrips)
		addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, vertices->size(), 12*(numberOfEdges)));
		for (short i=0; i < numberOfEdges; ++i)
		{
			nStart = vertices->size();

			Vec p[12]; // 12 points in total.
			double x0 = _rootDiameter.value() * cos (2.*pi * double (i)/numberOfEdges);
			double y0 = _rootDiameter.value() * sin (2.*pi * double (i)/numberOfEdges);
			double xp1 = _rootDiameter.value() * cos (2.*pi * double (i+1)/numberOfEdges);
			double yp1 = _rootDiameter.value() * sin (2.*pi * double (i+1)/numberOfEdges);

			p[0] = Vec (x0, y0, 0.);
			p[1] = Vec (x0, y0, z);
			// the space in between.
			p[10] = Vec (xp1, yp1, 0.);
			p[11] = Vec (xp1, yp1, z);
			Vec n = (p[1]-p[0]) ^ (p[10]-p[0]);
			n.normalize();

			// The tooth. Denendum part.
			p[2] = p[0] + n * _module.value();
			p[3] = p[1] + n * _module.value();
			p[8] = p[10] + n * _module.value();
			p[9] = p[11] + n * _module.value();

			// The tooth. Addendum part.
			p[4] = p[2] + n * _module.value(); 
			p[5] = p[3] + n * _module.value(); 
			p[6] = p[8] + n * _module.value();
			p[7] = p[9] + n * _module.value();

			Vec t = p[6] - p[4];
			p[4] += t*1./3.;
			p[6] -= t*1./3.;
			p[5] += t*1./3.;
			p[7] -= t*1./3.;

			if (i % 2 == 0)
			{
				for (int j=0; j < 5; ++j)
				{
					vertices->push_back(p[2*j+0]);
					vertices->push_back(p[2*j+1]);
					vertices->push_back(p[2*j+3]);
					vertices->push_back(p[2*j+2]);
					
					// Last two vertices
					Vec n = (p[2*j+1]-p[2*j])^(p[2*j+2]-p[2*j]);
					n.normalize();
					normals->push_back(n);
					normals->push_back(n);
					normals->push_back(n);
					normals->push_back(n);
				}
			}
			else
			{ 
				double xm1 = _rootDiameter.value() * cos (2.*pi * double (i-1)/numberOfEdges);
				double ym1 = _rootDiameter.value() * sin (2.*pi * double (i-1)/numberOfEdges);
				double xp2 = _rootDiameter.value() * cos (2.*pi * double (i+2)/numberOfEdges);
				double yp2 = _rootDiameter.value() * sin (2.*pi * double (i+2)/numberOfEdges);

				Vec pm = Vec (xm1, ym1, 0);
				Vec pp = Vec (xp2, yp2, 0);

				vertices->push_back(p[0]);
				vertices->push_back(p[1]);
				vertices->push_back(p[11]);
				vertices->push_back(p[10]);

				Vec nm1 = (p[1]-p[0])^(p[0]-pm);
				Vec np1 = (p[1]-p[0])^(pp-p[10]);
				nm1.normalize();
				np1.normalize();
				Vec n1 = n + nm1;
				n1.normalize();
				Vec n2 = n + np1;
				n2.normalize();

				// Use interpolated normals
				normals->push_back(n1);
				normals->push_back(n1);
				normals->push_back(n2);
				normals->push_back(n2);
			}
		}	
		// Internal edge
		nStart = vertices->size();
		for (short i=0; i < numberOfEdges+1; ++i)
		{
			double xm1 = _axisDiameter.value() * cos (2.*pi * double (i-1)/numberOfEdges);
			double ym1 = _axisDiameter.value() * sin (2.*pi * double (i-1)/numberOfEdges);
			double x0 = _axisDiameter.value() * cos (2.*pi * double (i)/numberOfEdges);
			double y0 = _axisDiameter.value() * sin (2.*pi * double (i)/numberOfEdges);
			double xp1 = _axisDiameter.value() * cos (2.*pi * double (i+1)/numberOfEdges);
			double yp1 = _axisDiameter.value() * sin (2.*pi * double (i+1)/numberOfEdges);

			// Actual two vertices
			Vec p0 (x0, y0, 0.);
			Vec p1 (x0, y0, z);
			
			// Last two vertices
			Vec pm0 (xm1, ym1, 0.);
			Vec pm1 (xm1, ym1, z);
			// Next two vertices
			Vec pp0 (xp1, yp1, 0);
			Vec pp1 (xp1, yp1, z);
			
			Vec n1 = (p1-p0) ^ (pp1-p0);
			Vec n2 = (p1-p0) ^ (p0-pm1);

			Vec n = n1 + n2;
			n.normalize();
			n *= -1;
			vertices->push_back(p0);
			vertices->push_back(p1);			
			normals->push_back(n);
			normals->push_back(n);
			
		}	
		addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, nStart, vertices->size() - nStart));
		nStart = vertices->size();
		
		// End surfaces
		for (short j=0; j < 2; ++j)
		{
			nStart = vertices->size();
			for (short i=0; i < numberOfEdges+1; ++i)
			{
				double x0 = _axisDiameter.value() * cos (2.*pi * double (i)/numberOfEdges);
				double y0 = _axisDiameter.value() * sin (2.*pi * double (i)/numberOfEdges);
				double x1 = _rootDiameter.value() * cos (2.*pi * double (i+1)/numberOfEdges);
				double y1 = _rootDiameter.value() * sin (2.*pi * double (i+1)/numberOfEdges);

				Vec p0 (x0, y0, z*j);
				Vec p1 (x1, y1, z*j);
				vertices->push_back(p0);
				vertices->push_back(p1);
				normals->push_back(j == 0 ? NO : NOz);
				normals->push_back(j == 0 ? NO : NOz);
			}
		
			addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, nStart, vertices->size()-nStart));			
		}
		
		// End caps for the teeth
		for (short j=0; j < 2; ++j)
		{
			for (short i=0; i < numberOfEdges; i+=2)
			{
				nStart = vertices->size();
				double x0 = _rootDiameter.value() * cos (2.*pi * double (i)/numberOfEdges);
				double y0 = _rootDiameter.value() * sin (2.*pi * double (i)/numberOfEdges);
				double x1 = _rootDiameter.value() * cos (2.*pi * double (i+1)/numberOfEdges);
				double y1 = _rootDiameter.value() * sin (2.*pi * double (i+1)/numberOfEdges);

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
				p2 += t * 1./3.;
				p3 -= t * 1./3.;
				vertices->push_back(p0);
				vertices->push_back(p1);
				vertices->push_back(p2);
				vertices->push_back(p3);
				vertices->push_back(p4);
				vertices->push_back(p5);
				addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, nStart, vertices->size()-nStart));
				// Same normal six times
				normals->push_back(j == 0 ? NO : NOz);
				normals->push_back(j == 0 ? NO : NOz);
				normals->push_back(j == 0 ? NO : NOz);
				normals->push_back(j == 0 ? NO : NOz);
				normals->push_back(j == 0 ? NO : NOz);
				normals->push_back(j == 0 ? NO : NOz);
			}
		}
		
	}


	ParametricSpurGear::~ParametricSpurGear(void)
	{
	}
}