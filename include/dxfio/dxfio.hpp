#ifndef DXFIO_DXFIO_HPP
#define DXFIO_DXFIO_HPP

/**
 *  dxfio reads and writes DXF, every release from R10 to R2018, ASCII and binary. This header
 *  brings in the whole public interface: Load, Save, Create, the Drawing they work on, the editing
 *  helpers of DxfIO::Edit and the audit of DxfIO::Audit.
 */

#include "dxfio/audit/audit.hpp"
#include "dxfio/create.hpp"
#include "dxfio/drawing.hpp"
#include "dxfio/edit/add.hpp"
#include "dxfio/edit/handles.hpp"
#include "dxfio/edit/space.hpp"
#include "dxfio/flags.hpp"
#include "dxfio/geometry/coordinates.hpp"
#include "dxfio/geometry/transform.hpp"
#include "dxfio/index.hpp"
#include "dxfio/library.hpp"
#include "dxfio/lineweight.hpp"
#include "dxfio/load.hpp"
#include "dxfio/save.hpp"
#include "dxfio/tables/face.hpp"
#include "dxfio/version.hpp"
#include "dxfio/view.hpp"

#endif /* DXFIO_DXFIO_HPP */
