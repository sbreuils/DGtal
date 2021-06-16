/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#pragma once

/**
 * @file NeighborhoodConvexityAnalyzer.h
 * @author Jacques-Olivier Lachaud (\c jacques-olivier.lachaud@univ-savoie.fr )
 * Laboratory of Mathematics (CNRS, UMR 5127), University of Savoie, France
 *
 * @date 2021/06/16
 *
 * Header file for module NeighborhoodConvexityAnalyzer.cpp
 *
 * This file is part of the DGtal library.
 */

#if defined(NeighborhoodConvexityAnalyzer_RECURSES)
#error Recursive header files inclusion detected in NeighborhoodConvexityAnalyzer.h
#else // defined(NeighborhoodConvexityAnalyzer_RECURSES)
/** Prevents recursive inclusion of headers. */
#define NeighborhoodConvexityAnalyzer_RECURSES

#if !defined NeighborhoodConvexityAnalyzer_h
/** Prevents repeated inclusion of headers. */
#define NeighborhoodConvexityAnalyzer_h

//////////////////////////////////////////////////////////////////////////////
// Inclusions
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <unordered_set>
#include "DGtal/base/Common.h"
#include "DGtal/base/Clone.h"
#include "DGtal/kernel/CPointPredicate.h"
#include "DGtal/kernel/domains/HyperRectDomain.h"
#include "DGtal/topology/CCellularGridSpaceND.h"
#include "DGtal/topology/KhalimskySpaceND.h"
#include "DGtal/geometry/volumes/DigitalConvexity.h"
//////////////////////////////////////////////////////////////////////////////

namespace DGtal
{

  /////////////////////////////////////////////////////////////////////////////
  // template class NeighborhoodConvexityAnalyzer
  /**
     Description of template class 'NeighborhoodConvexityAnalyzer' <p>
     \brief Aim: A class that model a \f$ (2k+1)^d \f$ neighborhood and
     that provides services to analyse the convexity properties of a
     digital set within this neighborhood.

     It is a model of boost::CopyConstructible,
     boost::DefaultConstructible, boost::Assignable.

     @tparam TKSpace an arbitrary model of CCellularGridSpaceND.
     @tparam K the parameter that determines the size of the
     neighborhood along all dimensions (spans 2K+1 points in each
     direction, hence the neighborhood cardinal is \f$ (2K+1)^d \f$).
  */
  template < typename TKSpace, int K >
  class NeighborhoodConvexityAnalyzer
  {
    BOOST_CONCEPT_ASSERT(( concepts::CCellularGridSpaceND< TKSpace > ));

  public:
    typedef NeighborhoodConvexityAnalyzer<TKSpace,K> Self;
    typedef TKSpace                         KSpace;
    typedef typename KSpace::Space          Space;
    typedef typename KSpace::Integer        Integer;
    typedef typename KSpace::Point          Point;
    typedef typename KSpace::Vector         Vector;
    typedef typename KSpace::Cell           Cell;
    typedef std::vector<Point>              PointRange;
    typedef HyperRectDomain<Space>          Domain;

    static const Dimension dimension = KSpace::dimension;

    // ------------------------- Standard services --------------------------------
  public:
    /// @name Standard services (construction, initialization, assignment)
    /// @{

    /**
     * Destructor.
     */
    ~NeighborhoodConvexityAnalyzer() = default;

    /**
     * Constructor.
     */
    NeighborhoodConvexityAnalyzer() = default;

    /**
     * Copy constructor.
     * @param other the object to clone.
     */
    NeighborhoodConvexityAnalyzer ( const Self & other ) = default;

    /**
     * Constructor from cellular space.
     * @param aKSpace any cellular grid space.
     */
    NeighborhoodConvexityAnalyzer( Clone<KSpace> aKSpace )
      : myDigConv( aKSpace )
    {
      myDomain       = Domain( aKSpace.lowerBound(), aKSpace.upperBound() );
      myComputations = 0;
      myResults      = 0;
    }

    /**
     * Constructor from lower and upper points.
     * @param lo the lowest point of the domain (bounding box for computations).
     * @param hi the highest point of the domain (bounding box for computations).
     */
    NeighborhoodConvexityAnalyzer( Point lo, Point hi )
      : myDomain( lo, hi ), myDigConv( lo, hi )
    {
      myComputations = 0;
      myResults      = 0;
    }

    /**
     * Assignment.
     * @param other the object to copy.
     * @return a reference on 'this'.
     */
    Self & operator= ( const Self & other ) = default;

    /// @return a const reference to the cellular grid space used by this object.
    const KSpace& space() const
    {
      return myDigConv.space();
    }

    /// @return a const reference to the domain used by this object.
    const Domain& domain() const
    {
      return myDomain;
    }

    /// @return the fixed parameter K of the neighborhood, which determines its size.
    static int size()
    { return K; }
    
    /// @}

    // ------------------------- Neighborhood services --------------------------------
  public:
    /// @name Neighborhood services
    /// @{

    /// Place the center of the neighborhood at point \a c on shape \a X
    /// All subsequent computations and results are related to this point afterwards.
    ///
    /// @tparam PointPredicate an arbitrary model of concept::CPointPredicate
    ///
    /// @param c any point in the domain of the embedding space.
    ///
    /// @param X a predicate associating a boolean to any point, which
    /// corresponds to a characteristic function of a digital subset X
    /// of the digital space.
    template < typename PointPredicate >
    void setCenter( Point c, const PointPredicate& X );

