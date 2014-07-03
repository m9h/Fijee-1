//  Copyright (c) 2014, Yann Cobigo 
//  All rights reserved.     
//   
//  Redistribution and use in source and binary forms, with or without       
//  modification, are permitted provided that the following conditions are met:   
//   
//  1. Redistributions of source code must retain the above copyright notice, this   
//     list of conditions and the following disclaimer.    
//  2. Redistributions in binary form must reproduce the above copyright notice,   
//     this list of conditions and the following disclaimer in the documentation   
//     and/or other materials provided with the distribution.   
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE   
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR   
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES   
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;   
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND   
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT   
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS   
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   
//     
//  The views and conclusions contained in the software and documentation are those   
//  of the authors and should not be interpreted as representing official policies,    
//  either expressed or implied, of the FreeBSD Project.  
#include "Build_dipoles_list_high_density.h"
//
// We give a comprehensive type name
//
typedef Domains::Build_dipoles_list_high_density DBdlhd;
typedef Domains::Access_parameters DAp;
//
// get function for the property map
//
Domains::Point_vector_high_density_map::reference 
Domains::get( Domains::Point_vector_high_density_map, Domains::Point_vector_high_density_map::key_type p)
{
  return std::get<0>(p);
};
//
//
//
DBdlhd::Build_dipoles_list_high_density():
  cell_size_(2. /*mm*/), layer_(1)
{
  //
  // Get the white and gray matter vertices matching tuples.
//  (DAp::get_instance())->get_lh_match_wm_gm_( lh_match_wm_gm_ );
//  (DAp::get_instance())->get_rh_match_wm_gm_( rh_match_wm_gm_ );

  (DAp::get_instance())->get_lh_white_matter_surface_point_normal_( lh_wm_ );
  (DAp::get_instance())->get_rh_white_matter_surface_point_normal_( rh_wm_ );

  std::cout << "Size left  H: " << lh_wm_.size()  << std::endl;
  std::cout << "Size right H: " << rh_wm_.size()  << std::endl;
}
//
//
//
DBdlhd::Build_dipoles_list_high_density( const DBdlhd& that )
{
}
//
//
//
DBdlhd::~Build_dipoles_list_high_density()
{
  /* Do nothing */
}
//
//
//
void 
DBdlhd::Make_list( const std::list< Cell_conductivity >& List_cell_conductivity )
{
  // 
  // Time log
  FIJEE_TIME_PROFILER("Domain::Build_dipoles_list_high_density::Make_list");

  // 
  // Initialize index and Point vector tuple.
  std::vector< IndexedPointVector > lh_wm_points( lh_wm_.size() );
  std::vector< IndexedPointVector > rh_wm_points( rh_wm_.size() );
  // Left hemisphere
  int l = 0;
  // 
  for( auto vertex : lh_wm_ )
    {
      lh_wm_points[l].get<0>() = l; 
      lh_wm_points[l].get<1>() = Dipole_position( vertex.x(), vertex.y(), vertex.z() ); 
      lh_wm_points[l].get<2>() = vertex;
      // 
      l++;
    }
  // Right hemisphere
  int r = 0;
  for( auto vertex : rh_wm_ )
    {
      rh_wm_points[r].get<0>() = r; 
      rh_wm_points[r].get<1>() = Dipole_position( vertex.x(), vertex.y(), vertex.z() ); 
      rh_wm_points[r].get<2>() = vertex;
      // 
      r++;
    }
  // 
  std::cout << "Density of the white matter 2D mesh: " 
	    << lh_wm_.size() + rh_wm_.size() 
	    << std::endl;

  // 
  // Compute the density of dipoles
  // 
  
  // 
  // Computes average spacing.
  const unsigned int nb_neighbors = 6; // 1 ring
  // 
  FT lh_average_spacing = CGAL::compute_average_spacing(lh_wm_points.begin(), lh_wm_points.end(),
							CGAL::Nth_of_tuple_property_map<1,IndexedPointVector>(),
							nb_neighbors);
  // 
  FT rh_average_spacing = CGAL::compute_average_spacing(rh_wm_points.begin(), rh_wm_points.end(),
							CGAL::Nth_of_tuple_property_map<1,IndexedPointVector>(),
							nb_neighbors);
  // 
  std::cout << "Average spacing dipole: " 
	    << lh_average_spacing << " mm in the left hemisphere, and " 
	    << rh_average_spacing << " mm in the right hemisphere" << std::endl;
  
  // 
  // Control of density
  // Left hemisphere
  lh_wm_points.erase( CGAL::grid_simplify_point_set(lh_wm_points.begin(), 
						    lh_wm_points.end(),  
						    CGAL::Nth_of_tuple_property_map< 1, IndexedPointVector >(),
						    cell_size_),
		      lh_wm_points.end() );
  // Right hemisphere
  rh_wm_points.erase( CGAL::grid_simplify_point_set(rh_wm_points.begin(), 
						    rh_wm_points.end(),  
						    CGAL::Nth_of_tuple_property_map< 1, IndexedPointVector >(),
						    cell_size_),
		      rh_wm_points.end() );
  // 
  std::cout << "Down sizing the dipole distribution d(d1,d2) > 1mm: " 
	    << lh_wm_points.size() + rh_wm_points.size() << " dipoles in the 2D mesh" 
	    << std::endl;


  //
  // Build the knn tree of mesh cell
  // 
  
  // Define two trees to reduce the complexity of the search algorithm
  High_density_tree 
    lh_tree,
    rh_tree;

  // 
  std::vector< bool > cell_conductivity_assignment(List_cell_conductivity.size(), false);
  // 
  for ( auto cell_conductivity : List_cell_conductivity )
    if ( cell_conductivity.get_cell_subdomain_() == GRAY_MATTER )
      {
	// 
	//
	Point_vector cell_centroid = ( cell_conductivity.get_centroid_lambda_() )[0];
	// 
	// left hemisphere
	if( cell_centroid.x() < 0 )
	  lh_tree.insert(std::make_tuple(Dipole_position( cell_centroid.x(),
							  cell_centroid.y(),
							  cell_centroid.z() ),
					 cell_conductivity));
	else // right hemisphere
	  rh_tree.insert(std::make_tuple(Dipole_position( cell_centroid.x(),
							  cell_centroid.y(),
							  cell_centroid.z() ),
					 cell_conductivity));
      }

  // 
  // Dipole list creation
  // 
  // Left hemisphere
  Select_dipole(lh_tree, lh_wm_points, cell_conductivity_assignment);
  // Right hemisphere
  Select_dipole(rh_tree, rh_wm_points, cell_conductivity_assignment);
  // 
  std::cout << "Gray matter dipole distribution: " 
	    << dipoles_list_.size() 
	    << " dipoles"
	    << std::endl;
  //
  //
  Make_analysis();
}
//
//
//
void 
DBdlhd::Select_dipole( const High_density_tree& Tree, 
		       const std::vector< IndexedPointVector >& PV_vector,
		       std::vector< bool >& Cell_assignment )
{
  // 
  // Left hemisphere
  for( auto vertex : PV_vector )
    {
      // 
      // Build the search tree
      High_density_neighbor_search
	dipoles_neighbor( Tree, vertex.get<1>(), layer_ );

      // 
      // Position of the dipole on the nearest mesh cell centroid
      auto dipole_position = dipoles_neighbor.begin();
      // If the conductivity centroid is not yet assigned; otherwise we skip it
      for (int layer = 0 ; layer < layer_ ; layer++ )
	{
	  if( !Cell_assignment[(std::get<1>(dipole_position->first)).get_cell_id_()] )
	    {
	      // Assignes the centroid
	      Cell_assignment[(std::get<1>(dipole_position->first)).get_cell_id_()] = true;
	      // Add the dipole in the list
	      dipoles_list_.push_back( Domains::Dipole(Point_vector((std::get<0>(dipole_position->first)).x(),
								    (std::get<0>(dipole_position->first)).y(),
								    (std::get<0>(dipole_position->first)).z(),
								    (vertex.get<2>()).vx(),
								    (vertex.get<2>()).vy(),
								    (vertex.get<2>()).vz()),
						       std::get<1>(dipole_position->first)) );
	      //
	      // R study
#ifdef TRACE
#if TRACE == 100
	      centroid_vertex_.push_back(std::make_tuple(((std::get<1>(dipole_position->first)).get_centroid_lambda_())[0],
							 vertex.get<2>()));
#endif
#endif
	      break;
	    }

	  // 
	  // next layer
	  dipole_position++;
	}
    }
}
//
//
//
void 
DBdlhd::Make_analysis()
{
#ifdef TRACE
#if TRACE == 100
  //
  //
  output_stream_
    // Dipole point-vector
    << "Dipole_X Dipole_Y Dipole_Z Dipole_vX Dipole_vY Dipole_vZ " 
    // Mesh centroide point-vector
    << "Cent_X Cent_Y Cent_Z Cent_vX Cent_vY Cent_vZ "
    // 2D mesh point-vector
    << "Vertex_X Vertex_Y Vertex_Z Vertex_vX Vertex_vY Vertex_vZ "
    << std::endl;
    
  //
  //
  for( auto centroid_vertex : centroid_vertex_ )
    {
      //
      // 
      output_stream_
	// "Dipole_X Dipole_Y Dipole_Z Dipole_vX Dipole_vY Dipole_vZ "
	<< ((std::get<0>(centroid_vertex))).x() << " "
	<< ((std::get<0>(centroid_vertex))).y() << " "
	<< ((std::get<0>(centroid_vertex))).z() << " "
	<< ((std::get<1>(centroid_vertex))).vx() << " "
	<< ((std::get<1>(centroid_vertex))).vy() << " "
	<< ((std::get<1>(centroid_vertex))).vz() << " "
      // "Cent_X Cent_Y Cent_Z Cent_vX Cent_vY Cent_vZ "
	<< (std::get<0>(centroid_vertex)).x() << " "
	<< (std::get<0>(centroid_vertex)).y() << " "
	<< (std::get<0>(centroid_vertex)).z() << " "
	<< (std::get<0>(centroid_vertex)).vx() << " "
	<< (std::get<0>(centroid_vertex)).vy() << " "
	<< (std::get<0>(centroid_vertex)).vz() << " "	
      // "Vertex_X Vertex_Y Vertex_Z Vertex_vX Vertex_vY Vertex_vZ "
	<< ((std::get<1>(centroid_vertex))).x() << " "
	<< ((std::get<1>(centroid_vertex))).y() << " "
	<< ((std::get<1>(centroid_vertex))).z() << " "
	<< ((std::get<1>(centroid_vertex))).vx() << " "
	<< ((std::get<1>(centroid_vertex))).vy() << " "
	<< ((std::get<1>(centroid_vertex))).vz() << " "
	<< std::endl;	
    } 

  //
  //
  Make_output_file("Dipole_high_density.distribution.frame");
#endif
#endif      
}
//
//
//
void
DBdlhd::Output_dipoles_list_xml()
{
  //
  // Output xml files. 
  std::string dipoles_XML = 
    (Domains::Access_parameters::get_instance())->get_files_path_output_();
  dipoles_XML += std::string("dipoles.xml");
  //
  std::ofstream dipoles_file( dipoles_XML.c_str() );
  
  //
  //
  Build_stream(dipoles_file);

  //
  //
  dipoles_file.close();
}
//
//
//
DBdlhd& 
DBdlhd::operator = ( const DBdlhd& that )
{

  //
  //
  return *this;
}
//
//
//
void
DBdlhd::Build_stream( std::ofstream& stream )
{
  //
  //
  stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	 << "<fijee xmlns:fijee=\"https://github.com/Fijee-Project/Fijee\">\n";
  
  //
  //
  stream << "  <dipoles size=\"" << dipoles_list_.size() << "\">\n";
  
  //
  //
  int index = 0;
  for ( auto dipole : dipoles_list_ )
    stream << "    <dipole index=\"" << index++ << "\" " << dipole << "/>\n";
  
  //
  //
  stream << "  </dipoles>\n" 
	 << "</fijee>\n"; 
}
//
//
//
std::ostream& 
Domains::operator << ( std::ostream& stream, 
		       const DBdlhd& that)
{

  //
  //
  return stream;
};
