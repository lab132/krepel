#pragma once

namespace kr
{
  struct KR_ENGINE_API Transform2D
  {
    static Transform2D zero() { return Transform2D{ ezVec2::ZeroVector(), ezAngle::Radian(0.0f) }; }

    ezVec2 position;
    ezAngle rotation;
  };

  /// \brief Whether the two given transforms are identical (no epsilon check).
  inline bool areIdentical(const Transform2D& lhs, const Transform2D& rhs);

  /// \brief Whether the two given transforms are equal given some tolerance value \a epsilon.
  ///
  /// \note We do not treat an angle of 720 to be equal to an angle of 0.
  ///       If you want this behavior, normalize the range of the rotations first.
  inline bool areEqual(const Transform2D& lhs,
                       const Transform2D& rhs,
                       float epsilon = ezMath::BasicType<float>::DefaultEpsilon());
}

#include <krEngine/implementation/transform2D.inl>
