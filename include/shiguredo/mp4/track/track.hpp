#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace shiguredo::mp4 {

class BoxInfo;
class Box;

namespace writer {

class Writer;

}

namespace box {

class SttsEntry;
class StscEntry;

}  // namespace box

}  // namespace shiguredo::mp4

namespace shiguredo::mp4::track {

struct ChunkInfo {
  std::uint64_t offset = 0;
  std::uint32_t number_of_samples = 0;
  bool initialized = false;
};

enum HandlerType {
  vide,
  soun,
};

class Track {
 public:
  virtual ~Track() = default;
  void addMdatData(const std::uint64_t timestamp, const std::vector<std::uint8_t>&, bool is_key);
  void addMdatData(const std::uint64_t timestamp, const std::uint8_t*, const std::size_t, bool is_key);
  virtual void appendTrakBoxInfo(BoxInfo*) = 0;
  void setMediaTime(const std::int64_t);
  std::uint64_t getTimescale() const;
  void resetChunkOffsets(std::uint64_t);
  void terminateCurrentChunk();

 protected:
  std::uint32_t m_timescale;
  float m_duration;
  std::uint32_t m_mvhd_timescale;
  std::uint64_t m_time_from_epoch;
  std::int64_t m_media_time;
  std::uint32_t m_track_id;
  shiguredo::mp4::writer::Writer* m_writer;
  bool m_finalized = false;

  HandlerType m_handler_type;

  std::vector<std::uint32_t> m_mdat_sample_sizes = {};
  std::vector<ChunkInfo> m_chunk_infos = {};
  ChunkInfo m_current_chunk_info;
  std::uint64_t m_prev_timestamp = 0;
  std::vector<std::uint32_t> m_sample_durations = {};
  std::vector<std::uint32_t> m_key_sample_numbers = {};

  void finalize();
  BoxInfo* makeTrakBoxInfo(BoxInfo*);
  virtual void makeTkhdBoxInfo(BoxInfo*) = 0;
  BoxInfo* makeEdtsBoxInfo(BoxInfo*);
  void makeElstBoxInfo(BoxInfo*);
  BoxInfo* makeMdiaBoxInfo(BoxInfo*);
  void makeMdhdBoxInfo(BoxInfo*);
  void makeHdlrBoxInfo(BoxInfo*, const std::string& name = "shiguredo::mp4");
  BoxInfo* makeMinfBoxInfo(BoxInfo*);
  BoxInfo* makeStblBoxInfo(BoxInfo*);
  void makeDinfBoxInfo(BoxInfo*);
  void makeSttsBoxInfo(BoxInfo*);
  void makeStscBoxInfo(BoxInfo*);
  void makeStssBoxInfo(BoxInfo*);
  void makeStszBoxInfo(BoxInfo*);
  void makeOffsetBoxInfo(BoxInfo*);
  std::array<std::uint8_t, 4> getHandlerTypeArray();

  std::uint64_t getDurationInMvhdTimescale() const;
  std::uint64_t getDurationInTimescale() const;
};

void make_stts_entries(std::vector<box::SttsEntry>*, const std::vector<std::uint32_t>&);
void make_stsc_entries(std::vector<box::StscEntry>* entries, const std::vector<ChunkInfo>& chunk_offsets);
Box* make_offset_box(const std::vector<ChunkInfo>&);

}  // namespace shiguredo::mp4::track
