#pragma once

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::bitio {

class Reader;
class Writer;

}  // namespace shiguredo::mp4::bitio

namespace shiguredo::mp4::box {

class Descriptor {
 public:
  virtual ~Descriptor() = default;

  virtual std::uint64_t writeData(bitio::Writer*) const = 0;
  virtual std::uint64_t getSize() const = 0;
  virtual std::uint32_t getDataSize() const = 0;
  virtual std::uint32_t getDataSizeWithSubDescriptors() const = 0;
  virtual std::uint64_t readData(bitio::Reader*) = 0;
  virtual std::string toString() const = 0;

 protected:
  std::int8_t m_tag;
};

const std::int8_t ESDescrTag = 0x03;
const std::int8_t DecoderConfigDescrTag = 0x04;
const std::int8_t DecSpecificInfoTag = 0x05;
const std::int8_t SLConfigDescrTag = 0x06;

struct ESDescriptorParameters {
  const std::uint32_t sub_descriptors_size = 0;
  const std::uint16_t ESID = 0;
  const bool stream_dependence_flag = false;
  const bool url_flag = false;
  const bool ocr_stream_flag = false;
  const std::int8_t stream_priority = 0;
  const std::uint16_t depends_on_esid = 0;
  const std::vector<std::uint8_t> url_string = {};
  const std::uint16_t OCRESID = 0;
};

class ESDescriptor : public Descriptor {
 public:
  ESDescriptor();
  explicit ESDescriptor(const ESDescriptorParameters&);

  std::uint64_t writeData(bitio::Writer*) const override;
  std::uint64_t getSize() const override;
  std::uint32_t getDataSize() const override;
  std::uint32_t getDataSizeWithSubDescriptors() const override;
  std::uint64_t readData(bitio::Reader*) override;
  std::string toString() const override;
  void addSubDescriptor(std::shared_ptr<Descriptor>);

 private:
  std::uint32_t m_sub_descriptors_size;
  std::uint16_t m_ESID;
  bool m_stream_dependence_flag;
  bool m_url_flag;
  bool m_ocr_stream_flag;
  std::int8_t m_stream_priority;
  std::uint16_t m_depends_on_esid;
  std::vector<std::uint8_t> m_url_string;
  std::uint16_t m_OCRESID;
  std::vector<std::shared_ptr<Descriptor>> m_sub_descriptors = {};
};

struct DecoderConfigDescriptorParameters {
  const std::uint32_t sub_descriptors_size = 0;
  const std::uint8_t object_type_indication;
  const std::int8_t stream_type;
  const bool upstream = false;
  const bool reserved = true;
  const std::uint32_t buffer_size_db;
  const std::uint32_t max_bitrate;
  const std::uint32_t avg_bitrate;
};

class DecoderConfigDescriptor : public Descriptor {
 public:
  DecoderConfigDescriptor();
  explicit DecoderConfigDescriptor(const DecoderConfigDescriptorParameters&);

  std::uint64_t writeData(bitio::Writer*) const override;
  std::uint64_t getSize() const override;
  std::uint32_t getDataSize() const override;
  std::uint32_t getDataSizeWithSubDescriptors() const override;
  std::uint64_t readData(bitio::Reader*) override;
  std::string toString() const override;
  void addSubDescriptor(std::shared_ptr<Descriptor>);

 private:
  std::uint32_t m_sub_descriptors_size = 0;
  std::uint8_t m_object_type_indication;
  std::int8_t m_stream_type;
  bool m_upstream;
  bool m_reserved;
  std::uint32_t m_buffer_size_db;
  std::uint32_t m_max_bitrate;
  std::uint32_t m_avg_bitrate;
  std::vector<std::shared_ptr<Descriptor>> m_sub_descriptors = {};
};

struct DecSpecificInfoParameters {
  const std::vector<std::uint8_t> data;
};

class DecSpecificInfo : public Descriptor {
 public:
  DecSpecificInfo();
  explicit DecSpecificInfo(const DecSpecificInfoParameters&);

  std::uint64_t writeData(bitio::Writer*) const override;
  std::uint64_t getSize() const override;
  std::uint32_t getDataSize() const override;
  std::uint32_t getDataSizeWithSubDescriptors() const override;
  std::uint64_t readData(bitio::Reader*) override;
  std::string toString() const override;

 private:
  std::vector<std::uint8_t> m_data;
};

struct SLConfigDescrParameters {
  const std::vector<std::uint8_t> data;
};

class SLConfigDescr : public Descriptor {
 public:
  SLConfigDescr();
  explicit SLConfigDescr(const SLConfigDescrParameters&);

  std::uint64_t writeData(bitio::Writer*) const override;
  std::uint64_t getSize() const override;
  std::uint32_t getDataSize() const override;
  std::uint32_t getDataSizeWithSubDescriptors() const override;
  std::uint64_t readData(bitio::Reader*) override;
  std::string toString() const override;

 private:
  std::vector<std::uint8_t> m_data;
};

BoxType box_type_esds();

struct EsdsParameters {
  const std::vector<std::shared_ptr<Descriptor>> descriptors;
};

class Esds : public FullBox {
 public:
  Esds();
  explicit Esds(const EsdsParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::vector<std::shared_ptr<Descriptor>> m_descriptors;
};

}  // namespace shiguredo::mp4::box
