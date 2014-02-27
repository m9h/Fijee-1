#ifndef _PHYSICS_H
#define _PHYSICS_H
//
// FEniCS
//
#include <dolfin.h>
//
// UCSF project
//
#include "PDE_solver_parameters.h"
#include "Conductivity.h"

//using namespace dolfin;

/*! \namespace Solver
 * 
 * Name space for our new package
 *
 */
//
// 
//
namespace Solver
{
  /*! \class Physics
   * \brief classe representing the mother class of all physical process: source localization (direct and subtraction), transcranial Current Stimulation (tDCS, tACS).
   *
   *  This class representing the Physical model.
   */
  class Physics
  {
  protected:
    //! Head model mesh
    boost::shared_ptr< dolfin::Mesh > mesh_;
    //! Head model sub domains
    boost::shared_ptr< MeshFunction< std::size_t > > domains_;
    //! Anisotropic conductivity
    boost::shared_ptr< Solver::Tensor_conductivity > sigma_;


  public:
    /*!
     *  \brief Default Constructor
     *
     *  Constructor of the class Physics
     *
     */
    Physics();
    /*!
     *  \brief Copy Constructor
     *
     *  Constructor is a copy constructor
     *
     */
    Physics( const Physics& ){};
    /*!
     *  \brief Destructeur
     *
     *  Destructor of the class Physics
     */
    virtual ~Physics(){/* Do nothing */};
    /*!
     *  \brief Operator =
     *
     *  Operator = of the class Physics
     *
     */
    Physics& operator = ( const Physics& ){return *this;};

  public:
    /*!
     *  \brief Solution domain extraction
     *
     *  This method extract from the Function solution U the sub solution covering the sub-domains Sub_domains.
     *  The result is a file with the name tDCS_{Sub_domains}.vtu
     *
     *  \param U: Function solution of the Partial Differential Equation.
     *  \param Sub_domains: array of sub-domain we want to extract from U.
     *
     */
    void solution_domain_extraction( const dolfin::Function&, std::list<std::size_t>&, 
				     const char* );
   };
};

#endif