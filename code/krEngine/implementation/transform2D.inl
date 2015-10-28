
bool kr::areIdentical(const kr::Transform2D& lhs, const kr::Transform2D& rhs)
{
  return lhs.position.IsIdentical(rhs.position)
      && lhs.rotation == rhs.rotation;
}

bool kr::areEqual(const kr::Transform2D& lhs,
                  const kr::Transform2D& rhs,
                  float epsilon)
{
  return lhs.position.IsEqual(rhs.position, epsilon)
      && lhs.rotation.IsEqualSimple(rhs.rotation, ezAngle::Degree(epsilon));
}
