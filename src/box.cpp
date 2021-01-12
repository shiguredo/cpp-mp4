#include "shiguredo/mp4/box.hpp"

#include <fmt/core.h>

#include <array>
#include <cstdint>
#include <istream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "shiguredo/mp4/bitio/bitio.hpp"
#include "shiguredo/mp4/box_header.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::bitio {

class Reader;
class Writer;

}  // namespace shiguredo::mp4::bitio

namespace shiguredo::mp4 {

Box::~Box() {
  if (m_header) {
    delete m_header;
  }
}

std::uint8_t Box::getVersion() const {
  return 0;
}

void Box::setVersion(std::uint8_t) {}

std::uint32_t Box::getFlags() const {
  return 0x000000;
}

bool Box::checkFlag(std::uint32_t) const {
  return true;
}

void Box::setFlags(std::uint32_t) {}

void Box::addFlag(std::uint32_t) {}

void Box::removeFlag(std::uint32_t) {}

BoxType Box::getType() const {
  return m_type;
}

std::uint64_t Box::getDataSize() const {
  return 0;
}

void Box::setHeader(BoxHeader* header) {
  m_header = header;
}

std::string Box::toString() const {
  auto only_data = toStringOnlyData();
  if (!std::empty(only_data)) {
    only_data = " " + only_data;
  }
  if (m_header) {
    return fmt::format("{}{}", m_header->toString(), only_data);
  }
  return fmt::format("header is not set:{}", only_data);
}

std::uint64_t Box::getHeaderSize() {
  makeHeader();
  return m_header->getHeaderSize();
}

void Box::seekToData(std::istream& is) {
  is.seekg(static_cast<std::streamoff>(m_header->getOffset() + m_header->getHeaderSize()), std::ios_base::beg);
  if (!is.good()) {
    throw std::runtime_error(fmt::format("Box::seekToData(): istream::seekg() failed: rdstate={}", is.rdstate()));
  }
}

void Box::seekToEnd(std::istream& is) {
  is.seekg(static_cast<std::streamoff>(m_header->getOffset() + m_header->getSize()), std::ios_base::beg);
  if (!is.good()) {
    throw std::runtime_error(fmt::format("Box::seekToEnd(): istream::seekg() failed: rdstate={}", is.rdstate()));
  }
}

std::uint64_t Box::getSize() const {
  if (m_header) {
    return m_header->getSize();
  }
  throw std::logic_error("Box::getSize(): header is not set");
}

void Box::setOffsetAndDataSize(const std::uint64_t offset, const std::uint64_t data_size) {
  makeHeader();
  m_header->setOffsetAndDataSize(offset, data_size);
}

std::uint64_t Box::write(std::ostream& os) {
  return m_header->write(os) + writeData(os);
}

void Box::makeHeader() {
  if (m_header) {
    return;
  }
  m_header = new BoxHeader({.type = m_type});
}

std::uint8_t FullBox::getVersion() const {
  return m_version;
}

void FullBox::setVersion(std::uint8_t version) {
  m_version = version;
}

std::uint32_t FullBox::getFlags() const {
  std::uint32_t flag = static_cast<std::uint32_t>(m_flags[0]) << 16;
  flag |= static_cast<std::uint32_t>(m_flags[1]) << 8;
  flag |= static_cast<std::uint32_t>(m_flags[2]);
  return flag;
}

std::string FullBox::getVersionAndFlagsString() const {
  return fmt::format("Version={} Flags=0x{:02x}{:02x}{:02x}", m_version, m_flags[0], m_flags[1], m_flags[2]);
}

bool FullBox::checkFlag(std::uint32_t flag) const {
  return (getFlags() & flag) != 0;
}

void FullBox::setFlags(std::uint32_t flag) {
  m_flags[0] = static_cast<std::uint8_t>((flag >> 16) & 0xff);
  m_flags[1] = static_cast<std::uint8_t>((flag >> 8) & 0xff);
  m_flags[2] = static_cast<std::uint8_t>(flag & 0xff);
}

void FullBox::addFlag(std::uint32_t flag) {
  setFlags(getFlags() | flag);
}

void FullBox::removeFlag(std::uint32_t flag) {
  setFlags(getFlags() ^ flag);
}

std::uint64_t FullBox::writeVersionAndFlag(bitio::Writer* writer) const {
  std::uint64_t wbits = bitio::write_uint<std::uint8_t>(writer, m_version);
  for (std::size_t i = 0; i < 3; ++i) {
    wbits += bitio::write_uint<std::uint8_t>(writer, m_flags[i]);
  }
  return wbits;
}

std::uint64_t FullBox::readVersionAndFlag(bitio::Reader* reader) {
  std::uint64_t rbits = bitio::read_uint<std::uint8_t>(reader, &m_version);
  for (std::size_t i = 0; i < 3; ++i) {
    rbits += bitio::read_uint<std::uint8_t>(reader, &m_flags[i]);
  }
  return rbits;
}

std::uint64_t FullBox::getDataSize() const {
  return 4;
}

void AnyTypeBox::setType(const BoxType& type) {
  m_type = type;
}

BoxType AnyTypeBox::getType() const {
  return m_type;
}

}  // namespace shiguredo::mp4
