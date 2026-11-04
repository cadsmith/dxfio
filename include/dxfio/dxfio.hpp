#ifndef DXFIO_DXFIO_HPP
#define DXFIO_DXFIO_HPP

/// dxfio reads and writes DXF, every release from R10 to R2018, ASCII and binary.
/// This header brings in the whole public interface: Load, Save, Create, the Document they work on,
/// the editing helpers of DxfIO::Edit and the audit of DxfIO::Audit.

#include "dxfio/audit/audit.hpp"
#include "dxfio/create.hpp"
#include "dxfio/document.hpp"
#include "dxfio/edit/add.hpp"
#include "dxfio/edit/find.hpp"
#include "dxfio/edit/handles.hpp"
#include "dxfio/edit/space.hpp"
#include "dxfio/load.hpp"
#include "dxfio/save.hpp"
#include "dxfio/tables/face.hpp"
#include "dxfio/version.hpp"

#endif /* DXFIO_DXFIO_HPP */
