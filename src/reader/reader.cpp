#include "shiguredo/mp4/reader/reader.hpp"

#include <bits/exception.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <istream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_header.hpp"
#include "shiguredo/mp4/box_info.hpp"
#include "shiguredo/mp4/box_map.hpp"
#include "shiguredo/mp4/box_types.hpp"

namespace shiguredo::mp4::reader {

SimpleReader::SimpleReader(std::istream& t_is) : m_is(t_is) {
  register_box_map(&m_box_map);
  m_is.seekg(0, std::ios_base::end);
  m_total_size = static_cast<std::uint64_t>(m_is.tellg());
  m_is.seekg(0, std::ios_base::beg);
  spdlog::debug("SimpleReader::SimpleReader(): m_total_size={}", m_total_size);
}

SimpleReader::~SimpleReader() {
  for (BoxInfo* i : m_boxes) {
    delete i;
  }
}

void SimpleReader::read() {
  std::uint64_t size = 0;
  while (size < m_total_size) {
    size += readBox(nullptr);
  }
  for (BoxInfo* i : m_boxes) {
    std::cout << i->toString() << std::endl;
  }
}

std::uint64_t SimpleReader::readBox(BoxInfo* parent) {
  BoxHeader* header = read_box_header(m_is);
  header->seekToData(m_is);
  std::stringstream ss;
  const auto remaining_size = m_total_size - header->getOffset() - header->getHeaderSize();
  if (header->getDataSize() > remaining_size) {
    spdlog::trace("SimpleReader::readBox(): corrupt file? Header={}", header->toString());
    throw std::runtime_error(fmt::format("corrupt file? box data size({}) is greater than remaining_size({})",
                                         header->getDataSize(), remaining_size));
  }
  std::copy_n(std::istreambuf_iterator<char>(m_is), header->getDataSize(), std::ostreambuf_iterator<char>(ss));

  spdlog::trace("SimpleReader::readBox(): Header={}", header->toString());

  Box* box = m_box_map.getBoxInstance(header);
  if (box == nullptr) {
    throw std::runtime_error("SimpleReader::readBox(): BoxMap::getBoxInstance() returns nullptr");
  }
  BoxInfo* info = new BoxInfo({.parent = parent, .box = box});
  if (!parent) {
    m_boxes.push_back(info);
  }

  const auto rbits = box->readData(ss);
  std::uint64_t rbytes = rbits / 8;
  m_is.seekg(static_cast<std::streamoff>(header->getOffset() + header->getHeaderSize() + rbytes));
  if (!m_is.good()) {
    throw std::runtime_error(
        fmt::format("SimpleReader::readBox(): istream::seekg() failed: rdstate={}", m_is.rdstate()));
  }

  while (header->getDataSize() > rbytes) {
    rbytes += readBox(info);
  }
  return header->getSize();
}

}  // namespace shiguredo::mp4::reader
