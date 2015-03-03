#pragma once

namespace kr
{
  class KR_ENGINE_API Transform2D
  {
  public: // *** Construction
    Transform2D() = default;

    explicit Transform2D(ZeroInit) :
      m_position(ezVec2::ZeroVector()),
      m_rotation(ezAngle::Radian(0.0f))
    {
    }

  public: // *** Accessors/Mutators
    void setPosition(ezVec2 pos) { m_position = move(pos); }
    ezVec2& getPosition() { return m_position; }
    ezVec2 getPosition() const { return m_position; }

    void setRotation(ezAngle angle) { m_rotation = move(angle); }
    ezAngle& getRotation() { return m_rotation; }
    ezAngle getRotation() const { return m_rotation; }

  public: // *** Data
    ezVec2 m_position;
    ezAngle m_rotation;
  };

  /// \brief Whether the two given transforms are identical (no epsilon check).
  bool areIdentical(const Transform2D& lhs, const Transform2D& rhs);

  /// \brief Whether the two given transforms are equal given some tolerance \a epsilon.
  ///
  /// \note We do not treat an angle of 720 to be equal to an angle of 0.
  //        If you want this behavior, normalize the range of the rotations first.
  bool areEqual(const Transform2D& lhs,
                const Transform2D& rhs,
                float epsilon = ezMath::BasicType<float>::DefaultEpsilon());
}
