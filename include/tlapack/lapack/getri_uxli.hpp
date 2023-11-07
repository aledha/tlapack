/// @file getri_uxli.hpp
/// @author Ali Lotfi, University of Colorado Denver, USA
//
// Copyright (c) 2021-2023, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef TLAPACK_GETRI_UXLI_HH
#define TLAPACK_GETRI_UXLI_HH

#include "tlapack/base/utils.hpp"
#include "tlapack/blas/copy.hpp"
#include "tlapack/blas/dotu.hpp"
#include "tlapack/blas/gemv.hpp"

namespace tlapack {

/** Worspace query of getri()
 *
 * @param[in] A n-by-n matrix.
 *
 * @return WorkInfo The amount workspace required.
 *
 * @ingroup workspace_query
 */
template <class T, TLAPACK_SMATRIX matrix_t>
constexpr WorkInfo getri_uxli_worksize(const matrix_t& A)
{
    if constexpr (is_same_v<T, type_t<matrix_t>>)
        return WorkInfo(ncols(A) - 1);
    else
        return WorkInfo(0);
}

/** @copybrief getri_uxli()
 * Workspace is provided as an argument.
 * @copydetails getri_uxli()
 *
 * @param work Workspace. Use the workspace query to determine the size needed.
 *
 * @ingroup computational
 */
template <TLAPACK_SMATRIX matrix_t, TLAPACK_WORKSPACE work_t>
int getri_uxli_work(matrix_t& A, work_t& work)
{
    using idx_t = size_type<matrix_t>;
    using T = type_t<matrix_t>;
    using range = pair<idx_t, idx_t>;

    // constant n, number of rows and also columns of A
    const idx_t n = ncols(A);

    // check arguments
    tlapack_check(nrows(A) == n);

    // Vector W
    auto [W, work2] = reshape(work, n - 1);

    // A has L and U in it, we will create X such that UXL=A in place of
    for (idx_t j = n - idx_t(1); j != idx_t(-1); j--) {
        // if A(j,j) is zero, then the matrix is not invertible
        if (A(j, j) == T(0)) return j + 1;

        if (j == n - 1) {
            A(j, j) = T(1) / A(j, j);
        }
        else {
            // X22, l21, u12 are as in method C Nick Higham
            auto X22 = tlapack::slice(A, range(j + 1, n), range(j + 1, n));
            auto l21 = tlapack::slice(A, range(j + 1, n), j);
            auto u12 = tlapack::slice(A, j, range(j + 1, n));
            auto w = tlapack::slice(W, range(0, n - j - 1));

            // first step of the algorithm, w holds x12
            tlapack::gemv(TRANSPOSE, T(-1) / A(j, j), X22, u12, w);

            // second line of the algorithm, update A(j, j)
            A(j, j) = (T(1) / A(j, j)) - tlapack::dotu(l21, w);

            // u12 updated, w available for use again
            tlapack::copy(w, u12);

            // third line of the algorithm
            tlapack::gemv(NO_TRANS, T(-1), X22, l21, w);

            // update l21
            tlapack::copy(w, l21);
        }
    }

    return 0;
}  // getri_uxli

/** getri computes inverse of a general n-by-n matrix A
 *  by solving for X in the following equation
 * \[
 *   U X L = I
 * \]
 *
 * @return = 0: successful exit
 * @return = i+1: if U(i,i) is exactly zero.  The triangular
 *          matrix is singular and its inverse can not be computed.
 *
 * @param[in,out] A n-by-n matrix.
 *      On entry, the factors L and U from the factorization P A = L U.
 *          L is stored in the lower triangle of A; unit diagonal is not stored.
 *          U is stored in the upper triangle of A.
 *      On exit, inverse of A is overwritten on A.
 *
 * @ingroup alloc_workspace
 */
template <TLAPACK_SMATRIX matrix_t>
int getri_uxli(matrix_t& A)
{
    using T = type_t<matrix_t>;

    // Functor
    Create<matrix_t> new_matrix;

    // Allocates workspace
    WorkInfo workinfo = getri_uxli_worksize<T>(A);
    std::vector<T> work_;
    auto work = new_matrix(work_, workinfo.m, workinfo.n);

    return getri_uxli_work(A, work);
}  // getri_uxli

}  // namespace tlapack

#endif  // TLAPACK_GETRI_UXLI_HH
