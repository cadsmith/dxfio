#include "dxfio/index.hpp"

namespace DxfIO
{
  Index::Index(const Drawing &drawing) : m_Drawing{&drawing}
  {
    for (const Entity &one : drawing.Entities)
    {
      m_Entities.try_emplace(BaseOf(one).Self.Value, &one);
    }
    for (const auto &block : drawing.Blocks)
    {
      for (const Entity &one : block.Entities)
      {
        m_Entities.try_emplace(BaseOf(one).Self.Value, &one);
      }
    }
    for (const Object &one : drawing.Objects)
    {
      m_Objects.try_emplace(BaseOf(one).Self.Value, &one);
    }
  }

  auto Index::Drawn() const -> const Drawing &
  {
    return *m_Drawing;
  }

  auto Index::FindEntity(Handle self) const -> const Entity *
  {
    const auto found = self ? m_Entities.find(self.Value) : m_Entities.end();
    return found == m_Entities.end() ? nullptr : found->second;
  }

  auto Index::FindObject(Handle self) const -> const Object *
  {
    const auto found = self ? m_Objects.find(self.Value) : m_Objects.end();
    return found == m_Objects.end() ? nullptr : found->second;
  }
} // namespace DxfIO
