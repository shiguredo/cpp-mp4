#pragma once

#include <array>
#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::bitio {

class Reader;
class Writer;

}  // namespace shiguredo::mp4::bitio

namespace shiguredo::mp4 {

class BoxHeader;

class Box {
 public:
  virtual ~Box();
  virtual std::uint8_t getVersion() const;
  virtual void setVersion(std::uint8_t);
  virtual std::uint32_t getFlags() const;
  virtual bool checkFlag(std::uint32_t) const;
  virtual void setFlags(std::uint32_t);
  virtual void addFlag(std::uint32_t);
  virtual void removeFlag(std::uint32_t);
  BoxType getType() const;
  virtual std::string toStringOnlyData() const = 0;
  std::string toString() const;
  std::uint64_t write(std::ostream&);
  virtual std::uint64_t writeData(std::ostream&) const = 0;
  virtual std::uint64_t readData(std::istream&) = 0;
  std::uint64_t getHeaderSize();
  virtual std::uint64_t getDataSize() const;
  std::uint64_t getSize() const;

  void setHeader(BoxHeader*);
  void seekToData(std::istream& is);
  void seekToEnd(std::istream& is);
  void setOffsetAndDataSize(const std::uint64_t, const std::uint64_t);

  auto operator<=>(const Box&) const = default;

 protected:
  BoxType m_type;
  BoxHeader* m_header = nullptr;

 private:
  void makeHeader();
};

class FullBox : public Box {
 public:
  std::uint8_t getVersion() const override;
  void setVersion(std::uint8_t version) override;
  std::uint32_t getFlags() const override;
  std::string getVersionAndFlagsString() const;
  bool checkFlag(std::uint32_t flag) const override;
  void setFlags(std::uint32_t flag) override;
  void addFlag(std::uint32_t flag) override;
  void removeFlag(std::uint32_t flag) override;
  std::uint64_t writeVersionAndFlag(bitio::Writer*) const;
  std::uint64_t readVersionAndFlag(bitio::Reader*);
  std::uint64_t getDataSize() const override;

  auto operator<=>(const FullBox&) const = default;

 protected:
  std::uint8_t m_version = 0;
  std::array<std::uint8_t, 3> m_flags = {0, 0, 0};
};

class AnyTypeBox : public Box {
 public:
  void setType(const BoxType&);
  BoxType getType() const;

  auto operator<=>(const AnyTypeBox&) const = default;
};

}  // namespace shiguredo::mp4
