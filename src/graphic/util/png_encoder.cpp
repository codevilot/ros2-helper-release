#include "ros2_helper/graphic/util/png_encoder.hpp"

#include <algorithm>

namespace ros2_helper
{
namespace
{

void append_u32_be(std::vector<uint8_t> & out, uint32_t value)
{
  out.push_back(static_cast<uint8_t>((value >> 24) & 0xff));
  out.push_back(static_cast<uint8_t>((value >> 16) & 0xff));
  out.push_back(static_cast<uint8_t>((value >> 8) & 0xff));
  out.push_back(static_cast<uint8_t>(value & 0xff));
}

uint32_t crc32(const uint8_t * data, size_t size)
{
  uint32_t crc = 0xffffffffU;
  for (size_t i = 0; i < size; ++i) {
    crc ^= data[i];
    for (int bit = 0; bit < 8; ++bit) {
      crc = (crc >> 1) ^ (0xedb88320U & (0U - (crc & 1U)));
    }
  }
  return crc ^ 0xffffffffU;
}

uint32_t adler32(const std::vector<uint8_t> & data)
{
  constexpr uint32_t mod = 65521;
  uint32_t a = 1;
  uint32_t b = 0;
  for (const uint8_t byte : data) {
    a = (a + byte) % mod;
    b = (b + a) % mod;
  }
  return (b << 16) | a;
}

void append_chunk(std::vector<uint8_t> & png, const char type[4], const std::vector<uint8_t> & data)
{
  append_u32_be(png, static_cast<uint32_t>(data.size()));
  const size_t crc_start = png.size();
  png.insert(png.end(), type, type + 4);
  png.insert(png.end(), data.begin(), data.end());
  append_u32_be(png, crc32(png.data() + crc_start, png.size() - crc_start));
}

std::vector<uint8_t> zlib_store(const std::vector<uint8_t> & raw)
{
  std::vector<uint8_t> out;
  out.push_back(0x78);
  out.push_back(0x01);

  size_t offset = 0;
  while (offset < raw.size()) {
    const size_t block_size = std::min<size_t>(65535, raw.size() - offset);
    const bool final_block = offset + block_size == raw.size();
    out.push_back(final_block ? 0x01 : 0x00);
    const uint16_t len = static_cast<uint16_t>(block_size);
    const uint16_t nlen = static_cast<uint16_t>(~len);
    out.push_back(static_cast<uint8_t>(len & 0xff));
    out.push_back(static_cast<uint8_t>((len >> 8) & 0xff));
    out.push_back(static_cast<uint8_t>(nlen & 0xff));
    out.push_back(static_cast<uint8_t>((nlen >> 8) & 0xff));
    out.insert(out.end(), raw.begin() + offset, raw.begin() + offset + block_size);
    offset += block_size;
  }

  append_u32_be(out, adler32(raw));
  return out;
}

}  // namespace

std::vector<uint8_t> encode_png_rgb(int width, int height, const std::vector<uint8_t> & rgb)
{
  std::vector<uint8_t> raw;
  raw.reserve(static_cast<size_t>(height) * (1 + static_cast<size_t>(width) * 3));
  for (int y = 0; y < height; ++y) {
    raw.push_back(0);
    const size_t row_start = static_cast<size_t>(y) * width * 3;
    raw.insert(raw.end(), rgb.begin() + row_start, rgb.begin() + row_start + static_cast<size_t>(width) * 3);
  }

  std::vector<uint8_t> png = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};

  std::vector<uint8_t> ihdr;
  append_u32_be(ihdr, static_cast<uint32_t>(width));
  append_u32_be(ihdr, static_cast<uint32_t>(height));
  ihdr.push_back(8);
  ihdr.push_back(2);
  ihdr.push_back(0);
  ihdr.push_back(0);
  ihdr.push_back(0);
  append_chunk(png, "IHDR", ihdr);
  append_chunk(png, "IDAT", zlib_store(raw));
  append_chunk(png, "IEND", {});
  return png;
}

}  // namespace ros2_helper
