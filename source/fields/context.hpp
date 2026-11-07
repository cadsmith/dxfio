#ifndef DXFIO_FIELDS_CONTEXT_HPP
#define DXFIO_FIELDS_CONTEXT_HPP

#include "encoding/encoding.hpp"
#include "tags/writer.hpp"

#include "dxfio/api.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/version.hpp"

#include <utility>
#include <vector>

namespace DxfIO::Fields
{
struct Scratch;

struct DXFIO_API Reading
{
  Version Release{Version::R2018};
  Encoding::Scheme Text{Encoding::Utf8()};
  std::vector<Issue> *Issues{nullptr};
  // Room kept from one record to the next, none when each record finds its own.
  Scratch *Reuse{nullptr};
  // The threads the records of a family are read on, one for all of them in their order.
  int Threads{1};

  void Tell(Issue issue) const
  {
    if (Issues != nullptr)
    {
      Issues->push_back(std::move(issue));
    }
  }
};

struct DXFIO_API Writing
{
  Tags::Writer *To{nullptr};
  Version Release{Version::R2018};
  Encoding::Scheme Text{Encoding::Utf8()};
  Version From{Version::R2018};
  // The threads the records of a family are written on, one for all of them in their order.
  int Threads{1};
};
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_CONTEXT_HPP */
