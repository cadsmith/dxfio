#include "../masked.hpp"
#include "ascii.hpp"
#include "checks.hpp"
#include "entities/io.hpp"

#include "dxfio/blocks/blocks.hpp"
#include "dxfio/entity.hpp"

#include <algorithm>
#include <format>
#include <string_view>

namespace DxfIO::Audit
{
  namespace
  {
    constexpr int Holding = 2;

    constexpr std::string_view Model = "*Model_Space";

    constexpr std::string_view Paper = "*Paper_Space";

    /**
     *  Whether a block holds an ATTDEF that is not constant: in its own entities, or, for a space's
     *  block, in those of the ENTITIES section drawn in that space.
     */
    auto Defining(const Document &drawing, const Blocks::Block &block) -> bool
    {
      if (std::ranges::any_of(block.Entities, Entities::Attributing))
      {
        return true;
      }
      const bool paper = Ascii::Same(block.Begin.Name, Paper);
      if (!paper && !Ascii::Same(block.Begin.Name, Model))
      {
        return false;
      }
      return std::ranges::any_of(
          drawing.Entities, [paper](const Entity &one) -> bool
          { return Entities::Paper(one) == paper && Entities::Attributing(one); });
    }
  } // namespace

  /**
   *  AutoCAD's INSERT and ATTSYNC ignore a block's attribute definitions unless its BLOCK is
   *  flagged as holding them: a block holding one and not flagged so is a warning, and flagged when
   *  the checks fix.
   */
  template <class DocumentT>
  void Attributed(Auditor<DocumentT> &with)
  {
    for (auto &block : with.Drawing->Blocks)
    {
      if (Masked(block.Begin.Flags, Holding) != 0 || !Defining(*with.Drawing, block))
      {
        continue;
      }
      with.Note(
          Severity::Warning,
          std::format("block {} holds attribute definitions, not flagged so", block.Begin.Name),
          block.Begin.Record.Base.Self);
      if constexpr (Auditor<DocumentT>::Fix)
      {
        block.Begin.Flags += Holding;
      }
    }
  }

  template void Attributed(Auditor<Document> &with);

  template void Attributed(Auditor<const Document> &with);
} // namespace DxfIO::Audit
