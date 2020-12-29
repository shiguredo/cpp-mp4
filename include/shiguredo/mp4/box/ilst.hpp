#pragma once

#include <cstdint>
#include <istream>
#include <string>

#include "shiguredo/mp4/box.hpp"
#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4::box {

struct IlstMetaParameters {
  const BoxType type;
};

class IlstMeta : public AnyTypeBox {
 public:
  IlstMeta() = default;
  explicit IlstMeta(const IlstMetaParameters&);
  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t readData(std::istream&) override;
};

BoxType box_type_ilst();

class Ilst : public Box {
 public:
  Ilst();

  std::string toStringOnlyData() const override;

  std::uint64_t writeData(std::ostream&) const override;
  std::uint64_t readData(std::istream&) override;
};

}  // namespace shiguredo::mp4::box
