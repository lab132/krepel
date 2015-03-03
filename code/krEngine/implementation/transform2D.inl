
bool kr::areIdentical(const kr::Transform2D& lhs, const kr::Transform2D& rhs)
{
  return lhs.m_position.IsIdentical(rhs.m_position)
      && lhs.m_rotation == rhs.m_rotation;
}

bool kr::areEqual(const kr::Transform2D& lhs,
                  const kr::Transform2D& rhs,
                  float epsilon)
{
  return lhs.m_position.IsEqual(rhs, epsilon)
      && lhs.m_rotation.IsEqualSimple(rhs, ezAngle::Degree(epsilon));
}
