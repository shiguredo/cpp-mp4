#pragma once

#include <array>
#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::bitio {

class Reader;
class Writer;

}  // namespace shiguredo::mp4::bitio

namespace shiguredo::mp4::box {

struct PsshKIDParameters {
  const std::array<std::uint8_t, 16> kid;
};

class PsshKID {
 public:
  PsshKID() = default;
  explicit PsshKID(const PsshKIDParameters&);
  std::string toString() const;
  std::uint64_t writeData(bitio::Writer*) const;
  std::uint64_t readData(bitio::Reader*);

 private:
  std::array<std::uint8_t, 16> m_kid;
};

struct PsshParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::array<std::uint8_t, 16> system_id;
  const std::vector<PsshKID> kids = {};
  const std::vector<std::uint8_t> data;
};

BoxType box_type_pssh();

class Pssh : public FullBox {
 public:
  Pssh();
  explicit Pssh(const PsshParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream&) override;

 private:
  std::array<std::uint8_t, 16> m_system_id;
  std::vector<PsshKID> m_kids = {};
  std::vector<std::uint8_t> m_data;
};

}  // namespace shiguredo::mp4::box
