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
#include<fijee.h>
//
//
//
int main()
{
  //
  //
  Solver::PDE_solver_parameters* solver_parameters = Solver::PDE_solver_parameters::get_instance();
  // 
  solver_parameters->init();

//  // 
//  // tACS electrodes' setup
//  std::vector< std::tuple<std::string, double> > positive_electrodes;
//  positive_electrodes.push_back( std::make_tuple("C6", 0.00112 /*[A]*/) );
//  //  positive_electrodes.push_back( std::make_tuple("FT7", 0.00112 /*[A]*/) );
//  std::vector< std::tuple<std::string, double> > negative_electrodes;
//  negative_electrodes.push_back( std::make_tuple("T8", -0.00112 / 4. /*[A]*/) );
//  negative_electrodes.push_back( std::make_tuple("F8", -0.00112 / 4. /*[A]*/) );
//  negative_electrodes.push_back( std::make_tuple("C4", -0.00112 / 4. /*[A]*/) );
//  negative_electrodes.push_back( std::make_tuple("P8", -0.00112 / 4. /*[A]*/) );
//  //  Electrodes::Electrodes_setup< Electrodes::Electrodes_tACS >
//  Electrodes::Electrodes_tACS electrodes_setting( positive_electrodes, negative_electrodes,
//					            10  /* [Hz]*/, 0.0005 /* [A] Amplitude */,
//					            0.1 /* [s] elapse time */, 
//					            1.  /* [s] starting time */ );
//  // 
//  electrodes_setting.output_XML( solver_parameters->get_files_path_output_() );

  //
  // Physical models:
  // export OMP_NUM_THREADS=2
  Solver::Model_solver< /* physical model */ Solver::tCS_tACS,
		        /* number_of_threads_ */ 4 >  model;
  //
  std::cout << "Loop over solvers" << std::endl;
  model.solver_loop();
  model.XML_output();

  //
  //
  return EXIT_SUCCESS;
}
