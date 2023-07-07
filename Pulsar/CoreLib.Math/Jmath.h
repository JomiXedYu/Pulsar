#pragma once
#include <cmath>
#include <string>
#include <type_traits>
#include <cassert>
#include <cstdint>
#include <algorithm>

namespace jmath
{
    template<typename T = float> inline constexpr T pi() { return static_cast<T>(3.14159265358979323846264338327950288); }

    template<typename T = float> inline constexpr T deg2rad() { return static_cast<T>(0.017453292519943f); }
    template<typename T = float> inline constexpr T rad2deg() { return static_cast<T>(57.295779513082320876798154814105f); }

    template<typename T> constexpr T Radians(T degree)
    {
        if constexpr (std::is_arithmetic_v<T>)
            return deg2rad<T>() * degree;
        else
            return deg2rad<typename T::value_type>() * degree;
    }

    template<typename T> constexpr T Degrees(T rad) { return rad2deg<typename T::value_type>() * rad; }
    template<typename T> constexpr T Clamp(T x, T min, T max)
    {
        return std::min(std::max(x, min), max);
    }
    template<typename T>
    constexpr bool FloatEqual(T const& x, T const& y)
    {
        return std::abs(x - y) <= std::numeric_limits<T>::epsilon();
    }

    inline float Chgsign(float x, float y)
    {
        auto a = *reinterpret_cast<int*>(&x) ^ *reinterpret_cast<int*>(&y) & (int)0x80000000;
        return *reinterpret_cast<float*>(&a);
    }

    template<typename T>
    struct Vector4
    {
        using value_type = T;

        T x, y, z, w;

        Vector4() : x(0), y(0), z(0), w(0) {}
        Vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

        const T* get_value_ptr() const { return &this->x; }
        T* get_value_ptr() { return &this->x; }

        static constexpr int column_count = 1;
        static constexpr int row_count = 4;

        static inline Vector4 Mul(const Vector4<T>& l, const Vector4<T>& r)
        {
            return { l.x * r.x, l.y * r.y, l.z * r.z, l.w * r.w };
        }

        T& operator[](int index) { return *(&x + index); }
        const T& operator[](int index) const { return *(&x + index); }
    };

    template<typename T> std::string to_string(const Vector4<T>& v)
    {
        std::string s;
        s.reserve(64);
        s.append("{x: "); s.append(std::to_string(v.x)); s.append(", ");
        s.append("y: "); s.append(std::to_string(v.y)); s.append(", ");
        s.append("z: "); s.append(std::to_string(v.z)); s.append(", ");
        s.append("w: "); s.append(std::to_string(v.w)); s.append("}");
        return s;
    }

