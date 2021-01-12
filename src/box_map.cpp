#include "shiguredo/mp4/box_map.hpp"

#include <bits/exception.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box/unsupported.hpp"
#include "shiguredo/mp4/box_header.hpp"

namespace shiguredo::mp4 {

BoxDef::BoxDef(const BoxDefParameters& params) : factory(params.factory), versions(params.versions) {}

void BoxMap::addBoxDef(const BoxFactory& factory, const BoxType& box_type, const std::vector<std::uint8_t>& versions) {
  m_map[box_type] = std::make_unique<BoxDef>(BoxDefParameters{.factory = factory, .versions = versions});
}

Box* BoxMap::getBoxInstance(const BoxType& type) {
  auto it = m_map.find(type);
  Box* box;
  if (it == m_map.end()) {
    spdlog::debug("BoxMap::getBoxInstance(): box not found: {}", type.toString());
    box = new box::Unsupported();
  } else {
    box = it->second->factory();
    auto any_type_box = dynamic_cast<AnyTypeBox*>(box);
    if (any_type_box != nullptr) {
      any_type_box->setType(type);
    }
  }

  return box;
}

Box* BoxMap::getBoxInstance(BoxHeader* header) {
  Box* box = getBoxInstance(header->getType());
  box->setHeader(header);
  return box;
}

bool BoxMap::isSupported(const BoxType& box_type) const {
  return m_map.contains(box_type);
}

std::vector<std::uint8_t> BoxMap::getSupportedVersions(const BoxType& box_type) const {
  auto it = m_map.find(box_type);
  if (it == std::end(m_map)) {
    throw std::runtime_error("BoxMap::getSupportedVersions(): not found");
  }
  return it->second->versions;
}

bool BoxMap::isSupportedVersion(const BoxType& box_type, const std::uint8_t ver) const {
  auto it = m_map.find(box_type);
  if (it == std::end(m_map)) {
    return false;
  }
  auto vs = it->second->versions;
  if (std::empty(vs)) {
    return true;
  }
  return std::any_of(std::begin(vs), std::end(vs), [ver](const auto v) { return ver == v; });
}

}  // namespace shiguredo::mp4
