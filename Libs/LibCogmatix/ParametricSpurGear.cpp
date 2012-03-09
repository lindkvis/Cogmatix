#include "StdAfx.h"

#include "ParametricSpurGear.h"
#include "CogException.h"

namespace LibCogmatix
{
    
    ParametricSpurGear::GearMap ParametricSpurGear::s_Gears;
    
    ParametricSpurGear::Ptr ParametricSpurGear::Create (short numberOfTeeth, double depth,
                                                        double axisDiameter, double module, double helix, double pitch_angle)
    {
        GearParameters params (numberOfTeeth, depth, axisDiameter, module, helix, pitch_angle);
        GearMap::iterator i = s_Gears.find(params);
        if (i == s_Gears.end())
        {
            Ptr gear = ParametricSpurGear::Ptr (new ParametricSpurGear(params));
            s_Gears[params] = gear;
            return gear;
        }
        else
            return i->second;
    }
    
    ParametricSpurGear::ParametricSpurGear(const GearParameters& p)
    : params (p)
    {
        EXCEPT_IF(params._numberOfTeeth <= 0, CogException::BadParameter,
                  "Wrong Number of Teeth");
        EXCEPT_IF(params._depth < fabs(EPSILON), CogException::BadParameter, "Bad depth");
        EXCEPT_IF(params._module < fabs(EPSILON), CogException::BadParameter, "Bad module");
        
        // Derived parameters
        _rootDiameter = double(params._numberOfTeeth - 2) * params._module;
        _outsideDiameter = double(params._numberOfTeeth + 2) * params._module;
        _pitchDiameter = double(params._numberOfTeeth) * params._module;
        _toothThicknessBottom = 0.5 * PI * params._module;
        _toothThicknessTop = 0.5 * _toothThicknessBottom; // guess
        
        double rootRadius = _rootDiameter/2.;
        double axisRadius = params._axisDiameter/2.;
        
        double zrootRadius = rootRadius;
        if (fabs(fabs(params._pitch_angle) - PI/2.) > EPSILON) // make sure it isn't PI/2, as tan() will be invalid
        {
            zrootRadius = rootRadius - params._depth / tan (params._pitch_angle);
        }
        
        EXCEPT_IF(_rootDiameter < fabs(EPSILON), CogException::BadParameter,
                  "Bad root diameter");
        EXCEPT_IF(params._axisDiameter >= _rootDiameter, CogException::BadParameter,
                  "Axis diameter too small");
        
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
        setVertexArray(vertices);
        setNormalArray(normals);
        setNormalBinding(BIND_PER_VERTEX);
        
        // Create twice as many flat edges as we need teeth (i.e. both the tooth and the space between)
        int numberOfEdges = 2 * params._numberOfTeeth;
        int nStart = 0;
        double z = -params._depth;
        Vec O(0., 0., 0.);
        Vec Oz(0., 0., z);
        Vec NO(0., 0., 1.);
        Vec NOz(0., 0., -1.);
        // Edges (Quadstrips)
        addPrimitiveSet(
                        new osg::DrawArrays(osg::PrimitiveSet::QUADS, vertices->size(),
                                            12 * (numberOfEdges)));
        for (short i = 0; i < numberOfEdges; ++i) {
            nStart = vertices->size();
            
            Vec p[12]; // 12 points in total.
            double x0 = rootRadius * cos(2*PI * double(i) / numberOfEdges);
            double y0 = rootRadius * sin(2*PI * double(i) / numberOfEdges);
            double xp1 = rootRadius * cos(2*PI * double(i + 1) / numberOfEdges);
            double yp1 = rootRadius * sin(2*PI * double(i + 1) / numberOfEdges);
            
            double x0z = zrootRadius * cos(params._helix + 2*PI * double(i) / numberOfEdges);
            double y0z = zrootRadius * sin(params._helix + 2*PI * double(i) / numberOfEdges);
            double xp1z = zrootRadius * cos(params._helix + 2*PI * double(i + 1) / numberOfEdges);
            double yp1z = zrootRadius * sin(params._helix + 2*PI * double(i + 1) / numberOfEdges);
            
            p[0] = Vec(x0, y0, 0.);
            Vec p0z = Vec(x0z, y0z, 0.);
            Vec p1nz = Vec(x0, y0, z);
            p[1] = Vec(x0z, y0z, z);
            // the space in between.
            p[10] = Vec(xp1, yp1, 0.);
            p[11] = Vec(xp1z, yp1z, z);
            Vec nu = (p1nz - p[0]) ^ (p[10] - p[0]);
            nu.normalize();
            Vec nl = (p[1] - p0z) ^ (p[11] - p[1]);
            nl.normalize();
            
            // The tooth. Denendum part.
            p[2] = p[0] + nu * params._module;
            p[3] = p[1] + nl * params._module;
            p[8] = p[10] + nu * params._module;
            p[9] = p[11] + nl * params._module;
            
            // The tooth. Addendum part.
            p[4] = p[2] + nu * params._module;
            p[5] = p[3] + nl * params._module;
            p[6] = p[8] + nu * params._module;
            p[7] = p[9] + nl * params._module;
            
            Vec tu = p[6] - p[4];
            Vec tl = p[7] - p[5];
            p[4] += tu * 1. / 3.;
            p[6] -= tu * 1. / 3.;
            p[5] += tl * 1. / 3.;
            p[7] -= tl * 1. / 3.;
            
            if (i % 2 == 0) {
                for (int j = 0; j < 5; ++j) {
                    vertices->push_back(p[2 * j + 0]);
                    vertices->push_back(p[2 * j + 1]);
                    vertices->push_back(p[2 * j + 3]);
                    vertices->push_back(p[2 * j + 2]);
                    
                    // Last two vertices
                    Vec n = (p[2 * j + 1] - p[2 * j]) ^ (p[2 * j + 2] - p[2 * j]);
                    n.normalize();
                    normals->push_back(n);
                    normals->push_back(n);
                    normals->push_back(n);
                    normals->push_back(n);
                }
            } else {
                double xm1 = rootRadius * cos(2*PI * double(i - 1) / numberOfEdges);
                double ym1 = rootRadius * sin(2*PI * double(i - 1) / numberOfEdges);
                double xp2 = rootRadius * cos(2*PI * double(i + 2) / numberOfEdges);
                double yp2 = rootRadius * sin(2*PI * double(i + 2) / numberOfEdges);
                
                Vec pm = Vec(xm1, ym1, 0);
                Vec pp = Vec(xp2, yp2, 0);
                
                vertices->push_back(p[0]);
                vertices->push_back(p[1]);
                vertices->push_back(p[11]);
                vertices->push_back(p[10]);
                
                Vec nm1 = (p[1] - p[0]) ^ (p[0] - pm);
                Vec np1 = (p[1] - p[0]) ^ (pp - p[10]);
                nm1.normalize();
                np1.normalize();
                Vec nu1 = nu + nm1;
                nu1.normalize();
                Vec nu2 = nu + np1;
                nu2.normalize();
                Vec nl1 = nl + nm1;
                nl1.normalize();
                Vec nl2 = nl + np1;
                nl2.normalize();
                
                // Use interpolated normals
                normals->push_back(nu1);
                normals->push_back(nl1);
                normals->push_back(nl2);
                normals->push_back(nu2);
            }
        }
        // Internal edge
        nStart = vertices->size();
        for (short i = 0; i < numberOfEdges + 1; ++i) {
            double xm1 = axisRadius * cos(2*PI * double(i - 1) / numberOfEdges);
            double ym1 = axisRadius * sin(2*PI * double(i - 1) / numberOfEdges);
            double x0 = axisRadius * cos(2*PI * double(i) / numberOfEdges);
            double y0 = axisRadius * sin(2*PI * double(i) / numberOfEdges);
            double xp1 = axisRadius * cos(2*PI * double(i + 1) / numberOfEdges);
            double yp1 = axisRadius * sin(2*PI * double(i + 1) / numberOfEdges);
            
            // Actual two vertices
            Vec p0(x0, y0, 0.);
            Vec p1(x0, y0, z);
            
            // Last two vertices
            Vec pm0(xm1, ym1, 0.);
            Vec pm1(xm1, ym1, z);
            // Next two vertices
            Vec pp0(xp1, yp1, 0);
            Vec pp1(xp1, yp1, z);
            
            Vec n1 = (p1 - p0) ^ (pp1 - p0);
            Vec n2 = (p1 - p0) ^ (p0 - pm1);
            
            Vec n = n1 + n2;
            n.normalize();
            n *= -1;
            vertices->push_back(p0);
            vertices->push_back(p1);
            normals->push_back(n);
            normals->push_back(n);
            
        }
        addPrimitiveSet(
                        new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, nStart,
                                            vertices->size() - nStart));
        nStart = vertices->size();
        
