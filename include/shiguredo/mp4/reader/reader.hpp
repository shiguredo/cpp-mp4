#pragma once

#include <cstdint>
#include <istream>
#include <vector>

#include "shiguredo/mp4/box_map.hpp"

namespace shiguredo::mp4 {

class BoxInfo;

}

namespace shiguredo::mp4::reader {

class SimpleReader {
 public:
  explicit SimpleReader(std::istream&);
  ~SimpleReader();
  void read();

 private:
  std::istream& m_is;
  BoxMap m_box_map;
  std::vector<BoxInfo*> m_boxes;
  std::uint64_t m_total_size;

  std::uint64_t readBox(BoxInfo*);
};

}  // namespace shiguredo::mp4::reader
