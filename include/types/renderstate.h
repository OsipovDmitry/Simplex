#ifndef TYPESRENDERSTATE_H
#define TYPESRENDERSTATE_H

#include "../utils/enumclass.h"

namespace types
{

ENUMCLASS(DepthTestFunc, int32_t,
          LessEqual,
          GreaterEqual,
          Less,
          Greater,
          Equal,
          NotEqual,
          Newer,
          Always_)

ENUMCLASS(BlendFunc, int32_t,
          Zero,
          One,
          SourceColor,
          OneMinusSourceColor,
          DestColor,
          OneMinusDestColor,
          SourceAlpha,
          OneMinusSourceAlpha,
          DestAlpha,
          OneMinusDestAlpha,
          ConstColor,
          OneMinusConstColor,
          ConstAlpha,
          OneMinusConstAlpha,
          SourceAlphaSaturate)

ENUMCLASS(BlendEquation, int32_t,
          Add,
          Subtract,
          ReverseSubtract,
          Min,
          Max)

}

#endif // TYPESRENDERSTATE_H
