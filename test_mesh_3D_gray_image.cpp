#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Labeled_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/Image_3.h>

#include <CGAL/IO/read_vtk_image_data.h>

// VTK includes
#include <vtkNew.h>
#include <vtkNIFTIImageReader.h>

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Labeled_mesh_domain_3<K> Mesh_domain;

#ifdef CGAL_CONCURRENT_MESH_3
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain,CGAL::Default,Concurrency_tag>::type Tr;

typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

// To avoid verbose function and named parameters call
using namespace CGAL::parameters;

int main(int argc, char* argv[])
{
  /// [Loads image]
//  const std::string fname = (argc>1)?argv[1]:CGAL::data_file_path("images/liver.inr.gz");
//  CGAL::Image_3 image;
//  if(!image.read(fname)){
//    std::cerr << "Error: Cannot read file " <<  fname << std::endl;
//    return EXIT_FAILURE;
//  }
	const std::string fname = (argc>1)?argv[1]:CGAL::data_file_path("tense-i.nii.gz");
  vtkNew<vtkNIFTIImageReader> reader;
      reader->SetFileName(fname.c_str()); 
      reader->Update();
      auto vtk_image = reader->GetOutput();
      CGAL::Image_3 image(CGAL::IO::read_vtk_image_data(vtk_image));
      std::cout << "\nImage successfully imported" << endl;
  /// [Loads image]

  /// [Domain creation]
  Mesh_domain domain = Mesh_domain::create_labeled_image_mesh_domain(image);
  /// [Domain creation]

  // Mesh criteria
  Mesh_criteria criteria(facet_angle=30, facet_size=3, facet_distance=4,
                         cell_radius_edge_ratio=3, cell_size=8);

  /// [Meshing]
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);
  /// [Meshing]

  // Output
  std::ofstream medit_file("out.mesh");
  c3t3.output_to_medit(medit_file);

  return 0;
}
