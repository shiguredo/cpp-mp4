#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct EmsgParameters {
  const std::uint8_t version = 0;
  const std::uint32_t flags = 0x000000;
  const std::string scheme_uri;
  const std::string value;
  const std::uint32_t timescale;
  const std::uint64_t presentation_time;
  const std::uint32_t event_duration;
  const std::uint32_t id;
  const std::vector<std::uint8_t> message_data;
};

BoxType box_type_emsg();

class Emsg : public FullBox {
 public:
  Emsg();
  explicit Emsg(const EmsgParameters&);

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream& os) const override;
  std::uint64_t getDataSize() const override;
  std::uint64_t readData(std::istream& is) override;

 private:
  std::string m_scheme_uri;
  std::string m_value;
  std::uint32_t m_timescale;
  std::uint64_t m_presentation_time;
  std::uint32_t m_event_duration;
  std::uint32_t m_id;
  std::vector<std::uint8_t> m_message_data;
};

}  // namespace shiguredo::mp4::box
