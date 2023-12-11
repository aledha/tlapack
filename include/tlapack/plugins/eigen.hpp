/// @file eigen.hpp
/// @author Weslley S Pereira, University of Colorado Denver, USA
//
// Copyright (c) 2021-2023, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef TLAPACK_EIGEN_HH
#define TLAPACK_EIGEN_HH

#include <Eigen/Core>
#include <cassert>

#include "tlapack/base/arrayTraits.hpp"
#include "tlapack/plugins/stdvector.hpp"

namespace tlapack {

// -----------------------------------------------------------------------------
// Helpers

namespace traits {
    namespace internal {
        // Auxiliary constexpr routines

        template <class Derived>
        std::true_type is_eigen_dense_f(const Eigen::DenseBase<Derived>*);
        std::false_type is_eigen_dense_f(const void*);

        template <class Derived>
        std::true_type is_eigen_matrix_f(const Eigen::MatrixBase<Derived>*);
        std::false_type is_eigen_matrix_f(const void*);

        template <class XprType, int BlockRows, int BlockCols, bool InnerPanel>
        std::true_type is_eigen_block_f(
            const Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>*);
        std::false_type is_eigen_block_f(const void*);

        /// True if T is derived from Eigen::EigenDense<T>
        /// @see https://stackoverflow.com/a/25223400/5253097
        template <class T>
        constexpr bool is_eigen_dense =
            decltype(is_eigen_dense_f(std::declval<T*>()))::value;

        /// True if T is derived from Eigen::EigenMatrix<T>
        /// @see https://stackoverflow.com/a/25223400/5253097
        template <class T>
        constexpr bool is_eigen_matrix =
            decltype(is_eigen_matrix_f(std::declval<T*>()))::value;

        /// True if T is derived from Eigen::EigenBlock
        /// @see https://stackoverflow.com/a/25223400/5253097
        template <class T>
        constexpr bool is_eigen_block =
            decltype(is_eigen_block_f(std::declval<T*>()))::value;

        template <class>
        struct isStdComplex : public std::false_type {};
        template <class T>
        struct isStdComplex<std::complex<T>> : public std::true_type {};

    }  // namespace internal

    /// True if T is derived from Eigen::DenseBase
    /// @see https://stackoverflow.com/a/25223400/5253097
    template <class T>
    constexpr bool is_eigen_type = internal::is_eigen_dense<T>;

}  // namespace traits

// -----------------------------------------------------------------------------
// Data traits

namespace traits {

    /// Layout for Eigen::Dense types
    template <class matrix_t>
    struct layout_trait<
        matrix_t,
        typename std::enable_if<is_eigen_type<matrix_t> &&
                                    (matrix_t::InnerStrideAtCompileTime == 1 ||
                                     matrix_t::OuterStrideAtCompileTime == 1),
                                int>::type> {
        static constexpr Layout value =
            (matrix_t::IsVectorAtCompileTime)
                ? Layout::Strided
                : ((matrix_t::IsRowMajor) ? Layout::RowMajor
                                          : Layout::ColMajor);
    };

    template <class matrix_t>
    struct real_type_traits<
        matrix_t,
        typename std::enable_if<is_eigen_type<matrix_t>, int>::type> {
        using type = Eigen::Matrix<real_type<typename matrix_t::Scalar>,
                                   matrix_t::RowsAtCompileTime,
                                   matrix_t::ColsAtCompileTime,
                                   matrix_t::IsRowMajor ? Eigen::RowMajor
                                                        : Eigen::ColMajor,
                                   matrix_t::MaxRowsAtCompileTime,
                                   matrix_t::MaxColsAtCompileTime>;
    };

    template <class matrix_t>
    struct complex_type_traits<
        matrix_t,
        typename std::enable_if<is_eigen_type<matrix_t>, int>::type> {
        using type = Eigen::Matrix<complex_type<typename matrix_t::Scalar>,
                                   matrix_t::RowsAtCompileTime,
                                   matrix_t::ColsAtCompileTime,
                                   matrix_t::IsRowMajor ? Eigen::RowMajor
                                                        : Eigen::ColMajor,
                                   matrix_t::MaxRowsAtCompileTime,
                                   matrix_t::MaxColsAtCompileTime>;
    };

