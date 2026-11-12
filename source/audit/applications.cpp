#include "checks.hpp"
#include "header/io.hpp"
#include "names.hpp"
#include "walk.hpp"

#include "dxfio/create.hpp"
#include "dxfio/tables/appid.hpp"
#include "dxfio/xdata.hpp"

#include <algorithm>
#include <format>
#include <string>
#include <utility>

namespace DxfIO::Audit
{
  namespace
  {
    /**
     *  Registers an application for the extended data that names it: a record of the APPID table,
     *  owned by it.
     */
    void Registered(Document &drawing, const std::string &name, Handle self)
    {
      Tables::Table<Tables::Appid> &table = drawing.Appid;
      Tables::Appid                 made{};
      made.Record.Name       = name;
      made.Record.Base.Self  = self;
      made.Record.Base.Owner = table.Head.Base.Self;
      table.Records.push_back(std::move(made));
      table.Head.Count = std::max(table.Head.Count, static_cast<int>(table.Records.size()));
    }
  } // namespace

  /**
   *  AutoCAD discards a drawing whose extended data names an application the APPID table does not
   *  register: each such application is an error, told once, and registered when the checks fix.
   *  One the release requires anyway is the required records' finding, not this one.
   */
  template <class DocumentT>
  void Applied(Auditor<DocumentT> &with)
  {
    Names known = Named(with.Drawing->Appid);
    for (const Tables::Appid &one : Create(Header::Declared(with.Drawing->Header)).Appid.Records)
    {
      known.emplace(one.Record.Name);
    }
    Bases(*with.Drawing,
          [&with, &known](auto &base) -> void
          {
            for (const Xdata &one : base.Extended)
            {
              if (known.contains(one.Application))
              {
                continue;
              }
              with.Note(Severity::Error,
                        std::format("application {} not registered", one.Application), base.Self);
              known.emplace(one.Application);
              if constexpr (Auditor<DocumentT>::Fix)
              {
                const Handle self = with.Drawing->Appid.Head.Base.Self ? with.Take() : Handle{};
                Registered(*with.Drawing, one.Application, self);
              }
            }
          });
  }

  template void Applied(Auditor<Document> &with);

  template void Applied(Auditor<const Document> &with);
} // namespace DxfIO::Audit
