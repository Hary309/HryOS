#include "fonts.hpp"

#include "fonts/psf.hpp"
#include "logger/logger.hpp"
#include "storage/fonts/zap-light16.h"

#include "assert.hpp"

void fonts::init()
{
    const bool load_status = fonts::load_psf_font(storage::fonts::zap_light16.data(), ZAP_LIGHT_16);

    HRY_ASSERT(load_status, "Cannot load default psf font!");

    logger::info("Fonts Initialized");
}
