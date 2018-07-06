#include "textstyle.h"

bool TextStyle::operator==(const TextStyle &other) const
{
	return
			font == other.font &&
			color == other.color &&

			outlineEnabled == other.outlineEnabled &&
			outlineColor == other.outlineColor &&
			outlineWidth == other.outlineWidth &&

			backgroundEnabled == other.backgroundEnabled &&
			backgroundColor == other.backgroundColor &&
			backgroundPadding == other.backgroundPadding
			;
}
