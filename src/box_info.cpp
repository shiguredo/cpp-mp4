#include "shiguredo/mp4/box_info.hpp"

#include <bits/exception.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <iterator>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box/sample_entry.hpp"

namespace shiguredo::mp4 {

BoxInfo::BoxInfo(const BoxInfoParameters& params) : m_box(params.box) {
  if (params.parent) {
    m_path = params.parent->getPath();
    params.parent->addLeaf(this);
    if (params.parent->getType() == BoxType("wave")) {
      auto ase = dynamic_cast<box::AudioSampleEntry*>(m_box);
      if (ase) {
        spdlog::debug("BoxInfo::BoxInfo(): setUnderWave()");
        ase->setUnderWave(true);
      }
    }
  } else {
    m_path = {};
  }
  m_path.push_back(m_box->getType());
}

BoxInfo::~BoxInfo() {
  for (BoxInfo* l : m_leafs) {
    delete l;
  }
  delete m_box;
}

BoxPath BoxInfo::getPath() const {
  return m_path;
}

Box* BoxInfo::getBox() const {
  return m_box;
}

std::uint64_t BoxInfo::getSize() const {
  return m_box->getSize();
}

void BoxInfo::addLeaf(BoxInfo* info) {
  m_leafs.push_back(info);
}

std::string BoxInfo::toString() const {
  std::vector<std::string> leafs;
  std::transform(std::begin(m_leafs), std::end(m_leafs), std::back_inserter(leafs),
                 [](const BoxInfo* info) { return fmt::format("{}", info->toString()); });
  const std::string pad((std::size(m_path) - 1) * 2, ' ');
  if (std::empty(leafs)) {
    return fmt::format("{}{}", pad, m_box->toString());
  }
  return fmt::format("{}{}\n{}", pad, m_box->toString(), fmt::join(leafs, "\n"));
}

std::uint64_t BoxInfo::adjustOffsetAndSize(const std::uint64_t offset) {
  std::uint64_t leafs_size = 0;
  const std::uint64_t header_size = m_box->getHeaderSize();
  const std::uint64_t data_size = m_box->getDataSize();
  for (auto l : m_leafs) {
    leafs_size += l->adjustOffsetAndSize(offset + header_size + data_size + leafs_size);
  }

  m_box->setOffsetAndDataSize(offset, data_size + leafs_size);

  if (header_size != m_box->getHeaderSize()) {
    adjustOffsetAndSize(offset);
  }
  return m_box->getSize();
}

void BoxInfo::write(std::ostream& os) const {
  spdlog::trace("BoxInfo::write(): {}", m_box->toString());
  m_box->write(os);
  for (auto l : m_leafs) {
    l->write(os);
  }
}

BoxType BoxInfo::getType() const {
  return m_box->getType();
}

}  // namespace shiguredo::mp4
