#ifndef ARRAYMATRIX_HPP__
#define ARRAYMATRIX_HPP__

#include <iostream>
#include <cmath>
#include <array>

namespace ArrayMatrix {

// matrix type
template <typename T, std::size_t M, std::size_t N>
using Matrix = std::array<std::array<T, N>, M>;

// vector type
template <typename T, std::size_t M>
using Vector = std::array<T, M>;

// converts a vector to a vertical matrix
template <typename T, std::size_t M>
Matrix<T,M,1> to_vertical_matrix(const Vector<T,M> &vector)
{
  Matrix<T,M,1> matrix;
  for (std::size_t i = 0; i < M; ++i)
    matrix[i][0] = vector[i];
  return matrix;
}

// converts a vector to a horizontal matrix
template <typename T, std::size_t N>
Matrix<T,1,N> to_horizontal_matrix(const Vector<T,N> &vector)
{
  Matrix<T,1,N> matrix;
  for (std::size_t i = 0; i < N; ++i)
    matrix[0][i] = vector[i];
  return matrix;
}

// converts a vertical matrix to a vector.
template <typename T, std::size_t M>
Vector<T,M> to_vector(const Matrix<T,M,1> &matrix)
{
  Vector<T,M> vector;
  for (std::size_t i = 0; i < M; ++i)
    vector[i] = matrix[i][0];
  return vector;
}

// Converts a horizontal matrix to a vector.
template <typename T, std::size_t N>
Vector<T,N> to_vector(const Matrix<T,1,N> &matrix)
{
  Vector<T,N> vector;
  for (std::size_t i = 0; i < N; ++i)
    vector[i] = matrix[0][i];
  return vector;
}

// matrix * matrix
template <typename T, std::size_t M, std::size_t N, std::size_t N2>
Matrix<T,M,N2>
  operator*(const Matrix<T,M,N>  &matrix1,
            const Matrix<T,N,N2> &matrix2)
{
  Matrix<T,M,N2> res{};
  for (std::size_t i = 0; i < M; ++i)
    for (std::size_t j = 0; j < N2; ++j)
      for (std::size_t k = 0; k < N; ++k)
        res[i][j] = res[i][j] + matrix1[i][k] * matrix2[k][j];
  return res;
}

// matrix * vector
template <typename T, std::size_t M, std::size_t N>
Vector<T,M>
  operator*(const Matrix<T,M,N> &matrix,
            const Vector<T,N>   &vector)
{
  return to_vector(matrix * to_vertical_matrix(vector));
}

// vector * matrix
template <typename T, std::size_t M, std::size_t N>
Vector<T,M>
  operator*(const Vector<T,N>   &vector,
            const Matrix<T,N,M> &matrix)
{
  return to_vector(to_horizontal_matrix(vector) * matrix);
}

// matrix + matrix
template <typename T, std::size_t M, std::size_t N>
Matrix<T,M,N>
  operator+(const Matrix<T,M,N> &matrix1,
            const Matrix<T,M,N> &matrix2)
{
  Matrix<T,M,N> res;
  for (std::size_t i = 0; i < M; ++i)
    for (std::size_t j = 0; j < N; ++j)
      res[i][j] = matrix1[i][j] + matrix2[i][j];
  return res;
}

// matrix - matrix
template <typename T, std::size_t M, std::size_t N>
Matrix<T,M,N>
  operator-(const Matrix<T,M,N> &matrix1,
            const Matrix<T,M,N> &matrix2)
{
  Matrix<T,M,N> res;
  for (std::size_t i = 0; i < M; ++i)
    for (std::size_t j = 0; j < N; ++j)
      res[i][j] = matrix1[i][j] - matrix2[i][j];
  return res;
}

// vector + vector
template <typename T, std::size_t N>
Vector<T,N>
  operator+(Vector<T,N> vector1,
            const Vector<T,N> &vector2)
{
  for (std::size_t i = 0; i < N; ++i)
    vector1[i] = vector1[i] + vector2[i];
  return vector1;
}

// vector - vector
template <typename T, std::size_t N>
Vector<T,N>
  operator-(Vector<T,N> vector1,
            const Vector<T,N> &vector2)
{
  for (std::size_t i = 0; i < N; ++i)
    vector1[i] = vector1[i] - vector2[i];
  return vector1;
}

// vector * scalar
template <typename T, std::size_t N>
Vector<T,N>
  operator*(Vector<T,N> vector,
            const T &scalar)
{
  for (std::size_t i = 0; i < N; ++i)
    vector[i] = vector[i] * scalar;
  return vector;
}

// vector / scalar
template <typename T, std::size_t N>
Vector<T,N>
  operator/(Vector<T,N> vector,
            const T &scalar)
{
  for (std::size_t i = 0; i < N; ++i)
    vector[i] = vector[i] / scalar;
  return vector;
}

// matrix * scalar
template <typename T, std::size_t M, std::size_t N>
Matrix<T,M,N>
  operator*(Matrix<T,M,N> matrix,
            const T &scalar)
{
  for (std::size_t i = 0; i < M; ++i)
    for (std::size_t j = 0; j < N; ++j)
      matrix[i][j] = matrix[i][j] * scalar;
  return matrix;
}

// matrix / scalar
template <typename T, std::size_t M, std::size_t N>
Matrix<T,M,N>
  operator/(Matrix<T,M,N> matrix,
            const T &scalar)
{
  for (std::size_t i = 0; i < M; ++i)
    for (std::size_t j = 0; j < N; ++j)
      matrix[i][j] = matrix[i][j] / scalar;
  return matrix;
}

// identity matrix
template <typename T, std::size_t N>
Matrix<T,N,N> identity_matrix()
{
  Matrix<T,N,N> res{};
  for (std::size_t i = 0; i < N; ++i)
    res[i][i] = 1;
  return res;
}

// scalar product = u1*v1 + u2*v2 + ...
template <typename T, std::size_t N>
T scalar_product(
  const Vector<T,N> &A,
  const Vector<T,N> &B)
{
  static_assert(N > 0, "Scalar can be calculated only for non-zero vectors.");
  auto sp = A[0] * B[0];
  for (std::size_t i = 1; i < N; ++i)
    sp = sp + A[i] * B[i];
  return sp;
}

// pointwise product = (u1*u2, u2*v2 + ...
template <typename T, std::size_t N>
Vector<T,N> pointwise_product(
  Vector<T,N> A,
  const Vector<T,N> &B)
{
  for (std::size_t i = 1; i < N; ++i)
    A[i] = A[i] * B[i];
  return A;
}

// vector product for 3D vectors (cross product)
template <typename T>
Vector<T,3> vector_product(
  const Vector<T,3> &A,
  const Vector<T,3> &B)
{
  return {{
    A[1]*B[2] - B[1]*A[2],
    A[2]*B[0] - B[2]*A[0],
    A[0]*B[1] - B[0]*A[1]
  }};
}

// norm(V) == ||V||
template <typename T, std::size_t N>
T norm(const Vector<T,N> &vector)
{
  using std::sqrt;
  return sqrt(scalar_product(vector, vector));
}

// resize to a unit vector
template <typename T, std::size_t N>
Vector<T,N> normalized(Vector<T,N> vector)
{
  auto mag = norm(vector);
  if (mag > 0)
    for (auto &v : vector)
      v /= mag;
  return vector;
}

// transposition
template <typename T, std::size_t M, std::size_t N>
Matrix<T,N,M>
  transposed(const Matrix<T,M,N> &matrix)
{
  Matrix<T,N,M> res;
  for (std::size_t i = 0; i < N; ++i)
    for (std::size_t j = 0; j < M; ++j)
      res[i][j] = matrix[j][i];
  return res;
}

// rotation matrix - rotate about an arbitrary vector
// by an arbitrary angle
template <typename T>
Matrix<T,3,3> rotation_matrix(
  const Vector<T,3> &vector,
  const T &angle)
{
  using std::sqrt;
  using std::sin;
  using std::cos;

  auto u = vector[0];
  auto v = vector[1];
  auto w = vector[2];

  auto mag = norm(vector);
  if (mag > 0 && mag != 1.0) {
    u /= mag;
    v /= mag;
    w /= mag;
  }

  auto uu = u*u;
  auto vv = v*v;
  auto ww = w*w;
  auto uv = u*v;
  auto uw = u*w;
  auto vw = v*w;
  auto sinAng = sin(angle);
  auto cosAng = cos(angle);
  auto usinAng = u*sinAng;
  auto vsinAng = v*sinAng;
  auto wsinAng = w*sinAng;

  return {{
    {{ uu+(1-uu)*cosAng      , uv*(1-cosAng)-wsinAng , uw*(1-cosAng)+vsinAng }},
    {{ uv*(1-cosAng)+wsinAng , vv+(1-vv)*cosAng      , vw*(1-cosAng)-usinAng }},
    {{ uw*(1-cosAng)-vsinAng , vw*(1-cosAng)+usinAng , ww+(1-ww)*cosAng      }}
  }};
}

// rotation matrix that rotates a vector A to match a vector B
template <typename T>
Matrix<T,3,3> rotation_matrix(
  const Vector<T,3> &A,
  const Vector<T,3> &B)
{
  using std::asin;
  auto vector = vector_product(A, B);
  auto angle = asin(norm(vector) / (norm(A) * norm(B)));

  return rotation_matrix(vector, angle);
}

// prints vector
template <typename T, std::size_t N>
std::ostream & operator<<(std::ostream &out, const Vector<T,N> vector)
{
  for (std::size_t i = 0; i < N; ++i)
    out << vector[i] << (i == N - 1 ? "" : "\t");
  return out;
}

// prints matrix
template <typename T, std::size_t M, std::size_t N>
std::ostream & operator<<(std::ostream &out, const Matrix<T,M,N> matrix)
{
  for (std::size_t i = 0; i < M; ++i) {
    out << matrix[i];
    out << "\n";
  }
  return out;
}

} /* namespace ArrayMatrix */
#endif /* ARRAYMATRIX_HPP_ */
