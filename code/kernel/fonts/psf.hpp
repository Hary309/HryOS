#pragma once

#include <stdint.h>

#include "fonts.hpp"

namespace fonts
{
    inline const uint8_t PSF1_MAGIC0 = 0x36;
    inline const uint8_t PSF1_MAGIC1 = 0x04;

    inline const uint8_t PSF1_MODE512 = 0x01;
    inline const uint8_t PSF1_MODEHASTAB = 0x02;
    inline const uint8_t PSF1_MODEHASSEQ = 0x04;
    inline const uint8_t PSF1_MAXMODE = 0x05;

    struct psf1_header
    {
        uint8_t magic[2];
        uint8_t mode;
        uint8_t char_size;
    };

    bool load_psf_font(uint8_t* data, font& f);
} // namespace fonts