    template<typename T> Vector4<T> operator+(const Vector4<T>& a, const Vector4<T>& b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
    template<typename T> Vector4<T> operator-(const Vector4<T>& a, const Vector4<T>& b) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
    template<typename T> Vector4<T> operator*(const Vector4<T>& a, const Vector4<T>& b) { return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; }
    template<typename T> Vector4<T> operator/(const Vector4<T>& a, const Vector4<T>& b) { return { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; }
    template<typename T> Vector4<T> operator+(const Vector4<T>& a, T b) { return { a.x + b, a.y + b, a.z + b, a.w + b }; }
    template<typename T> Vector4<T> operator-(const Vector4<T>& a, T b) { return { a.x - b, a.y - b, a.z - b, a.w - b }; }
    template<typename T> Vector4<T> operator*(const Vector4<T>& a, T b) { return { a.x * b, a.y * b, a.z * b, a.w * b }; }
    template<typename T> Vector4<T> operator/(const Vector4<T>& a, T b) { return { a.x / b, a.y / b, a.z / b, a.w / b }; }
    template<typename T> Vector4<T> operator+(T a, const Vector4<T>& b) { return { a + b.x, a + b.y, a + b.z, a + b.w }; }
    template<typename T> Vector4<T> operator-(T a, const Vector4<T>& b) { return { a - b.x, a - b.y, a - b.z, a - b.w }; }
    template<typename T> Vector4<T> operator*(T a, const Vector4<T>& b) { return { a * b.x, a * b.y, a * b.z, a * b.w }; }
    template<typename T> Vector4<T> operator/(T a, const Vector4<T>& b) { return { a / b.x, a / b.y, a / b.z, a / b.w }; }
    template<typename T> bool operator==(const Vector4<T>& a, const Vector4<T>& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

    using Vector4f = Vector4<float>;
    using Vector4d = Vector4<double>;
    using Vector4i = Vector4<int>;

    template<typename T>
    T Sum(const Vector4<T>& v) { return v.x + v.y + v.z + v.w; }

    template<typename T>
    struct Vector3
    {
        using value_type = T;

        T x, y, z;

        Vector3() : x(0), y(0), z(0) {}
        Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

        const T* get_value_ptr() const { return &this->x; }
        T* get_value_ptr() { return &this->x; }

        static constexpr int column_count = 1;
        static constexpr int row_count = 3;

        T& operator[](int index) { return *(&x + index); }
        const T& operator[](int index) const { return *(&x + index); }

        static Vector3 StaticUp() { return Vector3{ T(0), T(1), T(0) }; }
        static Vector3 StaticRight() { return Vector3{ T(1), T(0), T(0) }; }
        //right handle?
        static Vector3 StaticForward() { return Vector3{ T(0), T(0), T(1) }; }
        static Vector3 StaticZero() { return Vector3{ T(0), T(0), T(0) }; }
        static Vector3 StaticOne() { return Vector3{ T(1), T(1), T(1) }; }

        Vector3& operator +=(Vector3 v) { x += v.x; y += v.y; z += v.z; return *this; }
        Vector3& operator +=(T v) { x += v; y += v; z += v; return *this; }
        Vector3& operator -=(Vector3 v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
        Vector3& operator -=(T v) { x -= v; y -= v; z -= v; return *this; }
        Vector3& operator *=(T v) { x *= v; y *= v; z *= v; return *this; }
        Vector3& operator /=(T v) { x /= v; y /= v; z /= v; return *this; }
        Vector3 operator-() { return Vector3(-x, -y, -z); }

        operator Vector4<T>() const { return Vector4<T>{ T(x), T(y), T(z), T(0) }; }

        static inline T Distance(const Vector3& l, const Vector3& r)
        {
            T x = l.x - r.x;
            T z = l.z - r.z;
            T y = l.y - r.y;
            return sqrt(x * x + y * y * z * z);
        }
        static inline T Dot(const Vector3<T>& l, const Vector3<T>& r)
        {
            return l.x * r.x + l.y * r.y + l.z * r.z;
        }
        static inline Vector3 Mul(const Vector3<T>& l, const Vector3<T>& r)
        {
            return { l.x * r.x, l.y * r.y, l.z * r.z };
        }
        static Vector3<T> Normalize(const Vector3<T>& target);
        void Normalized() { *this = Normalize(*this); }
        static inline Vector3<T> Cross(const Vector3<T>& target1, const Vector3<T>& target2)
        {
            return Vector3<T>(
                target1.y * target2.z - target1.z * target2.y,
                target1.z * target2.x - target1.x * target2.z,
                target1.x * target2.y - target1.y * target2.x
                );
        }
        T Magnitude() const
        {
            return sqrtf(Dot(*this, *this));
        }

        static Vector3 Identity() { return { T(1), T(1), T(1) }; }
    };
    template<typename T> inline Vector3<T> operator+(Vector3<T> l, T r) { return Vector3<T>(l.x + r, l.y + r, l.z + r); }
    template<typename T> inline Vector3<T> operator+(T l, Vector3<T> r) { return Vector3<T>(l + r.x, l + r.y, l + r.z); }
    template<typename T> inline Vector3<T> operator+(Vector3<T> l, Vector3<T> r) { return Vector3<T>(l.x + r.x, l.y + r.y, l.z + r.z); }
    template<typename T> inline Vector3<T> operator-(Vector3<T> l, T r) { return Vector3<T>(l.x - r, l.y - r, l.z - r); }
    template<typename T> inline Vector3<T> operator-(T l, Vector3<T> r) { return Vector3<T>(l - r.x, l - r.y, l - r.z); }
    template<typename T> inline Vector3<T> operator-(Vector3<T> l, Vector3<T> r) { return Vector3<T>(l.x - r.x, l.y - r.y, l.z - r.z); }
    template<typename T> inline Vector3<T> operator/(Vector3<T> v3, T f) { return Vector3<T>(v3.x / f, v3.y / f, v3.z / f); }
    template<typename T> inline Vector3<T> operator/(T f, Vector3<T> v3) { return Vector3<T>(f / v3.x, f / v3.y, f / v3.z); }
    template<typename T> inline Vector3<T> operator*(Vector3<T> v3, T f) { return Vector3<T>(v3.x * f, v3.y * f, v3.z * f); }
    template<typename T> inline Vector3<T> operator*(T f, Vector3<T> v3) { return Vector3<T>(v3.x * f, v3.y * f, v3.z * f); }
    template<typename T> inline Vector3<T> operator*(Vector3<T> l, Vector3<T> r) { return Vector3<T>(l.x * r.x, l.y * r.y, l.z * r.z); }
    template<typename T> inline bool operator==(Vector3<T> l, Vector3<T> r) { return l.x == r.x && l.y && r.y && l.z && r.z; }

    template<typename T>
    inline Vector3<T> Vector3<T>::Normalize(const Vector3<T>& target)
    {
        return target / sqrtf(Dot(target, target));
    }

    template<typename T> std::string to_string(const Vector3<T>& v)
    {
        std::string s;
        s.reserve(64);
        s.append("{x: "); s.append(std::to_string(v.x)); s.append(", ");
        s.append("y: "); s.append(std::to_string(v.y)); s.append(", ");
        s.append("z: "); s.append(std::to_string(v.z)); s.append("}");
        return s;
    }

    using Vector3f = Vector3<float>;
    using Vector3d = Vector3<double>;
    using Vector3i = Vector3<int>;

    template<typename T>
    T Sum(const Vector3<T>& v) { return v.x + v.y + v.z; }

    template<typename T>
    Vector3<T> Chgsign(const Vector3<T>& a, const Vector3<T>& b)
    {
        return Vector3<T>{ Chgsign(a.x, b.x), Chgsign(a.y, b.y), Chgsign(a.z, b.z) };
    }

    template<typename T>
    struct Vector2
    {
        using value_type = T;

        T x, y;

        Vector2() : x(0), y(0) {}
        Vector2(T _x, T _y) : x(_x), y(_y) {}

        template<typename U>
        Vector2(const U& r) : x((T)r.x), y((T)r.y) {}

        const T* get_value_ptr() const { return &this->x; }
        T* get_value_ptr() { return &this->x; }

        static constexpr int column_count = 1;
        static constexpr int row_count = 2;

        T& operator[](int index) { return *(&x + index); }
        const T& operator[](int index) const { return *(&x + index); }

        static Vector2 StaticZero() { return Vector2{ T(0), T(0) }; }
        static Vector2 StaticOne() { return Vector2{ T(1), T(1) }; }
        static Vector2 StaticUp() { return Vector2{ T(0), T(1) }; }
        static Vector2 StaticRight() { return Vector2{ T(1), T(0) }; }

        Vector2 operator-() { return Vector2(-x, -y); }
        Vector2 operator+=(Vector2 r) { x += r.x; y += r.y; return *this; }
        Vector2 operator+=(T r) { x += r; y += r; return *this; }
        Vector2 operator-=(Vector2 r) { x -= r.x; y -= r.y; return *this; }
        Vector2 operator-=(T r) { x -= r; y -= r; return *this; }
        Vector2 operator*=(T r) { x *= r; y *= r; return *this; }
        Vector2 operator/=(T r) { x /= r; y /= r; return *this; }

        operator Vector3<T>() const { return Vector3<T>{ x, y, T(0) }; }

        static inline T Dot(const Vector2<T>& a, const Vector2<T>& b)
        {
            return a.x * b.x + a.y * b.y;
        }
        static Vector2<T> Normalize(const Vector2<T>& input);
        static void Normalized() { *this = Normalize(*this); }
        inline Vector2 Reflect(const Vector2& input, const Vector2& normal)
        {
            return -input + 2.0f * Dot(Normalize(input), normal) * normal;
        }
    };


    template<typename T> inline Vector2<T> operator*(Vector2<T> a, T b) { return Vector2<T>(a.x * b, a.y * b); }
    template<typename T> inline Vector2<T> operator*(T a, Vector2<T> b) { return Vector2<T>(a * b.x, a * b.y); }
    template<typename T> inline Vector2<T> operator*(Vector2<T> a, Vector2<T> b) { return Vector2<T>(a.x * b.x, a.y * b.y); }
    template<typename T> inline Vector2<T> operator/(Vector2<T> a, T b) { return Vector2<T>(a.x / b, a.y / b); }
    template<typename T> inline Vector2<T> operator/(T a, Vector2<T> b) { return Vector2<T>(a / b.x, a / b.y); }
    template<typename T> inline Vector2<T> operator+(Vector2<T> a, T b) { return Vector2<T>(a.x + b, a.y + b); }
    template<typename T> inline Vector2<T> operator+(T a, Vector2<T> b) { return Vector2<T>(a + b.x, a + b.y); }
    template<typename T> inline Vector2<T> operator+(Vector2<T> a, Vector2<T> b) { return Vector2<T>(a.x + b.x, a.y + b.y); }
    template<typename T> inline Vector2<T> operator-(Vector2<T> a, T b) { return Vector2<T>(a.x - b, a.y - b); }
    template<typename T> inline Vector2<T> operator-(T a, Vector2<T> b) { return Vector2<T>(a - b.x, a - b.y); }
    template<typename T> inline Vector2<T> operator-(Vector2<T> a, Vector2<T> b) { return Vector2<T>(a.x - b.x, a.y - b.y); }
    template<typename T> inline bool operator==(Vector2<T> a, Vector2<T> b) { return a.x == b.x && a.y == b.y; }

    template<typename T>
    inline Vector2<T> Vector2<T>::Normalize(const Vector2<T>& input)
    {
        return input / sqrt(Dot(input, input));
    }

    template<typename T> std::string to_string(Vector2<T> v)
    {
        std::string s;
        s.reserve(64);
        s.append("{x: "); s.append(std::to_string(v.x)); s.append(", ");
        s.append("y: "); s.append(std::to_string(v.y)); s.append("}");
        return s;
    }

    using Vector2f = Vector2<float>;
    using Vector2d = Vector2<double>;
    using Vector2i = Vector2<int>;

    template<typename T>
    T Sum(const Vector2<T>& v) { return v.x + v.y; }

    template<typename T>
    struct Matrix2
    {
        using value_type = T;
        Vector2<T> M[2];

        Matrix2(
            T ax, T bx,
            T ay, T by) {
            M[0] = Vector2<T>(ax, ay);
            M[1] = Vector2<T>(bx, by);
        }
        Matrix2(Vector2<T> x, Vector2<T> y) { M[0] = x; M[1] = y; }

        static constexpr int column_count = 2;
        static constexpr int row_count = 2;

        const T* get_value_ptr() const { return &M[0].x; }
        T* get_value_ptr() { return &M[0].x; }

        Vector2<T>& operator[](int i) { return M[i]; }
        const Vector2<T>& operator[](int i) const { return M[i]; }

        static Matrix2 StaticScalar(T k = T(1))
        {
            return Matrix2(
                T(k), T(0),
                T(0), T(k)
            );
        }
    };
    using Matrix2f = Matrix2<float>;
    using Matrix2d = Matrix2<double>;
    using Matrix2i = Matrix2<int>;

    template<typename T>
    struct Matrix3
    {
        using value_type = T;

        Vector3<T> M[3];

        const T* get_value_ptr() const { return &M[0].x; }
        T* get_value_ptr() { return &M[0].x; }

        Matrix3(
            T ax, T bx, T cx,
            T ay, T by, T cy,
            T az, T bz, T cz
        ) {
            M[0] = Vector3<T>(ax, ay, az);
            M[1] = Vector3<T>(bx, by, bz);
            M[2] = Vector3<T>(cx, cy, cz);
        }
        Matrix3(Vector3<T> x, Vector3<T> y, Vector3<T> z) { M[0] = x; M[1] = y; M[2] = z; }

        static constexpr int column_count = 3;
        static constexpr int row_count = 3;

        Vector3<T>& operator[](int i) { return M[i]; }
        const Vector3<T>& operator[](int i) const { return M[i]; }

        static Matrix3 StaticScalar(T k = T(1))
        {
            return Matrix3(
                T(k), T(0), T(0),
                T(0), T(k), T(0),
                T(0), T(0), T(k)
            );
        }
    };
    using Matrix3f = Matrix3<float>;
    using Matrix3d = Matrix3<double>;
    using Matrix3i = Matrix3<int>;

    template<typename T>
    struct Matrix4
    {
        using value_type = T;

        Vector4<T> M[4];

        const T* get_value_ptr() const { return &M[0].x; }
        T* get_value_ptr() { return &M[0].x; }

        Matrix4() {}

        Matrix4(
            T ax, T bx, T cx, T dx,
            T ay, T by, T cy, T dy,
            T az, T bz, T cz, T dz,
            T aw, T bw, T cw, T dw
        ) {
            M[0] = Vector4<T>(ax, ay, az, aw);
            M[1] = Vector4<T>(bx, by, bz, bw);
            M[2] = Vector4<T>(cx, cy, cz, cw);
            M[3] = Vector4<T>(dx, dy, dz, dw);
        }
        Matrix4(const Vector4<T>& x, const Vector4<T>& y, const Vector4<T>& z, const Vector4<T>& w) { M[0] = x; M[1] = y; M[2] = z; M[3] = w; }

        static constexpr int column_count = 4;
        static constexpr int row_count = 4;

        Vector4<T>& operator[](int i) { return M[i]; }
        const Vector4<T>& operator[](int i) const { return M[i]; }

        Vector4<T> GetColumn(int i) const { return M[i]; }
        Vector4<T> GetRow(int i) const { return Vector4<T>{ M[0][i], M[1][i], M[2][i], M[3][i] }; }

        static Matrix4 StaticScalar(T k = T(1))
        {
            return Matrix4(
                T(k), T(0), T(0), T(0),
                T(0), T(k), T(0), T(0),
                T(0), T(0), T(k), T(0),
                T(0), T(0), T(0), T(k)
            );
        }

    };

    template<typename MAT>
    inline void Transpose(MAT* mat)
    {
        static_assert(MAT::column_count == MAT::row_count);

        for (int c = 0; c < MAT::column_count; c++)
        {
            for (int r = 0; r < MAT::column_count; r++)
            {
                if (c == r) break;
                auto a = mat->M[c][r];
                mat->M[c][r] = mat->M[r][c];
                mat->M[r][c] = a;
            }
        }
    }

    template<typename T>
    inline std::string to_string(const Matrix4<T>& v)
    {
        std::string s;
        s.reserve(64);
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 4; c++)
            {
                s.append(std::to_string(v[c][r]));
                if (c != 3)
                    s.append(", ");
            }
            s.append("\n");
        }
        return s;
    }

    template<typename T>
    Matrix4<T> operator*(const Matrix4<T>& a, const Matrix4<T>& b)
    {
        Matrix4<T> m;
        for (int l = 0; l < 4; l++)
        {
            for (int r = 0; r < 4; r++)
            {
                m[l][r] = Sum(a.GetRow(r) * b.GetColumn(l));
            }
        }
        return m;
    }

    template<typename T>
    Vector4<T> operator*(const Matrix4<T>& a, const Vector4<T>& b)
    {
        Vector4<T> v;
        for (int r = 0; r < 4; r++)
        {
            v[r] = Sum(Vector4<T>::Mul(a.GetRow(r), b));
        }
        return v;
    }


    using Matrix4f = Matrix4<float>;
    using Matrix4d = Matrix4<double>;
    using Matrix4i = Matrix4<int>;

    enum class EulerRotationOrder : uint8_t
    {
        ZYX, YZX, XZY, ZXY, YXZ, XYZ
    };
    template<typename T>
    struct Quaternion
    {
        using value_type = T;

        T x, y, z, w;

        Quaternion() : x(0), y(0), z(0), w(1) {}
        Quaternion(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
        Quaternion(const Quaternion&) = default;
        Quaternion& operator=(const Quaternion& q) = default;

        //void SetEulerZYX(Vector3<T> euler) {
        //    Vector3<T> in = Radians(euler) * T(0.5);
        //    Vector3<T> c = Vector3<T>{ std::cos(in.x), std::cos(in.y), std::cos(in.z) };
        //    Vector3<T> s = Vector3<T>{ std::sin(in.x), std::sin(in.y), std::sin(in.z) };

        //    this->w = c.x * c.y * c.z + s.x * s.y * s.z;
        //    this->x = s.x * c.y * c.z - c.x * s.y * s.z;
        //    this->y = c.x * s.y * c.z + s.x * c.y * s.z;
        //    this->z = c.x * c.y * s.z - s.x * s.y * c.z;
        //}

    public:
        static Quaternion Identity()
        {
            return Quaternion(T(0), T(0), T(0), T(1));
        }
        Quaternion& operator*=(const Quaternion& q)
        {
            Quaternion& p = *this;

            T _x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
            T _y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
            T _z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
            T _w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;

            p.x = _x; p.y = _y; p.z = _z; p.w = _w;

            return *this;
        }
        Quaternion& operator/=(const T& scalar)
        {
            x /= scalar; y /= scalar; z /= scalar; w /= scalar;
            return *this;
        }
        friend Quaternion operator/(const Quaternion& in_q, const T& scalar)
        {
            Quaternion q = in_q;
            q /= scalar;
            return q;
        }
        Quaternion operator*(const Quaternion& q) const
        {
            Quaternion nq = *this;
            nq *= q;
            return nq;
        }

        static T Dot(const Quaternion& q1, const Quaternion& q2)
        {
            return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
        }
        static T Magnitude(const Quaternion& q)
        {
            return std::sqrt(Dot(q, q));
        }
        static Quaternion FromAxisQuaternions(const Quaternion& q1, const Quaternion& q2, const Quaternion& q3)
        {
            return (q1 * q2) * q3;
        }
        static Quaternion NormalizeSafe(const Quaternion& q)
        {
            T mag = Magnitude(q);
            if (mag < 1e-6f)
                return Quaternion::Identity();
            else
                return q / mag;
        }
        Vector3<T> GetEulerRad(EulerRotationOrder order = EulerRotationOrder::ZXY) const
        {
            auto q = NormalizeSafe(*this);
            T v[7] = { T(0) };
            T d[10] = { q.x * q.x, q.x * q.y, q.x * q.z, q.x * q.w, q.y * q.y, q.y * q.z, q.y * q.w, q.z * q.z, q.z * q.w, q.w * q.w };

            bool n2 = false;
            switch (order)
            {
            case jmath::EulerRotationOrder::ZXY:
                v[6] = d[5] - d[3];
                v[4] = 2.0f * (d[1] + d[8]);
                v[5] = d[4] - d[7] - d[0] + d[9];
                v[0] = -1.0f;
                v[1] = 2.0f * v[6];

                if (std::abs(v[6]) < 0.499999f)
                {
                    v[2] = 2.0f * (d[2] + d[6]);
                    v[3] = d[7] - d[0] - d[4] + d[9];
                }
                else
                {
                    float a, b, c, e;
                    a = d[1] + d[8];
                    b = -d[5] + d[3];
                    c = d[1] - d[8];
                    e = d[5] + d[3];

                    v[2] = a * e + b * c;
                    v[3] = b * e - a * c;
                    n2 = true;
                }
                return Vector3<T>(
                    v[0] * std::asin(std::clamp(T(v[1]), T(-1), T(1))),
                    std::atan2(v[2], v[3]),
                    n2 ? 0 : std::atan2(v[4], v[5]));
                break;
            default:
                assert(order != EulerRotationOrder::ZXY);
                break;
            }
            throw 0;
        }
        Vector3<T> GetEuler(EulerRotationOrder order = EulerRotationOrder::ZXY) const
        {
            return Degrees(GetEulerRad(order));
        }
        static Quaternion FromEuler(const Vector3<T>& euler, EulerRotationOrder order = EulerRotationOrder::ZXY)
        {
            Quaternion q;
            q.SetEulerRad(euler * (jmath::pi<T>() / T(180)), order);
            //q.SetEulerRad(jmath::Radians(euler), order);
            return q;
        }

        Matrix4<T> ToMatrix() const
        {
            T x = this->x * T(2);
            T y = this->y * T(2);
            T z = this->z * T(2);
            T xx = this->x * x;
            T yy = this->y * y;
            T zz = this->z * z;
            T xy = this->x * y;
            T xz = this->x * z;
            T yz = this->y * z;
            T wx = this->w * x;
            T wy = this->w * y;
            T wz = this->w * z;

            Matrix4<T> mat;
            auto m = mat.get_value_ptr();
            m[0] = 1.0f - (yy + zz);
            m[1] = xy + wz;
            m[2] = xz - wy;
            m[3] = 0.0F;

            m[4] = xy - wz;
            m[5] = 1.0f - (xx + zz);
            m[6] = yz + wx;
            m[7] = 0.0F;

            m[8] = xz + wy;
            m[9] = yz - wx;
            m[10] = 1.0f - (xx + yy);
            m[11] = 0.0F;

            m[12] = 0.0F;
            m[13] = 0.0F;
            m[14] = 0.0F;
            m[15] = 1.0F;

            return mat;
        }
    private:
        void SetEulerRad(const Vector3<T>& euler, EulerRotationOrder order)
        {
            float cx(std::cos(euler.x / 2.0f));
            float sx(std::sin(euler.x / 2.0f));

            float cy(std::cos(euler.y / 2.0f));
            float sy(std::sin(euler.y / 2.0f));

            float cz(std::cos(euler.z / 2.0f));
            float sz(std::sin(euler.z / 2.0f));

            Quaternion qx(sx, 0.0f, 0.0f, cx);
            Quaternion qy(0.0f, sy, 0.0f, cy);
            Quaternion qz(0.0f, 0.0f, sz, cz);

            switch (order)
            {
            case EulerRotationOrder::ZYX: *this = FromAxisQuaternions(qx, qy, qz); break;
            case EulerRotationOrder::YZX: *this = FromAxisQuaternions(qx, qz, qy); break;
            case EulerRotationOrder::XZY: *this = FromAxisQuaternions(qy, qz, qx); break;
            case EulerRotationOrder::ZXY: *this = FromAxisQuaternions(qy, qx, qz); break;
            case EulerRotationOrder::YXZ: *this = FromAxisQuaternions(qz, qx, qy); break;
            case EulerRotationOrder::XYZ: *this = FromAxisQuaternions(qz, qy, qx); break;
            }

        }
        inline static Vector3<T> Internal_MakePositive(Vector3<T> euler)
        {
            float num = -0.005729578f;
            float num2 = 360.f + num;
            if (euler.x < num) { euler.x += 360.f; }
            else if (euler.x > num2) { euler.x -= 360.f; }
            if (euler.y < num) { euler.y += 360.f; }
            else if (euler.y > num2) { euler.y -= 360.f; }
            if (euler.z < num) { euler.z += 360.f; }
            else if (euler.z > num2) { euler.z -= 360.f; }
            return euler;
        }
        inline static T roll(const Quaternion& q)
        {
            return static_cast<T>(std::atan2(static_cast<T>(2) * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z));
        }
        inline static T pitch(const Quaternion& q)
        {
            T const y = static_cast<T>(2) * (q.y * q.z + q.w * q.x);
            T const x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;
            if (FloatEqual(x, T(0)) && FloatEqual(y, T(0))) //avoid atan2(0,0) - handle singularity - Matiis
                return static_cast<T>(static_cast<T>(2) * std::atan2(q.x, q.w));

            return static_cast<T>(std::atan2(y, x));

            //return (T)std::atan2(T(2) * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
        }
        inline static T yaw(const Quaternion& q)
        {
            return std::asin(Clamp(static_cast<T>(-2) * (q.x * q.z - q.w * q.y), static_cast<T>(-1), static_cast<T>(1)));
        }
    public:

    public:
        friend Quaternion operator-(const Quaternion& a, const Quaternion& b)
        {
            return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
        }
    };

    template<typename T>
    Quaternion<T> Chgsign(const Quaternion<T>& a, const Quaternion<T>& b)
    {
        return Quaternion<T>{ Chgsign(a.x, b.x), Chgsign(a.y, b.y), Chgsign(a.z, b.z), Chgsign(a.w, b.w) };
    }

    template<typename T>
    Matrix4<T> Translate(const Vector3<T>& v)
    {
        Matrix4<T> m = Matrix4<T>::StaticScalar();
        m[3][0] = v.x;
        m[3][1] = v.y;
        m[3][2] = v.z;
        return m;
    }

    template<typename T>
    Matrix4<T> Rotate(const Quaternion<T>& q)
    {
        Matrix4<T> Result = Matrix4<T>::StaticScalar();
        T qxx(q.x * q.x);
        T qyy(q.y * q.y);
        T qzz(q.z * q.z);
        T qxz(q.x * q.z);
        T qxy(q.x * q.y);
        T qyz(q.y * q.z);
        T qwx(q.w * q.x);
        T qwy(q.w * q.y);
        T qwz(q.w * q.z);

        Result[0][0] = T(1) - T(2) * (qyy + qzz);
        Result[0][1] = T(2) * (qxy + qwz);
        Result[0][2] = T(2) * (qxz - qwy);

        Result[1][0] = T(2) * (qxy - qwz);
        Result[1][1] = T(1) - T(2) * (qxx + qzz);
        Result[1][2] = T(2) * (qyz + qwx);

        Result[2][0] = T(2) * (qxz + qwy);
        Result[2][1] = T(2) * (qyz - qwx);
        Result[2][2] = T(1) - T(2) * (qxx + qyy);
        return Result;
    }
    template<typename T>
    Matrix4<T> Scale(const Vector3<T>& v)
    {
        Matrix4<T> m;
        m[0][0] = v.x;
        m[1][1] = v.y;
        m[2][2] = v.z;
        m[3][3] = T(1);
        return m;
    }

    template<typename T>
    inline std::string to_string(const Quaternion<T>& v)
    {
        std::string s;
        s.reserve(64);
        s.append("{x: "); s.append(std::to_string(v.x)); s.append(", ");
        s.append("y: "); s.append(std::to_string(v.y)); s.append(", ");
        s.append("z: "); s.append(std::to_string(v.z)); s.append(", ");
        s.append("w: "); s.append(std::to_string(v.w)); s.append("}");
        return s;
    }
    using Quat4f = Quaternion<float>;
    using Quat4d = Quaternion<double>;

    template<typename T>
    Quaternion<T> Inverse(const Quaternion<T>& q)
    {
        return Quaternion<T>(-q.x, -q.y, -q.z, q.w);
    }

    template<typename T>
    struct Transform2D
    {
        using value_type = T;

        Vector2<T> Position;
        T Rotation;
        Vector2<T> Scale;
    };

    using Transform2Df = Transform2D<float>;
    using Transform2Dd = Transform2D<double>;

    template<typename T>
    struct Transform3D
    {
        using value_type = T;

        Vector3<T> Position;
        Quaternion<T> Rotation;
        Vector3<T> Scale;
    };

    using Transform3Df = Transform3D<float>;
    using Transform3Dd = Transform3D<double>;

    template<typename T>
    struct Color4
    {
        using value_type = T;
        T r, g, b, a;

        const T* get_value_ptr() const { return &r; }

        Color4() : r(0), g(0), b(0), a(0) {}
        Color4(T _r, T _g, T _b) : r(_r), g(_g), b(_b), a(1) {}
        Color4(T _r, T _g, T _b, T _a) : r(_r), g(_g), b(_b), a(_a) {}

        Color4& operator+=(const Color4& c) { r += c.r; g += c.g; b += c.b; a += c.a;  return *this; }
        Color4& operator-=(const Color4& c) { r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this; }
        Color4& operator*=(const Color4& c) { r *= c.r; g *= c.g; b *= c.b; a *= c.a; return *this; }
        Color4& operator/=(const Color4& c) { r /= c.r; g /= c.g; b /= c.b; a /= c.a; return *this; }
        Color4& operator+=(T s) { r += s; g += s; b += s; a += s;  return *this; }
        Color4& operator-=(T s) { r -= s; g -= s; b -= s; a -= s; return *this; }
        Color4& operator*=(T s) { r *= s; g *= s; b *= s; a *= s; return *this; }
        Color4& operator/=(T s) { r /= s; g /= s; b /= s; a /= s; return *this; }
        bool operator==(const Color4& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
    };
    template<typename T> inline Color4<T> operator+(const Color4<T>& a, const Color4<T>& b) { return { a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a }; }
    template<typename T> inline Color4<T> operator-(const Color4<T>& a, const Color4<T>& b) { return { a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a }; }
    template<typename T> inline Color4<T> operator*(const Color4<T>& a, const Color4<T>& b) { return { a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a }; }
    template<typename T> inline Color4<T> operator/(const Color4<T>& a, const Color4<T>& b) { return { a.r / b.r, a.g / b.g, a.b / b.b, a.a / b.a }; }
    template<typename T> inline Color4<T> operator+(const Color4<T>& a, T b) { return { a.r + b, a.g + b, a.b + b, a.a + b }; }
    template<typename T> inline Color4<T> operator-(const Color4<T>& a, T b) { return { a.r - b, a.g - b, a.b - b, a.a - b }; }
    template<typename T> inline Color4<T> operator*(const Color4<T>& a, T b) { return { a.r * b, a.g * b, a.b * b, a.a * b }; }
    template<typename T> inline Color4<T> operator/(const Color4<T>& a, T b) { return { a.r / b, a.g / b, a.b / b, a.a / b }; }
    template<typename T> inline Color4<T> operator+(T a, const Color4<T>& b) { return { a + b.r, a + b.g, a + b.b, a + b.a }; }
    template<typename T> inline Color4<T> operator-(T a, const Color4<T>& b) { return { a - b.r, a - b.g, a - b.b, a - b.a }; }
    template<typename T> inline Color4<T> operator*(T a, const Color4<T>& b) { return { a * b.r, a * b.g, a * b.b, a * b.a }; }
    template<typename T> inline Color4<T> operator/(T a, const Color4<T>& b) { return { a / b.r, a / b.g, a / b.b, a / b.a }; }

    template<typename T>
    inline std::string to_string(const Color4<T>& c)
    {
        std::string s;
        s.reserve(64);
        s.append("{r: "); s.append(std::to_string(c.r)); s.append(", ");
        s.append("g: "); s.append(std::to_string(c.g)); s.append(", ");
        s.append("b: "); s.append(std::to_string(c.b)); s.append(", ");
        s.append("a: "); s.append(std::to_string(c.a)); s.append("}");
        return s;
    }

    using Color8b4 = Color4<uint8_t>;
    using LinearColorf = Color4<float>;


    template<typename B, typename L>
    Color4<B> LinearColorToBitColor(const Color4<L>& l)
    {
        static_assert(std::is_floating_point_v<L>, "L not integral type");
        static_assert(std::is_integral_v<B>, "B not integral type");

        constexpr int scalar = std::numeric_limits<typename Color4<L>::value_type>::max();
        return { B(l.r * scalar), B(l.g * scalar), B(l.b * scalar), B(l.a * scalar) };
    }

    template<typename L, typename B>
    Color4<L> BitColorToLinearColor(const Color4<B>& b)
    {
        static_assert(std::is_floating_point_v<L>, "L not integral type");
        static_assert(std::is_integral_v<B>, "B not integral type");

        constexpr int scalar = std::numeric_limits<typename Color4<B>::value_type>::max();
        return { L(b.r) / scalar, L(b.g) / scalar, L(b.b) / scalar, L(b.a) / scalar };
    }
}
