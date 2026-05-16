#include "base85ed.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

constexpr char ALPHABET[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";

constexpr std::size_t BASE = 85;
constexpr std::size_t RAW_BLOCK_SIZE = 4;
constexpr std::size_t ENCODED_BLOCK_SIZE = 5;
constexpr std::uint64_t MAX_UINT32 = 0xFFFFFFFFULL;

std::array<int, 256> make_decode_table() {
    std::array<int, 256> table{};
    table.fill(-1);

    for (std::size_t i = 0; i < BASE; ++i) {
        table[static_cast<unsigned char>(ALPHABET[i])] = static_cast<int>(i);
    }

    return table;
}

const std::array<int, 256>& decode_table() {
    static const std::array<int, 256> table = make_decode_table();
    return table;
}

void append_encoded_block(
    std::vector<std::uint8_t>& out,
    std::uint32_t value,
    std::size_t chars_to_write
) {
    std::array<std::uint8_t, ENCODED_BLOCK_SIZE> encoded{};

    for (std::size_t i = ENCODED_BLOCK_SIZE; i > 0; --i) {
        encoded[i - 1] = static_cast<std::uint8_t>(ALPHABET[value % BASE]);
        value /= BASE;
    }

    out.insert(out.end(), encoded.begin(), encoded.begin() + chars_to_write);
}

}

std::vector<std::uint8_t> base85::encode(const std::vector<std::uint8_t>& bytes) {
    std::vector<std::uint8_t> result;
    result.reserve(
        (bytes.size() + RAW_BLOCK_SIZE - 1) / RAW_BLOCK_SIZE * ENCODED_BLOCK_SIZE
    );

    for (std::size_t offset = 0; offset < bytes.size(); offset += RAW_BLOCK_SIZE) {
        const std::size_t block_size =
            std::min(RAW_BLOCK_SIZE, bytes.size() - offset);

        std::uint32_t value = 0;

        for (std::size_t i = 0; i < RAW_BLOCK_SIZE; ++i) {
            value <<= 8;

            if (i < block_size) {
                value |= bytes[offset + i];
            }
        }

        append_encoded_block(result, value, block_size + 1);
    }
    return result;
}

std::vector<std::uint8_t> base85::decode(const std::vector<std::uint8_t>& b85str) {
    std::vector<std::uint8_t> result;
    result.reserve(b85str.size() / ENCODED_BLOCK_SIZE * RAW_BLOCK_SIZE);
    const auto& table = decode_table();
    for (std::size_t offset = 0; offset < b85str.size(); offset += ENCODED_BLOCK_SIZE) {
        const std::size_t block_size =
            std::min(ENCODED_BLOCK_SIZE, b85str.size() - offset);
        const std::size_t padding = ENCODED_BLOCK_SIZE - block_size;
        std::uint64_t value = 0;
        for (std::size_t i = 0; i < ENCODED_BLOCK_SIZE; ++i) {
            const std::uint8_t ch =
                i < block_size ? b85str[offset + i] : static_cast<std::uint8_t>('~');
            const int digit = table[ch];
            if (digit < 0) {
                throw std::invalid_argument("bad base85 character");
            }
            value = value * BASE + static_cast<std::uint64_t>(digit);
        }
        if (value > MAX_UINT32) {
            throw std::invalid_argument("base85 overflow");
        }
        std::array<std::uint8_t, RAW_BLOCK_SIZE> decoded{
            static_cast<std::uint8_t>((value >> 24) & 0xFF),
            static_cast<std::uint8_t>((value >> 16) & 0xFF),
            static_cast<std::uint8_t>((value >> 8) & 0xFF),
            static_cast<std::uint8_t>(value & 0xFF),
        };

        result.insert(result.end(), decoded.begin(), decoded.end() - padding);
    }

    return result;
}
