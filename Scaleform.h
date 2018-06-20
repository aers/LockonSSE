#pragma once

#include "../skse64/ScaleformCallbacks.h"

class GFxValue;
class GFxMovieView;

namespace Scaleform
{
	bool RegisterCallback(GFxMovieView * view, GFxValue * root);
}