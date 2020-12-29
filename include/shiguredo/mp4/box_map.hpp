#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

#include <boost/function.hpp>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4 {

class Box;
class BoxHeader;

typedef boost::function<Box*()> BoxFactory;

struct BoxDefParameters {
  const BoxFactory factory;
  const std::vector<std::uint8_t> versions;
};

class BoxDef {
 public:
  explicit BoxDef(const BoxDefParameters&);
  BoxFactory factory;
  std::vector<std::uint8_t> versions;
};

class BoxMap {
 public:
  BoxMap() = default;

  void addBoxDef(const BoxFactory&, const BoxType&, const std::vector<std::uint8_t>&);

  Box* getBoxInstance(const BoxType&);
  Box* getBoxInstance(BoxHeader*);
  bool isSupported(const BoxType& box_type) const;
  std::vector<std::uint8_t> getSupportedVersions(const BoxType&) const;

  bool isSupportedVersion(const BoxType&, const std::uint8_t) const;

 private:
  std::map<BoxType, std::unique_ptr<BoxDef>> m_map;
};

}  // namespace shiguredo::mp4
