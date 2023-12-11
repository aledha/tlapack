# \<T\>LAPACK

C++ Template Linear Algebra PACKage

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://github.com/tlapack/tlapack/blob/master/LICENSE)
[![Continuous Testing](https://github.com/tlapack/tlapack/actions/workflows/cmake.yml/badge.svg?branch=master)](https://github.com/tlapack/tlapack/actions/workflows/cmake.yml)
[![Doxygen](https://github.com/tlapack/tlapack/actions/workflows/doxygen.yml/badge.svg?branch=master)](https://github.com/tlapack/tlapack/actions/workflows/doxygen.yml)
[![ClangFormat](https://github.com/tlapack/tlapack/actions/workflows/clang-format-check.yml/badge.svg)](https://github.com/tlapack/tlapack/actions/workflows/clang-format-check.yml)
[![CodeQL](https://github.com/tlapack/tlapack/actions/workflows/codeql.yml/badge.svg?branch=master)](https://github.com/tlapack/tlapack/actions/workflows/codeql.yml)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/tlapack/tlapack/badge)](https://securityscorecards.dev/viewer/?uri=github.com/tlapack/tlapack)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/7835/badge)](https://www.bestpractices.dev/projects/7835)

## About

First things to know about \<T\>LAPACK:

1. We write &nbsp; \<T\>LAPACK &nbsp; whenever it is possible. This includes all software documentation, discussions, and presentation.
2. We say it &nbsp; T-L-A-PACK &nbsp; .
3. We use &nbsp; `tlapack` &nbsp; for files, folders, and links, to make it more portable and easier to use.

\<T\>LAPACK provides:

- Precision-neutral function template implementation

_Supported in part by [NSF ACI 2004850](http://www.nsf.gov/awardsearch/showAward?AWD_ID=2004850)._

## Current functionality

\<T\>LAPACK is a work in progress (WIP) project. This is a list of the current functionality:

- [x] BLAS
  - [x] Level 1, except SDSDOT.
  - [x] Level 2, except the routines for banded and packed formats (xGBMV, xHBMV, xHPMV, xSBMV, xSPMV, xTBMV, xTPMV, xTBSV, xTPSV, xHPR, xHPR2, xSPR, xSPR2).
  - [x] Level 3.
- [ ] Linear solvers
  - [ ] Safe scaling triangular solve
  - [ ] Safe scaling solver for Sylvester equations
- [ ] Matrix factorizations
  - [x] Cholesky
    - Recursive
    - Blocked
  - [ ] Fully pivoted QR
  - [ ] Fully pivoted RQ
  - [ ] Fully pivoted LQ
  - [ ] Fully pivoted QL
  - [x] Hessenberg reduction
    - Level-2
    - Blocked
  - [x] Householder QR
    - Level-2
    - Blocked
  - [x] Householder RQ
    - Level-2
    - Blocked
  - [x] Householder LQ
    - Level-2
    - Blocked
  - [x] Householder QL
    - Level-2
    - Blocked
  - [x] Householder bidiagonalization
    - Level-2
    - Blocked
  - [x] LU with partial pivoting
    - Level-0
    - Recursive
- [ ] Matrix inversion
  - [x] General matrix
    - Method C from §14.3.3 in <a href="#1">[1]</a>
    - Method D from §14.3.4 in <a href="#1">[1]</a>
  - [ ] Hermitian positive definite
  - [x] Triangular matrix
    - Recursive
- [x] Norms of general, hermitian, symmetric and triangular matrices
  - [x] 1-norm
  - [x] Infinity-norm
  - [x] Frobenius-norm
  - [x] Max-norm
- [ ] Nonsymmetric Eigenvalue Problem
  - [x] Schur decomposition
    - Double-shift implicit QR
    - Multishift implicit QR with Aggressive Early Deflation (AED)
  - [ ] Eigenvector computation
  - [ ] Swap eigenvalues in Schur form
  - [x] Generalized Schur problem
  - [x] Generalized eigenvalue problem
- [ ] Symmetric Eigenvalue Problem
  - [ ] Tridiagonal reduction
  - [ ] Eigenvector computation
- [ ] Singular Value Decomposition (SVD)
  - [x] Standard singular value problem
  - [ ] Generalized singular value problem
- [ ] Linear least squares
  - [ ] Using QR factorization
  - [ ] Using SVD
- [x] Additional kernels
  - [x] Order 1 and 2 Sylvester equation solver
  - [x] In-place upper times lower triangular matrix multiplication for general and hermitian matrices
  - [x] In-place lower times upper triangular matrix multiplication
  - [x] In-place transpose of a matrix

The complete documentation and implementation for the routines that are available are listed in the [API documentation](https://tlapack.github.io/tlapack/).

## Installation

\<T\>LAPACK is built and installed with [CMake](https://cmake.org/).

### Getting CMake

You can either download binaries for the [latest stable](https://cmake.org/download/#latest) or [previous](https://cmake.org/download/#previous) release of CMake,
or build the [current development distribution](https://github.com/Kitware/CMake) from source. CMake is also available in the APT repository on Ubuntu 16.04 or higher.

### Building and Installing \<T\>LAPACK

\<T\>LAPACK can be build following the standard CMake recipe

```sh
cmake -B build .      # configuration step
cmake --build build   # build step
```

To install \<T\>LAPACK in your system, use

```sh
cmake -B build -D CMAKE_INSTALL_PREFIX=/path/to/install . # configuration step
cmake --build build --target install                      # install step
```

### CMake options

Standard environment variables affect CMake.
Some examples are

    CXX                 C++ compiler
    CXXFLAGS            C++ compiler flags
    LDFLAGS             linker flags

The Fortran and C wrappers to \<T\>LAPACK also use, among others,

    CC                  C compiler
    CFLAGS              C compiler flags
    FC                  Fortran compiler
    FFLAGS              Fortran compiler flags

- [This page](https://cmake.org/cmake/help/latest/manual/cmake-env-variables.7.html) lists the environment variables that have special meaning to CMake.

It is also possible to pass variables to CMake during the configuration step using the `-D` flag.
The following example builds \<T\>LAPACK in debug mode inside the directory `build`

```sh
mkdir build
cmake -B build -DCMAKE_BUILD_TYPE=Debug .
cmake --build build
```

- [This page](https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html) documents variables that are provided by CMake or have meaning to CMake when set by project code.

### \<T\>LAPACK options

Here are the \<T\>LAPACK specific options and their default values

    # Option                            # Default

    BUILD_BLASPP_TESTS                  OFF

        Use BLAS++ tests to test <T>LAPACK templates.
        REQUIRES: BUILD_TESTING=ON

    BUILD_EXAMPLES                      ON

        Build examples

    BUILD_LAPACKPP_TESTS                OFF

        Use LAPACK++ tests to test <T>LAPACK templates.
        REQUIRES: BUILD_TESTING=ON

    BUILD_STANDALONE_TESTS              OFF

        Build tests as standalone executables in addition to tests for CTest.
        The executables are installed in the test directory.
        The executables do not use Catch2.
        REQUIRES: BUILD_TESTING=ON

    BUILD_testBLAS_TESTS                ON

        Build testBLAS tests.
        REQUIRES: BUILD_TESTING=ON

    BUILD_TESTING                       ON

        Build the testing tree

    BUILD_C_WRAPPERS                          OFF

        Build and install C wrappers (Work In Progress)

    BUILD_CBLAS_WRAPPERS                      OFF

        Build and install CBLAS wrappers (Work In Progress)

    BUILD_Fortran_WRAPPERS                    OFF

        Build and install Fortran wrappers (Work In Progress)

    TLAPACK_CHECK_INPUT                 ON

        Enable checks on input arguments.
        REQUIRES: TLAPACK_NDEBUG=OFF

    TLAPACK_DEFAULT_INFCHECK            ON

        Default behavior of checks for Infs. Checks can be activated/deactivated at runtime.
        REQUIRES: TLAPACK_NDEBUG=OFF
                  TLAPACK_ENABLE_INFCHECK=ON

    TLAPACK_DEFAULT_NANCHECK            ON

        Default behavior of checks for NaNs. Checks can be activated/deactivated at runtime.
        REQUIRES: TLAPACK_NDEBUG=OFF
                  TLAPACK_ENABLE_NANCHECK=ON

    TLAPACK_ENABLE_INFCHECK             OFF

        Enable check for Infs as specified in the documentation of each routine.
        REQUIRES: TLAPACK_NDEBUG=OFF

    TLAPACK_ENABLE_NANCHECK             OFF

        Enable check for NaNs as specified in the documentation of each routine.
        REQUIRES: TLAPACK_NDEBUG=OFF

    TLAPACK_INT_T                       int64_t

        Type of all non size-related integers in libtlapack_c, libtlapack_cblas, libtlapack_fortran, and in the routines of the legacy API. It is the type
        used for the array increments, e.g., incx and incy.
        Supported types:
            int, short, long, long long, int8_t, int16_t,
            int32_t, int64_t, int_least8_t, int_least16_t,
            int_least32_t, int_least64_t, int_fast8_t,
            int_fast16_t, int_fast32_t, int_fast64_t,
            intmax_t, intptr_t, ptrdiff_t
        NOTE: TLAPACK_INT_T=int64_t if TLAPACK_USE_LAPACKPP=ON

    TLAPACK_NDEBUG                      OFF

        Disable all error checks.

    TLAPACK_SIZE_T                      size_t

        Type of all size-related integers in libtlapack_c, libtlapack_cblas, libtlapack_fortran, and in the routines of the legacy API.
        Supported types:
            int, short, long, long long, int8_t, int16_t,
            int32_t, int64_t, int_least8_t, int_least16_t,
            int_least32_t, int_least64_t, int_fast8_t,
            int_fast16_t, int_fast32_t, int_fast64_t,
            intmax_t, intptr_t, ptrdiff_t,
            size_t, uint8_t, uint16_t, uint32_t, uint64_t
        NOTE: TLAPACK_SIZE_T=int64_t if TLAPACK_USE_LAPACKPP=ON

    TLAPACK_USE_LAPACKPP               OFF

        Use LAPACK++ wrappers to link with optimized BLAS and LAPACK libraries.
        Mind that LAPACK++ needs BLAS++.
        Branches compatible with <T>LAPACK:
            https://bitbucket.org/weslleyspereira/blaspp/branch/tlapack
            https://bitbucket.org/weslleyspereira/lapackpp/branch/tlapack

## Dependencies on other projects

\<T\>LAPACK currently depends on the following projects:

| Project                            | Version       | When                                                 |
| ---------------------------------- | ------------- | ---------------------------------------------------- |
| CMake                              | >= v3.14      | Always                                               |
| ClangFormat                        | 10            | Always                                               |
| Catch2                             | >= 3.0.1      | `BUILD_TESTING=ON`                                   |
| Git                                | -             | `BUILD_BLASPP_TESTS=ON  OR  BUILD_LAPACKPP_TESTS=ON` |
| github.com/icl-utk-edu/testsweeper | >= 2021.04.00 | `BUILD_BLASPP_TESTS=ON  OR  BUILD_LAPACKPP_TESTS=ON` |
| LAPACK                             | >= 3.9.0      | `BUILD_BLASPP_TESTS=ON  OR  BUILD_LAPACKPP_TESTS=ON` |
| ICL BLAS++ test suite              | 2023.06.00    | `BUILD_BLASPP_TESTS=ON`                              |
| ICL LAPACK++ test suite            | 2022.07.00    | `BUILD_LAPACKPP_TESTS=ON`                            |
| LAPACKE                            | >= 3.9.0      | `BUILD_LAPACKPP_TESTS=ON`                            |
| github.com/tlapack/testBLAS        | Latest        | `BUILD_testBLAS_TESTS=ON`                            |

Note that:

- The CMake build system is prepared to download Catch2 and testsweeper whenever they are not available in the system.
- ICL BLAS++ and LAPACK++ test suites and testBLAS are Git submodules of \<T\>LAPACK. The are downloaded and built when the corresponding options are set to `ON`.

\<T\>LAPACK may also use the following projects when available:

| Project       | Version tested   | When                                         |
| ------------- | ---------------- | -------------------------------------------- |
| ICL BLAS++    | 2023.11.05       | `TLAPACK_USE_LAPACKPP=ON`                    |
| ICL LAPACK++  | 2023.11.05       | `TLAPACK_USE_LAPACKPP=ON`                    |
| kokkos mdspan | 0.4.0            | `TLAPACK_TEST_MDSPAN=ON` or in some examples |
| Eigen         | commit: 2873916f | `TLAPACK_TEST_EIGEN=ON` or in some examples  |
| GNU MPFR C++  | Latest in APT    | `TLAPACK_TEST_MPFR=ON` or in some examples   |
| GNU libquad   | Latest in APT    | `TLAPACK_TEST_QUAD=ON`                       |
| StarPU        | 1.4.1            | Running StarPU examples                      |

We also continuously test \<T\>LAPACK with optimized BLAS and LAPACK implementations: OpenBLAS, Intel MKL, Flame BLIS, LAPACK, Netlib BLAS.

> [!NOTE]
> \<T\>LAPACK supports Eigen v3.4.0 after applying the patch [9210e71f](https://gitlab.com/libeigen/eigen/-/commit/9210e71fb378a0f1542272506dc2759b6c147237). This commit fixes an ADL conflict between `Eigen::internal::size()` and `std::size()`.

## Documentation

The documentation of \<T\>LAPACK is generated using Doxygen. The documentation is available online at https://tlapack.github.io/tlapack. Alternatively, you can generate the documentation in your local machine. To do so, follow the steps below:

- Install Doxygen in your local machine. See the [Doxygen installation page](https://www.doxygen.nl/download.html) for more details.

- In the top directory of \<T\>LAPACK, run `doxygen docs/Doxyfile` to generate the \<T\>LAPACK documentation via Doxygen in your local machine.

Additional information about the software can be found in the [Wiki pages of the project](https://github.com/tlapack/tlapack/wiki).

## Contributing

Please read [CONTRIBUTING.md](https://github.com/tlapack/tlapack/blob/master/CONTRIBUTING.md) for details on how to contribute to \<T\>LAPACK.

## Testing

\<T\>LAPACK is continuously tested on Ubuntu, MacOS and Windows using GNU compilers. See the latest test results in the [Github Actions](https://github.com/tlapack/tlapack/actions) webpage for \<T\>LAPACK. The tests split into three categories:

- Test routines in [test/src](test/src) using

  1. various precision types: `float`, `double`, `std::complex<float>`, `std::complex<double>` and `Eigen::half`.

  2. various matrix and vector data structures: `tlapack::LegacyMatrix`, `Eigen::Matrix` and `std::experimental::mdspan` (the latter from `https://github.com/kokkos/mdspan`).

- Tests from [testBLAS](https://github.com/tlapack/testBLAS) for good conformance with BLAS standards.

- [BLAS++ testers](https://github.com/icl-utk-edu/blaspp/tree/master/test) and [LAPACK++ testers](https://github.com/icl-utk-edu/lapackpp/tree/master/test) for measuring performance and accuracy compared to LAPACKE.

To test \<T\>LAPACK, build with `BUILD_TESTING=ON`. Then, run `ctest` inside the build directory.

## License

BSD 3-Clause License

Copyright (c) 2021-2023, University of Colorado Denver. All rights reserved.

Copyright (c) 2017-2021, University of Tennessee. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## References

<a id="1">[1]</a> Higham, N. J. (2002). Accuracy and stability of numerical algorithms. Society for industrial and applied mathematics.
