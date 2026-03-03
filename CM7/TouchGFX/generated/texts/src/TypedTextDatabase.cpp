#include <touchgfx/TypedText.hpp>

// this is a stub implementation to satisfy references in the code
// actual database is produced by TouchGFX Designer with proper IDs

using namespace touchgfx;

TypedText TypedTextDatabase::getTypedText(uint8_t id)
{
    // return an empty typed text for all ids
    return TypedText(TypedText::LAYOUT);
}

uint16_t TypedTextDatabase::getTypedTextId(const uint8_t* text)
{
    return 0;
}

uint8_t TypedTextDatabase::getLanguage()
{
    return 0;
}

void TypedTextDatabase::setLanguage(uint8_t)
{
}

// define text ids used in our views (placeholders)
enum
{
    T_RPM_VALUE = 1,
    T_TYPED_TEXT_EMPTY = 0,
};
