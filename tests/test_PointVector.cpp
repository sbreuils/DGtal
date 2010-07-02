/**
 * @file test_PointVector.cpp
 * @author David Coeurjolly (\c david.coeurjolly@liris.cnrs.fr )
 *
 *
 * @date 2010/03/03
 *
 * This file is part of the DGtal library
 */

/**
 * Description of test_trace' <p>
 * Aim: simple test of \ref MeasureOfStraighLines
 */

#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include "DGtal/base/Common.h"
#include "DGtal/kernel/PointVector.h"

using namespace DGtal;
using namespace std;


bool testComparison()
{
    const double t[ ] = { 3.5, 4.1, 2.2, 3.2 };
    PointVector<double, 4> v ( t );
    PointVector<double, 4> v2 ( t );
    PointVector<double, 4> v3 ( { 3.5, 4.2, 2.2, 3.2 } );

    trace.beginBlock("Comparison of Points");
    if (v == v2)
        trace.info()<< "v == v2 (true)"<<std::endl;
    else
        trace.info()<< "v == v2 (false)"<<std::endl;

    if (v == v3)
        trace.info()<< "v == v3 (true)"<<std::endl;
    else
        trace.info()<< "v == v3 (false)"<<std::endl;

    if (v < v3)
        trace.info()<< "v < v3 (true)"<<std::endl;
    else
        trace.info()<< "v < v3 (false)"<<std::endl;


    trace.endBlock();

    return ((v == v2) && !(v != v2));
}



/**
 * Test instanciation of Points
 *
 **/
bool testSimplePoint()
{
    PointVector<int,3>  aPVInt3; 
    PointVector<double,4> aPoint={-3 ,4 ,4.5 ,0};
    PointVector<double,4> aFPoint;

    aPoint *= 5.6;

    // Marche pô
    // aPoint.isValid();
    cout << "aPoint=" << aPoint << endl;

    trace.beginBlock ( "Test point dimension" );
    trace.info() << "aPoint dimension="<<aPoint.dimension() <<endl;
    trace.endBlock();

    if ( aPoint.dimension() != 4 )
        return false;

    PointVector<double, 4> v ({ 3.5, 4.1, 2.2, 3.2 });
    aPoint = aFPoint + v;
    trace.beginBlock ( "Test point addition with vector" );
    trace.info() << "aPoint = "<< aFPoint << " + " << v << endl;
    trace.info() << "aPoint = "<< aPoint << endl;
    trace.endBlock();

    return true;
}

bool testNorms()
{
    typedef PointVector<double,3> PointType;
    PointType aPoint;

    aPoint.at ( 2 ) =  2;
    aPoint.at ( 1 ) = -1;
    aPoint.at ( 0 ) =  3;

    trace.beginBlock ( "Test of Norms" );
    trace.info() << "aPoint l_2 norm="<<aPoint.norm() <<endl;
    trace.info() << "aPoint l_1 norm="<<aPoint.norm ( PointType::L_1 ) <<endl;
    trace.info() << "aPoint l_infty norm="<<aPoint.norm ( PointType::L_infty ) <<endl;
    trace.endBlock();


    return ( ( aPoint.norm ( PointType::L_1 ) == 6 ) &&
             ( aPoint.norm ( PointType::L_infty ) == 3 ) );

}

/**
* Test instancition of Vectors
*
**/
bool testSimpleVector()
{
    PointVector<int,3>  aPVInt3;
    PointVector<double,4> aVector;
    PointVector<double,4> aFVector;

    trace.beginBlock ( "Test of Vector Dimension" );
    trace.info() << "aVector dimension="<< aVector.dimension() <<endl;
    trace.info() << "aVector = "<< aVector <<endl;
    trace.endBlock();

    if ( aVector.dimension() != 4 )
        return false;

    aVector += aFVector;

    return true;
}


bool testIterator()
{
    PointVector<double,25> aPoint;
    PointVector<int,4> avector;

    trace.beginBlock("Point Iterator Test");

    for (unsigned int i=0;i<25;++i)
        aPoint.at(i) = i;
    trace.info() << "aPoint="<<aPoint<< std::endl;

    trace.info() << "With iterator: ";
    for (PointVector<double,25>::Iterator it = aPoint.begin() ;  it != aPoint.end(); ++it)
        trace.info() << (*it) <<" " ;

    trace.info() << std::endl;

    trace.endBlock();

    return true;
}

bool testOperators()
{
  trace.beginBlock("Point Operators Test");

  PointVector<int,4> p1({1,2,3,4});
  PointVector<int,4> p2({5,4,3,2});

  trace.info() << "p1: "<<p1 <<", "<<"p2: "<<p2 <<std::endl;
  trace.info() << "p1+p2: "<<p1+p2 <<std::endl;
  trace.info() << "p1-p2: "<<p1-p2 <<std::endl;
  trace.info() << "inf(p1,p2): "<<p1.inf(p2) <<std::endl;
  trace.info() << "sup(p1,p2): "<<p1.sup(p2) <<std::endl;
  trace.endBlock();

  return true; 
}

int main()
{
    bool res;
    res =  testSimplePoint()  &&    testSimpleVector() &&
      testNorms()  &&
      testIterator() && testComparison() &&
      testOperators();
    if (res)
        return 0;
    else
        return 1;
}

