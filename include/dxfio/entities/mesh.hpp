#ifndef DXFIO_ENTITIES_MESH_HPP
#define DXFIO_ENTITIES_MESH_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/group.hpp"
#include "dxfio/vector.hpp"

#include <string>
#include <vector>

namespace DxfIO::Entities
{
  /**
   *  The AcDbSubDMesh subclass of a MESH: its vertices, faces, edges and creases.
   */
  struct DXFIO_API Net
  {
    /**
     *  The version of the mesh's layout. Group 71.
     */
    int Version{2};
    /**
     *  Whether creases are blended. Group 72.
     */
    int Blend{};
    /**
     *  The level of subdivision smoothing. Group 91.
     */
    int Level{};
    /**
     *  The vertices; group 92 counts them. Groups 10, 20, 30.
     */
    std::vector<Vector> Vertices{};
    /**
     *  The faces, each its vertex count and then the index of each vertex; group 93 counts the
     *  numbers. Group 90.
     */
    std::vector<int> Faces{};
    /**
     *  The edges, as pairs of vertex indices; group 94 counts the pairs. Group 90. A MESH whose
     *  count of indices is odd is not written, and one read so is kept as it came.
     */
    std::vector<int> Edges{};
    /**
     *  The crease of each edge; group 95 counts them. Group 140.
     */
    std::vector<double> Creases{};
    /**
     *  The number of parts of the mesh whose properties are overridden. Group 90.
     */
    int Overridden{};
    /**
     *  The overrides that follow, kept as the groups they came in.
     */
    std::vector<Group> Overrides{};

    auto operator==(const Net &) const -> bool = default;
  };

  /**
   *  A MESH: a subdivision surface of faces, edges and creases.
   */
  struct DXFIO_API Mesh
  {
    /**
     *  The layer, linetype, color and the rest every entity carries.
     */
    Entities::Record Record{};
    /**
     *  The proxy graphics, for programs that cannot draw the entity. Group 92, or 160 from R2010,
     *  then 310.
     */
    std::string Picture{};
    /**
     *  The vertices, faces, edges and creases, from the AcDbSubDMesh subclass.
     */
    Boxed<Entities::Net> Net{};

    auto operator==(const Mesh &) const -> bool = default;
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_MESH_HPP */
