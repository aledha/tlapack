/// @file laset.hpp
/// @author Weslley S Pereira, University of Colorado Denver, USA
/// Adapted from @see https://github.com/langou/latl/blob/master/include/laset.h
//
// Copyright (c) 2012-2021, University of Colorado Denver. All rights reserved.
//
// This file is part of <T>LAPACK.
// <T>LAPACK is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#ifndef __SLATE_LASET_HH__
#define __SLATE_LASET_HH__

#include "lapack/laset.hpp"

namespace lapack {

/** Initializes a matrix to diagonal and off-diagonal values
 * 
 * @param[in] matrixtype :
 *
 *        'U': A is assumed to be upper triangular; elements below the diagonal are not referenced.
 *        'L': A is assumed to be lower triangular; elements above the diagonal are not referenced.
 *        otherwise, A is assumed to be a full matrix.
 *
 * @see laset( Uplo, blas::idx_t, blas::idx_t, TA, TA, TA*, blas::idx_t )
 * 
 * @ingroup auxiliary
 */
template< typename TA >
void inline laset(
    MatrixType matrixtype, blas::idx_t m, blas::idx_t n,
    TA alpha, TA beta,
    TA* A, blas::idx_t lda )
{
    if (matrixtype == MatrixType::Upper) {
        laset(Uplo::Upper, m, n, alpha, beta, A, lda);
    } else if (matrixtype == MatrixType::Lower) {
        laset(Uplo::Lower, m, n, alpha, beta, A, lda);
    } else {
        laset(Uplo::General, m, n, alpha, beta, A, lda);
    }
}

}

#endif // __SLATE_LASET_HH__