    /// Create Eigen::Matrix, @see Create
    template <typename U>
    struct CreateFunctor<U,
                         typename std::enable_if<is_eigen_type<U>, int>::type> {
        static constexpr int Rows_ =
            (U::RowsAtCompileTime == 1) ? 1 : Eigen::Dynamic;
        static constexpr int Cols_ =
            (U::ColsAtCompileTime == 1) ? 1 : Eigen::Dynamic;
        static constexpr int Options_ =
            (U::IsRowMajor) ? Eigen::RowMajor : Eigen::ColMajor;

        template <typename T>
        constexpr auto operator()(std::vector<T>& v,
                                  Eigen::Index m,
                                  Eigen::Index n = 1) const
        {
            assert(m >= 0 && n >= 0);
            v.resize(0);
            return Eigen::Matrix<T, Rows_, Cols_, Options_>(m, n);
        }
    };

    /// Create Eigen::Matrix @see CreateStatic
    template <typename U, int m, int n>
    struct CreateStaticFunctor<
        U,
        m,
        n,
        typename std::enable_if<is_eigen_type<U>, int>::type> {
        static constexpr int Options_ =
            (U::IsRowMajor) ? Eigen::RowMajor : Eigen::ColMajor;
        static_assert(m >= 0 && n >= -1);

        template <typename T>
        constexpr auto operator()(T* v) const
        {
            constexpr int Cols_ = (n == -1) ? 1 : n;
            return Eigen::Matrix<T, m, Cols_, Options_, m, Cols_>();
        }
    };
}  // namespace traits

// -----------------------------------------------------------------------------
// Data descriptors for Eigen datatypes

// Size
template <
    class T,
    int Rows,
    int Cols,
    int Options,
    int MaxRows,
    int MaxCols
#if __cplusplus >= 201703L
    // Avoids conflict with std::size
    ,
    std::enable_if_t<!(traits::internal::isStdComplex<std::decay_t<T>>::value),
                     int> = 0
#endif
    >
constexpr auto size(
    const Eigen::Matrix<T, Rows, Cols, Options, MaxRows, MaxCols>& x) noexcept
{
    return x.size();
}
template <class Derived>
constexpr auto size(const Eigen::EigenBase<Derived>& x) noexcept
{
    return x.size();
}
// Number of rows
template <class T>
constexpr auto nrows(const Eigen::EigenBase<T>& x) noexcept
{
    return x.rows();
}
// Number of columns
template <class T>
constexpr auto ncols(const Eigen::EigenBase<T>& x) noexcept
{
    return x.cols();
}

// -----------------------------------------------------------------------------
/*
 * It is important to separate block operations between:
 * 1. Data types that do not derive from Eigen::Block
 * 2. Data types that derive from Eigen::Block
 *
 * This is done to avoid the creation of classes like:
 * Block< Block< Block< MaxtrixXd > > >
 * which would increase the number of template specializations in <T>LAPACK.
 *
 * Moreover, recursive algorithms call themselves using blocks of thier
 * matrices. This creates a dead lock at compile time if the sizes are
 * given at runtime, and the compiler cannot deduce where to stop the
 * recursion for the template specialization.
 */

#define isSlice(SliceSpec) !std::is_convertible<SliceSpec, Eigen::Index>::value

// Block operations for Eigen::Dense that are not derived from Eigen::Block

template <
    class T,
    class SliceSpecRow,
    class SliceSpecCol,
    typename std::enable_if<isSlice(SliceSpecRow) && isSlice(SliceSpecCol) &&
                                traits::is_eigen_type<T> &&
                                !traits::internal::is_eigen_block<T>,
                            int>::type = 0>
constexpr auto slice(T& A, SliceSpecRow&& rows, SliceSpecCol&& cols) noexcept
{
    return A.block(rows.first, cols.first, rows.second - rows.first,
                   cols.second - cols.first);
}

template <class T,
          typename SliceSpecCol,
          typename std::enable_if<traits::is_eigen_type<T> &&
                                      !traits::internal::is_eigen_block<T>,
                                  int>::type = 0>
constexpr auto slice(T& A, Eigen::Index rowIdx, SliceSpecCol&& cols) noexcept
{
    return A.row(rowIdx).segment(cols.first, cols.second - cols.first);
}

template <class T,
          typename SliceSpecRow,
          typename std::enable_if<traits::is_eigen_type<T> &&
                                      !traits::internal::is_eigen_block<T>,
                                  int>::type = 0>
constexpr auto slice(T& A, SliceSpecRow&& rows, Eigen::Index colIdx) noexcept
{
    return A.col(colIdx).segment(rows.first, rows.second - rows.first);
}

template <class T,
          typename SliceSpec,
          typename std::enable_if<traits::is_eigen_type<T> &&
                                      !traits::internal::is_eigen_block<T>,
                                  int>::type = 0>
constexpr auto slice(T& x, SliceSpec&& range) noexcept
{
    return x.segment(range.first, range.second - range.first);
}

template <class T,
          typename SliceSpec,
          typename std::enable_if<traits::is_eigen_type<T> &&
                                      !traits::internal::is_eigen_block<T>,
                                  int>::type = 0>
constexpr auto rows(T& A, SliceSpec&& rows) noexcept
{
    return A.middleRows(rows.first, rows.second - rows.first);
}

template <class T,
          typename std::enable_if<traits::is_eigen_type<T> &&
                                      !traits::internal::is_eigen_block<T>,
                                  int>::type = 0>
constexpr auto row(T& A, Eigen::Index rowIdx) noexcept
{
    return A.row(rowIdx);
}

template <class T,
          typename SliceSpec,
          typename std::enable_if<traits::is_eigen_type<T> &&
                                      !traits::internal::is_eigen_block<T>,
                                  int>::type = 0>
constexpr auto cols(T& A, SliceSpec&& cols) noexcept
{
    return A.middleCols(cols.first, cols.second - cols.first);
}

template <class T,
          typename std::enable_if<traits::is_eigen_type<T> &&
                                      !traits::internal::is_eigen_block<T>,
                                  int>::type = 0>
constexpr auto col(T& A, Eigen::Index colIdx) noexcept
{
    return A.col(colIdx);
}

// Block operations for Eigen::Dense that are derived from Eigen::Block

template <
    class XprType,
    int BlockRows,
    int BlockCols,
    bool InnerPanel,
    class SliceSpecRow,
    class SliceSpecCol,
    typename std::enable_if<isSlice(SliceSpecRow) && isSlice(SliceSpecCol),
                            int>::type = 0>
constexpr auto slice(Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
                     SliceSpecRow&& rows,
                     SliceSpecCol&& cols) noexcept
{
    assert(rows.second <= A.rows());
    assert(cols.second <= A.cols());

    return Eigen::Block<XprType, Eigen::Dynamic, Eigen::Dynamic, InnerPanel>(
        A.nestedExpression(), A.startRow() + rows.first,
        A.startCol() + cols.first, rows.second - rows.first,
        cols.second - cols.first);
}

template <class XprType,
          int BlockRows,
          int BlockCols,
          bool InnerPanel,
          typename SliceSpecCol>
constexpr auto slice(Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
                     Eigen::Index rowIdx,
                     SliceSpecCol&& cols) noexcept
{
    assert(rowIdx < A.rows());
    assert(cols.second <= A.cols());

    return Eigen::Block<XprType, 1, Eigen::Dynamic, InnerPanel>(
        A.nestedExpression(), A.startRow() + rowIdx, A.startCol() + cols.first,
        1, cols.second - cols.first);
}

template <class XprType,
          int BlockRows,
          int BlockCols,
          bool InnerPanel,
          typename SliceSpecRow>
constexpr auto slice(Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
                     SliceSpecRow&& rows,
                     Eigen::Index colIdx) noexcept
{
    assert(rows.second <= A.rows());
    assert(colIdx < A.cols());

    return Eigen::Block<XprType, Eigen::Dynamic, 1, InnerPanel>(
        A.nestedExpression(), A.startRow() + rows.first, A.startCol() + colIdx,
        rows.second - rows.first, 1);
}

template <class XprType, int BlockRows, bool InnerPanel, typename SliceSpec>
constexpr auto slice(Eigen::Block<XprType, BlockRows, 1, InnerPanel>& x,
                     SliceSpec&& range) noexcept
{
    assert(range.second <= x.size());

    return Eigen::Block<XprType, Eigen::Dynamic, 1, InnerPanel>(
        x.nestedExpression(), x.startRow() + range.first, x.startCol(),
        range.second - range.first, 1);
}

template <class XprType, int BlockCols, bool InnerPanel, typename SliceSpec>
constexpr auto slice(Eigen::Block<XprType, 1, BlockCols, InnerPanel>& x,
                     SliceSpec&& range) noexcept
{
    assert(range.second <= x.size());

    return Eigen::Block<XprType, 1, Eigen::Dynamic, InnerPanel>(
        x.nestedExpression(), x.startRow(), x.startCol() + range.first, 1,
        range.second - range.first);
}

template <class XprType,
          int BlockRows,
          int BlockCols,
          bool InnerPanel,
          typename SliceSpec>
constexpr auto rows(Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
                    SliceSpec&& rows) noexcept
{
    assert(rows.second <= A.rows());

    return Eigen::Block<XprType, Eigen::Dynamic, BlockCols, InnerPanel>(
        A.nestedExpression(), A.startRow() + rows.first, A.startCol(),
        rows.second - rows.first, A.cols());
}

template <class XprType, int BlockRows, int BlockCols, bool InnerPanel>
constexpr auto row(Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
                   Eigen::Index rowIdx) noexcept
{
    assert(rowIdx < A.rows());

    return Eigen::Block<XprType, 1, BlockCols, InnerPanel>(
        A.nestedExpression(), A.startRow() + rowIdx, A.startCol(), 1, A.cols());
}

template <class XprType,
          int BlockRows,
          int BlockCols,
          bool InnerPanel,
          typename SliceSpec>
constexpr auto cols(Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
                    SliceSpec&& cols) noexcept
{
    assert(cols.second <= A.cols());

    return Eigen::Block<XprType, BlockRows, Eigen::Dynamic, InnerPanel>(
        A.nestedExpression(), A.startRow(), A.startCol() + cols.first, A.rows(),
        cols.second - cols.first);
}

template <class XprType, int BlockRows, int BlockCols, bool InnerPanel>
constexpr auto col(Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
                   Eigen::Index colIdx) noexcept
{
    assert(colIdx < A.cols());

    return Eigen::Block<XprType, BlockRows, 1, InnerPanel>(
        A.nestedExpression(), A.startRow(), A.startCol() + colIdx, A.rows(), 1);
}

template <
    class XprType,
    int BlockRows,
    int BlockCols,
    bool InnerPanel,
    class SliceSpecRow,
    class SliceSpecCol,
    typename std::enable_if<isSlice(SliceSpecRow) && isSlice(SliceSpecCol),
                            int>::type = 0>
constexpr auto slice(
    const Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
    SliceSpecRow&& rows,
    SliceSpecCol&& cols) noexcept
{
    assert(rows.second <= A.rows());
    assert(cols.second <= A.cols());

    return Eigen::Block<const XprType, Eigen::Dynamic, Eigen::Dynamic,
                        InnerPanel>(
        A.nestedExpression(), A.startRow() + rows.first,
        A.startCol() + cols.first, rows.second - rows.first,
        cols.second - cols.first);
}

template <class XprType,
          int BlockRows,
          int BlockCols,
          bool InnerPanel,
          typename SliceSpecCol>
constexpr auto slice(
    const Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
    Eigen::Index rowIdx,
    SliceSpecCol&& cols) noexcept
{
    assert(rowIdx < A.rows());
    assert(cols.second <= A.cols());

    return Eigen::Block<const XprType, 1, Eigen::Dynamic, InnerPanel>(
        A.nestedExpression(), A.startRow() + rowIdx, A.startCol() + cols.first,
        1, cols.second - cols.first);
}

template <class XprType,
          int BlockRows,
          int BlockCols,
          bool InnerPanel,
          typename SliceSpecRow>
constexpr auto slice(
    const Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
    SliceSpecRow&& rows,
    Eigen::Index colIdx) noexcept
{
    assert(rows.second <= A.rows());
    assert(colIdx < A.cols());

    return Eigen::Block<const XprType, Eigen::Dynamic, 1, InnerPanel>(
        A.nestedExpression(), A.startRow() + rows.first, A.startCol() + colIdx,
        rows.second - rows.first, 1);
}

template <class XprType, int BlockRows, bool InnerPanel, typename SliceSpec>
constexpr auto slice(const Eigen::Block<XprType, BlockRows, 1, InnerPanel>& x,
                     SliceSpec&& range) noexcept
{
    assert(range.second <= x.size());

    return Eigen::Block<const XprType, Eigen::Dynamic, 1, InnerPanel>(
        x.nestedExpression(), x.startRow() + range.first, x.startCol(),
        range.second - range.first, 1);
}

template <class XprType, int BlockCols, bool InnerPanel, typename SliceSpec>
constexpr auto slice(const Eigen::Block<XprType, 1, BlockCols, InnerPanel>& x,
                     SliceSpec&& range) noexcept
{
    assert(range.second <= x.size());

    return Eigen::Block<const XprType, 1, Eigen::Dynamic, InnerPanel>(
        x.nestedExpression(), x.startRow(), x.startCol() + range.first, 1,
        range.second - range.first);
}

template <class XprType,
          int BlockRows,
          int BlockCols,
          bool InnerPanel,
          typename SliceSpec>
constexpr auto rows(
    const Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
    SliceSpec&& rows) noexcept
{
    assert(rows.second <= A.rows());

    return Eigen::Block<const XprType, Eigen::Dynamic, BlockCols, InnerPanel>(
        A.nestedExpression(), A.startRow() + rows.first, A.startCol(),
        rows.second - rows.first, A.cols());
}

template <class XprType, int BlockRows, int BlockCols, bool InnerPanel>
constexpr auto row(
    const Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
    Eigen::Index rowIdx) noexcept
{
    assert(rowIdx < A.rows());

    return Eigen::Block<const XprType, 1, BlockCols, InnerPanel>(
        A.nestedExpression(), A.startRow() + rowIdx, A.startCol(), 1, A.cols());
}

template <class XprType,
          int BlockRows,
          int BlockCols,
          bool InnerPanel,
          typename SliceSpec>
constexpr auto cols(
    const Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
    SliceSpec&& cols) noexcept
{
    assert(cols.second <= A.cols());

    return Eigen::Block<const XprType, BlockRows, Eigen::Dynamic, InnerPanel>(
        A.nestedExpression(), A.startRow(), A.startCol() + cols.first, A.rows(),
        cols.second - cols.first);
}

template <class XprType, int BlockRows, int BlockCols, bool InnerPanel>
constexpr auto col(
    const Eigen::Block<XprType, BlockRows, BlockCols, InnerPanel>& A,
    Eigen::Index colIdx) noexcept
{
    assert(colIdx < A.cols());

    return Eigen::Block<const XprType, BlockRows, 1, InnerPanel>(
        A.nestedExpression(), A.startRow(), A.startCol() + colIdx, A.rows(), 1);
}

#undef isSlice

/// Get the Diagonal of an Eigen Matrix
template <class T,
          typename std::enable_if<traits::internal::is_eigen_matrix<T>,
                                  int>::type = 0>
constexpr auto diag(T& A, int diagIdx = 0) noexcept
{
    return A.diagonal(diagIdx);
}

// Transpose view
template <class matrix_t,
          typename std::enable_if<(traits::is_eigen_type<matrix_t> &&
                                   !matrix_t::IsVectorAtCompileTime),
                                  int>::type = 0>
constexpr auto transpose_view(matrix_t& A) noexcept
{
    using T = typename matrix_t::Scalar;
    using Stride = Eigen::OuterStride<>;
    assert(A.innerStride() == 1);

    constexpr int Rows_ = matrix_t::ColsAtCompileTime;
    constexpr int Cols_ = matrix_t::RowsAtCompileTime;

    using transpose_t = Eigen::Matrix<
        T, Rows_, Cols_,
        (matrix_t::IsRowMajor) ? Eigen::ColMajor : Eigen::RowMajor,
        matrix_t::MaxColsAtCompileTime, matrix_t::MaxRowsAtCompileTime>;

    using map_t = Eigen::Map<transpose_t, Eigen::Unaligned, Stride>;

    return map_t((T*)A.data(), A.cols(), A.rows(), A.outerStride());
}

// Reshape view into matrices
template <class matrix_t,
          typename std::enable_if<(traits::is_eigen_type<matrix_t> &&
                                   !matrix_t::IsVectorAtCompileTime),
                                  int>::type = 0>
auto reshape(matrix_t& A, Eigen::Index m, Eigen::Index n)
{
    using idx_t = Eigen::Index;
    using T = typename matrix_t::Scalar;
    using newm_t =
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,
                      matrix_t::IsRowMajor ? Eigen::RowMajor : Eigen::ColMajor>;
    using map_t = Eigen::Map<newm_t, Eigen::Unaligned, Eigen::OuterStride<>>;

    // constants
    const idx_t size = A.size();
    const idx_t new_size = m * n;
    const idx_t stride = A.outerStride();
    const bool is_contiguous =
        (size <= 1) ||
        (matrix_t::IsRowMajor ? (stride == A.cols() || A.rows() <= 1)
                              : (stride == A.rows() || A.cols() <= 1));

    // Check arguments
    if (new_size > size)
        throw std::domain_error("New size is larger than current size");
    if (A.innerStride() != 1)
        throw std::domain_error(
            "Reshaping is not available for matrices with inner stride "
            "different than 1.");

    if (is_contiguous) {
        const idx_t s = size - new_size;
        if constexpr (matrix_t::IsRowMajor)
            return std::make_pair(map_t((T*)A.data(), m, n, n),
                                  map_t((T*)A.data() + new_size, 1, s, s));
        else
            return std::make_pair(map_t((T*)A.data(), m, n, m),
                                  map_t((T*)A.data() + new_size, s, 1, s));
    }
    else {
        if (m == A.rows() || n == 0) {
            return std::make_pair(
                map_t((T*)A.data(), m, n, stride),
                map_t((T*)A.data() + (matrix_t::IsRowMajor ? n : n * stride),
                      A.rows(), A.cols() - n, stride));
        }
        else if (n == A.cols() || m == 0) {
            return std::make_pair(
                map_t((T*)A.data(), m, n, stride),
                map_t((T*)A.data() + (matrix_t::IsRowMajor ? m * stride : m),
                      A.rows() - m, A.cols(), stride));
        }
        else {
            throw std::domain_error(
                "Cannot reshape to non-contiguous matrix if both the number of "
                "rows and columns are different.");
        }
    }
}
template <class vector_t,
          typename std::enable_if<(traits::is_eigen_type<vector_t> &&
                                   vector_t::IsVectorAtCompileTime),
                                  int>::type = 0>
auto reshape(vector_t& v, Eigen::Index m, Eigen::Index n)
{
    using idx_t = Eigen::Index;
    constexpr idx_t Rows_ = vector_t::IsRowMajor ? 1 : Eigen::Dynamic;
    constexpr idx_t Cols_ = vector_t::IsRowMajor ? Eigen::Dynamic : 1;

    using T = typename vector_t::Scalar;
    using newm_t = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,
                                 (vector_t::IsRowMajor) ? Eigen::RowMajor
                                                        : Eigen::ColMajor>;
    using newv_t = Eigen::Matrix<T, Rows_, Cols_,
                                 (vector_t::IsRowMajor) ? Eigen::RowMajor
                                                        : Eigen::ColMajor>;
    using map0_t = Eigen::Map<newm_t, Eigen::Unaligned, Eigen::OuterStride<>>;
    using map1_t = Eigen::Map<newv_t, Eigen::Unaligned, Eigen::InnerStride<>>;

    // constants
    const idx_t size = v.size();
    const idx_t new_size = m * n;
    const idx_t s = size - new_size;
    const idx_t stride = v.innerStride();
    const bool is_contiguous = (size <= 1 || stride == 1);

    // Check arguments
    if (new_size > size)
        throw std::domain_error("New size is larger than current size");
    if (!is_contiguous && (newm_t::IsRowMajor ? (n > 1) : (m > 1)))
        throw std::domain_error(
            "New sizes are not compatible with the current vector.");

    if (is_contiguous) {
        return std::make_pair(
            map0_t((T*)v.data(), m, n, (newm_t::IsRowMajor) ? n : m),
            map1_t((T*)v.data() + new_size, s, Eigen::InnerStride<>(1)));
    }
    else {
        return std::make_pair(map0_t((T*)v.data(), m, n, stride),
                              map1_t((T*)v.data() + new_size * stride, s,
                                     Eigen::InnerStride<>(stride)));
    }
}

// Reshape view into vectors
template <class matrix_t,
          typename std::enable_if<(traits::is_eigen_type<matrix_t> &&
                                   !matrix_t::IsVectorAtCompileTime),
                                  int>::type = 0>
auto reshape(matrix_t& A, Eigen::Index n)
{
    using idx_t = Eigen::Index;
    constexpr idx_t Rows_ = matrix_t::IsRowMajor ? 1 : Eigen::Dynamic;
    constexpr idx_t Cols_ = matrix_t::IsRowMajor ? Eigen::Dynamic : 1;

    using T = typename matrix_t::Scalar;
    using newm_t =
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,
                      matrix_t::IsRowMajor ? Eigen::RowMajor : Eigen::ColMajor>;
    using newv_t =
        Eigen::Matrix<T, Rows_, Cols_,
                      matrix_t::IsRowMajor ? Eigen::RowMajor : Eigen::ColMajor>;
    using map0_t = Eigen::Map<newv_t, Eigen::Unaligned, Eigen::InnerStride<>>;
    using map1_t = Eigen::Map<newm_t, Eigen::Unaligned, Eigen::OuterStride<>>;

    // constants
    const idx_t size = A.size();
    const idx_t stride = A.outerStride();
    const bool is_contiguous =
        (size <= 1) ||
        (matrix_t::IsRowMajor ? (stride == A.cols() || A.rows() <= 1)
                              : (stride == A.rows() || A.cols() <= 1));

    // Check arguments
    if (n > size)
        throw std::domain_error("New size is larger than current size");
    if (A.innerStride() != 1)
        throw std::domain_error(
            "Reshaping is not available for matrices with inner stride "
            "different than 1.");

    if (is_contiguous) {
        const idx_t s = size - n;
        return std::make_pair(map0_t((T*)A.data(), n, Eigen::InnerStride<>(1)),
                              (matrix_t::IsRowMajor)
                                  ? map1_t((T*)A.data() + n, 1, s, s)
                                  : map1_t((T*)A.data() + n, s, 1, s));
    }
    else {
        if (n == 0) {
            return std::make_pair(
                map0_t((T*)A.data(), 0, Eigen::InnerStride<>(1)),
                map1_t((T*)A.data(), A.rows(), A.cols(), stride));
        }
        else if (n == A.rows()) {
            if constexpr (matrix_t::IsRowMajor)
                return std::make_pair(
                    map0_t((T*)A.data(), n, Eigen::InnerStride<>(stride)),
                    map1_t((T*)A.data() + 1, A.rows(), A.cols() - 1, stride));
            else
                return std::make_pair(
                    map0_t((T*)A.data(), n, Eigen::InnerStride<>(1)),
                    map1_t((T*)A.data() + stride, A.rows(), A.cols() - 1,
                           stride));
        }
        else if (n == A.cols()) {
            if constexpr (matrix_t::IsRowMajor)
                return std::make_pair(
                    map0_t((T*)A.data(), n, Eigen::InnerStride<>(1)),
                    map1_t((T*)A.data() + stride, A.rows() - 1, A.cols(),
                           stride));
            else
                return std::make_pair(
                    map0_t((T*)A.data(), n, Eigen::InnerStride<>(stride)),
                    map1_t((T*)A.data() + 1, A.rows() - 1, A.cols(), stride));
        }
        else {
            throw std::domain_error(
                "Cannot reshape to non-contiguous matrix into a vector if the "
                "new size is different from the number of rows and columns.");
        }
    }
}
template <class vector_t,
          typename std::enable_if<(traits::is_eigen_type<vector_t> &&
                                   vector_t::IsVectorAtCompileTime),
                                  int>::type = 0>
auto reshape(vector_t& v, Eigen::Index n)
{
    using idx_t = Eigen::Index;
    constexpr idx_t Rows_ = vector_t::IsRowMajor ? 1 : Eigen::Dynamic;
    constexpr idx_t Cols_ = vector_t::IsRowMajor ? Eigen::Dynamic : 1;

    using T = typename vector_t::Scalar;
    using newv_t =
        Eigen::Matrix<T, Rows_, Cols_,
                      vector_t::IsRowMajor ? Eigen::RowMajor : Eigen::ColMajor>;
    using map_t = Eigen::Map<newv_t, Eigen::Unaligned, Eigen::InnerStride<>>;

    // constants
    const idx_t stride = v.innerStride();

    // Check arguments
    if (n > v.size())
        throw std::domain_error("New size is larger than current size");

    return std::make_pair(map_t((T*)v.data(), n, Eigen::InnerStride<>(stride)),
                          map_t((T*)v.data() + n * stride, v.size() - n,
                                Eigen::InnerStride<>(stride)));
}

// -----------------------------------------------------------------------------
// Deduce matrix and vector type from two provided ones

namespace traits {

