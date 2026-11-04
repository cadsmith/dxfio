#include "fields/owned.hpp"
#include "fields/walk.hpp"
#include "tags/codes.hpp"
#include "unknown/io.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace DxfIO::Fields
{
auto Walk::Rest() const -> std::vector<Group>
{
  std::vector<Group> out;
  for (std::size_t index = 0; index < m_Body.size(); ++index)
  {
    if (!m_Used.at(index))
    {
      out.push_back(Owned(At(index), m_With.Text));
    }
  }
  return out;
}

void Walk::Spend()
{
  m_Used.assign(m_Body.size(), true);
}

auto Walk::Kept() -> std::vector<Group>
{
  std::vector<Group> out;
  bool beyond = false;
  const std::size_t first = Opening();
  for (std::size_t index = 0; index < m_Body.size(); ++index)
  {
    const Tags::Tag &one = At(index);
    if (m_Used.at(index))
    {
      continue;
    }
    if (std::optional<std::string> why = Dropped(index, one.Code, Standing{.Beyond = beyond, .Early = index < first}))
    {
      m_With.Tell(Issue{.Line = one.Line, .What = std::move(*why)});
      continue;
    }
    beyond = beyond ||
             (m_With.Release >= Version::R13 && (one.Code == Tags::Codes::Marker || one.Code == Tags::Codes::Embedded));
    out.push_back(Owned(one, m_With.Text));
  }
  return out;
}

auto Walk::Dropped(std::size_t at, int code, Standing where) -> std::optional<std::string>
{
  if (!m_Unfit.at(at) && m_Repeats.Repeated(at, code))
  {
    return std::format("group {} given again, one kept", code);
  }
  if (!where.Early && (where.Beyond || !m_Repeats.Trails(code)))
  {
    return std::nullopt;
  }
  if (m_Unfit.at(at))
  {
    return std::format("group {} holds no value its field takes", code);
  }
  // A group of a code the kind names, outside the part that names it: the record does not fit its layout.
  // A kind read through Taking is then kept as Unknown, whole; one read through Taken keeps its fields and
  // loses the group, and is told.
  m_Refused = true;
  return std::format("group {} out of its place, dropped", code);
}

auto Walk::Opening() const -> std::size_t
{
  if (m_With.Release < Version::R13)
  {
    return 0;
  }
  const auto found = std::ranges::find(m_Body, Tags::Codes::Marker, &Tags::Tag::Code);
  return found == m_Body.end() ? 0 : static_cast<std::size_t>(found - m_Body.begin());
}
} // namespace DxfIO::Fields