        // End surfaces
        for (short j = 0; j < 2; ++j) {
            nStart = vertices->size();
            for (short i = 0; i < numberOfEdges + 1; ++i) {
                double x0 = axisRadius * cos(2*PI * double(i) / numberOfEdges);
                double y0 = axisRadius * sin(2*PI * double(i) / numberOfEdges);
                double x1 = rootRadius * cos(2*PI * double(i) / numberOfEdges);
                double y1 = rootRadius * sin(2*PI * double(i) / numberOfEdges);
                
                double x0z = axisRadius * cos(params._helix + 2*PI * double(i) / numberOfEdges);
                double y0z = axisRadius * sin(params._helix + 2*PI * double(i) / numberOfEdges);
                double x1z = zrootRadius * cos(params._helix + 2*PI * double(i) / numberOfEdges);
                double y1z = zrootRadius * sin(params._helix + 2*PI * double(i) / numberOfEdges);
                
                Vec p0, p1;
                if (j == 0) {
                    p0 = Vec(x0, y0, 0);
                    p1 = Vec(x1, y1, 0);
                } else {
                    p0 = Vec(x0z, y0z, z);
                    p1 = Vec(x1z, y1z, z);
                }
                vertices->push_back(p0);
                vertices->push_back(p1);
                normals->push_back(j == 0 ? NO : NOz);
                normals->push_back(j == 0 ? NO : NOz);
            }
            
            addPrimitiveSet(
                            new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, nStart,
                                                vertices->size() - nStart));
        }
        
        // End caps for the teeth
        for (short j = 0; j < 2; ++j) {
            for (short i = 0; i < numberOfEdges; i += 2) {
                nStart = vertices->size();
                double x0 = rootRadius * cos(2. * PI * double(i) / numberOfEdges);
                double y0 = rootRadius * sin(2. * PI * double(i) / numberOfEdges);
                double x1 = rootRadius * cos(2. * PI * double(i + 1) / numberOfEdges);
                double y1 = rootRadius * sin(2. * PI * double(i + 1) / numberOfEdges);
                
                double x0z = zrootRadius * cos(params._helix + 2*PI * double(i) / numberOfEdges);
                double y0z = zrootRadius * sin(params._helix + 2*PI * double(i) / numberOfEdges);
                double x1z = zrootRadius * cos(params._helix + 2*PI * double(i + 1) / numberOfEdges);
                double y1z = zrootRadius * sin(params._helix + 2*PI * double(i + 1) / numberOfEdges);
                
                Vec p0, p5, n;
                if (j == 0) {
                    p0 = Vec(x0, y0, 0);
                    p5 = Vec(x1, y1, 0);
                    n = (Vec(x1, y1, 0) - Vec(x0, y0, 0))
                    ^ (Vec(x0, y0, 0) - Vec(x0, y0, z));
                } else {
                    p0 = Vec(x0z, y0z, z);
                    p5 = Vec(x1z, y1z, z);
                    n = (Vec(x1z, y1z, 0) - Vec(x0z, y0z, 0))
                    ^ (Vec(x0z, y0z, 0) - Vec(x0z, y0z, z));
                }
                n.normalize();
                
                // The tooth. Denendum part.
                Vec p1 = p0 + n * params._module;
                Vec p4 = p5 + n * params._module;
                
                // The tooth. Addendum part.
                Vec p2 = p1 + n * params._module;
                Vec p3 = p4 + n * params._module;
                
                Vec t = p3 - p2;
                p2 += t * 1. / 3.;
                p3 -= t * 1. / 3.;
                vertices->push_back(p0);
                vertices->push_back(p1);
                vertices->push_back(p2);
                vertices->push_back(p3);
                vertices->push_back(p4);
                vertices->push_back(p5);
                addPrimitiveSet(
                                new osg::DrawArrays(osg::PrimitiveSet::POLYGON, nStart,
                                                    vertices->size() - nStart));
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
        
    double ParametricSpurGear::toothRatio (const Vec& v) const
    {
        double delta = 2. * PI / params._numberOfTeeth;
        double angle = acos (v * Vec(1., 0., 0.)/v.length());
        double ratioEdge = fmod (angle, delta) / delta;
        return fmod(ratioEdge+0.5, 1.);
    }
    
    double ParametricSpurGear::angleFromRatio (double ratioOther, const Vec& vOwn) const
    {
        double ratioOwn = toothRatio (vOwn);
        double dRatio = 1. - (ratioOther + ratioOwn);
        double dAngle = 2. * PI / params._numberOfTeeth;
        return dRatio * dAngle;
    }
    
}