    template <typename T>
    constexpr bool cast_to_eigen_type = is_eigen_type<T> || is_stdvector_type<T>
#ifdef TLAPACK_LEGACYARRAY_HH
                                        || is_legacy_type<T>
#endif
#ifdef TLAPACK_MDSPAN_HH
                                        || is_mdspan_type<T>
#endif
        ;

    // for two types
    // should be especialized for every new matrix class
    template <typename matrixA_t, typename matrixB_t>
    struct matrix_type_traits<
        matrixA_t,
        matrixB_t,
        typename std::enable_if<
            ((is_eigen_type<matrixA_t> ||
              is_eigen_type<matrixB_t>)&&cast_to_eigen_type<matrixA_t> &&
             cast_to_eigen_type<matrixB_t>),
            int>::type> {
        using T = scalar_type<type_t<matrixA_t>, type_t<matrixB_t>>;

        static constexpr Layout LA = layout<matrixA_t>;
        static constexpr Layout LB = layout<matrixB_t>;
        static constexpr int L =
            ((LA == Layout::RowMajor) && (LB == Layout::RowMajor))
                ? Eigen::RowMajor
                : Eigen::ColMajor;

        using type = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, L>;
    };

    // for two types
    // should be especialized for every new vector class
    template <typename vecA_t, typename vecB_t>
    struct vector_type_traits<
        vecA_t,
        vecB_t,
        typename std::enable_if<
            ((is_eigen_type<vecA_t> ||
              is_eigen_type<vecB_t>)&&cast_to_eigen_type<vecA_t> &&
             cast_to_eigen_type<vecB_t>),
            int>::type> {
        using T = scalar_type<type_t<vecA_t>, type_t<vecB_t>>;
        using type = Eigen::Matrix<T, Eigen::Dynamic, 1>;
    };

#if !defined(TLAPACK_MDSPAN_HH) && !defined(TLAPACK_LEGACYARRAY_HH)
    template <class vecA_t, class vecB_t>
    struct matrix_type_traits<
        vecA_t,
        vecB_t,
        std::enable_if_t<traits::is_stdvector_type<vecA_t> &&
                             traits::is_stdvector_type<vecB_t>,
                         int>> {
        using T = scalar_type<type_t<vecA_t>, type_t<vecB_t>>;
        using type = Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor>;
    };

