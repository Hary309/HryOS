#include "psf.hpp"

#include "fonts/fonts.hpp"
#include "logger/logger.hpp"

#include "binary.hpp"

bool fonts::load_psf_font(uint8_t* data, font& f)
{
    psf1_header* psf = reinterpret_cast<psf1_header*>(data);

    if (psf->magic[0] != PSF1_MAGIC0 || psf->magic[1] != PSF1_MAGIC1)
    {
        return false;
    }

    auto* begin = data + sizeof(psf1_header);

    f.glyphs = reinterpret_cast<glyph*>(begin);

    return true;
}