    /// @return the current center.
    Point center() const
    {
      return myCenter;
    }

    /// Tells if the current center belongs to the shape X
    bool isCenterInX() const
    {
      return myCenterInX;
    }

    /// @return 'true' iff the center is locally fully convex collapsible.
    bool isFullyConvexCollapsible()
    {
      if ( isCenterInX() )
        return ( ! myLocalX.empty() )
          && isFullyConvex( true )
          && isFullyConvex( false );
      else
        return ( ! myLocalCompX.empty() )
          && isComplementaryFullyConvex( true )
          && isComplementaryFullyConvex( false );
    }
    
    /// Tells if the shape X is locally fully convex.
    /// @param with_center if 'true' add the center to the digital set.
    ///
    /// @return 'true' iff the local neighborhood of X (with/without
    /// the center) is fully convex
    bool isFullyConvex( bool with_center )
    {
      int mask = with_center
        ? FullConvexity_X_with_center : FullConvexity_X_without_center;
      if ( myComputations & mask ) return bool( myResults & mask );
      if ( with_center ) myLocalX.push_back( center() );
      bool ok = myDigConv.isFullyConvex( myLocalX );
      if ( with_center ) myLocalX.pop_back();
      myComputations |= mask;
      if ( ok ) myResults |= mask;
      return ok;
    }

    /// Tells if the complementary of the shape is locally fully convex.
    /// @param with_center if 'true' add the center to the digital set.
    ///
    /// @return 'true' iff the local neighborhood of the complementary
    /// of X (with/without the center) is fully convex
    bool isComplementaryFullyConvex( bool with_center )
    {
      int mask = with_center
        ? FullConvexity_CompX_with_center : FullConvexity_CompX_without_center;
      if ( myComputations & mask ) return bool( myResults & mask );
      if ( with_center ) myLocalCompX.push_back( center() );
      bool ok = myDigConv.isFullyConvex( myLocalCompX );
      if ( with_center ) myLocalCompX.pop_back();
      myComputations |= mask;
      if ( ok ) myResults |= mask;
      return ok;
    }

    /// Tells if the shape is locally digitally 0-convex.
    /// @param with_center if 'true' add the center to the digital set.
    ///
    /// @return 'true' iff the local neighborhood of X (with/without
    /// the center) is 0 convex
    bool is0Convex( bool with_center )
    {
      int mask = with_center
        ? FullConvexity_X_with_center : FullConvexity_X_without_center;
      if ( myComputations & mask ) return bool( myResults & mask );
      if ( with_center ) myLocalX.push_back( center() );
      bool ok = myDigConv.is0Convex( myLocalX );
      if ( with_center ) myLocalX.pop_back();
      myComputations |= mask;
      if ( ok ) myResults |= mask;
      return ok;
    }

    /// Tells if the complementary of the shape is locally digitally 0-convex.
    /// @param with_center if 'true' add the center to the digital set.
    ///
    /// @return 'true' iff the local neighborhood of the complementary
    /// of X (with/without the center) is 0 convex
    bool isComplementary0Convex( bool with_center )
    {
      int mask = with_center
        ? FullConvexity_CompX_with_center : FullConvexity_CompX_without_center;
      if ( myComputations & mask ) return bool( myResults & mask );
      if ( with_center ) myLocalCompX.push_back( center() );
      bool ok = myDigConv.is0Convex( myLocalCompX );
      if ( with_center ) myLocalCompX.pop_back();
      myComputations |= mask;
      if ( ok ) myResults |= mask;
      return ok;
    }

    /// @}
    
    // ------------------------- Protected Datas ------------------------------
  protected:
    /// The bounded domain in which computations are carried out.
    Domain myDomain;
    /// The digital convexity that is used for checking full convexity.
    DigitalConvexity<KSpace> myDigConv;
    /// The current center of the neighborhood
    Point myCenter;
    /// the part of X belonging to this neighborhood.
    PointRange myLocalX;
    /// the part of the neighborhood that is not in X.
    PointRange myLocalCompX;
    /// tells if the center belongs to X
    bool myCenterInX;
    
    /// Enum types indicating the possible type of local computations.
    enum Computation {
      FullConvexity_X_with_center        = 0x1,
      FullConvexity_X_without_center     = 0x2,
      FullConvexity_CompX_with_center    = 0x4,
      FullConvexity_CompX_without_center = 0x8,
      Convexity_X_with_center            = 0x10,
      Convexity_X_without_center         = 0x20,
      Convexity_CompX_with_center        = 0x40,
      Convexity_CompX_without_center     = 0x80,
    };
    /// Stores which properties have already been computed.
    int myComputations;
    /// Stores the properties boolean values.
    int myResults;
      
    // ------------------------- Private Datas --------------------------------
  private:

    // ------------------------- Internals ------------------------------------
  private:

  }; // end of class NeighborhoodConvexityAnalyzer


} // namespace DGtal


///////////////////////////////////////////////////////////////////////////////
// Includes inline functions.
#include "NeighborhoodConvexityAnalyzer.ih"

//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#endif // !defined NeighborhoodConvexityAnalyzer_h

#undef NeighborhoodConvexityAnalyzer_RECURSES
#endif // else defined(NeighborhoodConvexityAnalyzer_RECURSES)