    template <class vecA_t, class vecB_t>
    struct vector_type_traits<
        vecA_t,
        vecB_t,
        std::enable_if_t<traits::is_stdvector_type<vecA_t> &&
                             traits::is_stdvector_type<vecB_t>,
                         int>> {
        using T = scalar_type<type_t<vecA_t>, type_t<vecB_t>>;
        using type = Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor>;
    };
#endif

}  // namespace traits

// -----------------------------------------------------------------------------
// Cast to Legacy arrays

template <class T, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
constexpr auto legacy_matrix(
    const Eigen::Matrix<T, Rows, Cols, Options, MaxRows, MaxCols>& A) noexcept
{
    using matrix_t = Eigen::Matrix<T, Rows, Cols, Options, MaxRows, MaxCols>;
    using idx_t = Eigen::Index;

    if constexpr (matrix_t::IsVectorAtCompileTime)
        return legacy::Matrix<T, idx_t>{Layout::ColMajor, 1, A.size(),
                                        (T*)A.data(), A.innerStride()};
    else {
        constexpr Layout L = layout<matrix_t>;
        return legacy::Matrix<T, idx_t>{L, A.rows(), A.cols(), (T*)A.data(),
                                        A.outerStride()};
    }
}

template <class Derived>
constexpr auto legacy_matrix(
    const Eigen::MapBase<Derived, Eigen::ReadOnlyAccessors>& A) noexcept
{
    using T = typename Derived::Scalar;
    using idx_t = Eigen::Index;

    if constexpr (Derived::IsVectorAtCompileTime) {
        assert(A.outerStride() == 1 ||
               (A.innerStride() == 1 &&
                traits::internal::is_eigen_block<Derived>));
        return legacy::Matrix<T, idx_t>{Layout::ColMajor, 1, A.size(),
                                        (T*)A.data(), A.innerStride()};
    }
    else {
        assert(A.innerStride() == 1);
        constexpr Layout L = layout<Derived>;
        return legacy::Matrix<T, idx_t>{L, A.rows(), A.cols(), (T*)A.data(),
                                        A.outerStride()};
    }
}

template <class T, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
constexpr auto legacy_vector(
    const Eigen::Matrix<T, Rows, Cols, Options, MaxRows, MaxCols>& A) noexcept
{
    using matrix_t = Eigen::Matrix<T, Rows, Cols, Options, MaxRows, MaxCols>;
    using idx_t = Eigen::Index;

    if constexpr (matrix_t::IsVectorAtCompileTime)
        return legacy::Vector<T, idx_t>{A.size(), (T*)A.data(),
                                        A.innerStride()};
    else {
        assert(A.rows() == 1 || A.cols() == 1);
        return legacy::Vector<T, idx_t>{A.size(), (T*)A.data(),
                                        A.outerStride()};
    }
}

template <class Derived>
constexpr auto legacy_vector(
    const Eigen::MapBase<Derived, Eigen::ReadOnlyAccessors>& A) noexcept
{
    using T = typename Derived::Scalar;
    using idx_t = Eigen::Index;

    if constexpr (Derived::IsVectorAtCompileTime) {
        assert(A.outerStride() == 1 ||
               (A.innerStride() == 1 &&
                traits::internal::is_eigen_block<Derived>));
        return legacy::Vector<T, idx_t>{A.size(), (T*)A.data(),
                                        A.innerStride()};
    }
    else {
        assert(A.innerStride() == 1);
        assert(A.rows() == 1 || A.cols() == 1);
        return legacy::Vector<T, idx_t>{A.size(), (T*)A.data(),
                                        A.outerStride()};
    }
}

}  // namespace tlapack

#endif  // TLAPACK_EIGEN_HH
