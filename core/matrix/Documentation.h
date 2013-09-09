/* This file is part of Into. 
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#error "Documentation.h is not intended for inclusion. Its sole purpose is to provide documentation for a package."

/**
 * @defgroup Matrix Matrices and linear algebra
 *
 * In Into, matrices are used to store many types of data, including
 * matrices and vectors in plain mathematical sense, images, feature
 * vectors, geometrical objects such as rectangles and circles etc.
 * The matrix library contains the basic matrix classes, matrix
 * arithmetic routines, and linear algebra algorithms.
 *
 * @par Lazy evaluation
 *
 * The matrix library aims at a good balance between performance and
 * usability. Although the library itself contains some hard-to-digest
 * template code, the programmer seldom has the need to deal with the
 * internals. The library uses template meta-programming techniques to
 * optimize matrix evaluations at compile time. Consider the following
 * example:
 *
 * @code
 * PiiMatrix<double> A(3,3);
 * A = A * 5 + 3; 
 * @endcode
 *
 * The textbook approach to make this work - which is still in use in
 * many libraries - is to implement @p operator* and @p operator+
 * functions that take a matrix and a scalar as parameters and return
 * a matrix. This is equivalent to the following:
 *
 * @code
 * PiiMatrix<double> A(3,3);
 * PiiMatrix<double> tmp1 = A * 5;
 * PiiMatrix<double> tmp2 = tmp1 + 3;
 * A = tmp2;
 * @endcode
 *
 * In Into, no temporary matrices need to be created. Instead, the
 * operator functions return @ref PiiConceptualMatrix "matrix concepts"
 * that are evaluated only when needed. The following
 * pseudocode shows how this is done:
 *
 * @code
 * A * 5 -> MatrixConcept<multiplies, Matrix>
 * MatrixConcept<multiplies, Matrix> + 3 -> MatrixConcept<add, MatrixConcept<multiplies, Matrix>>
 * A = MatrixConcept<add, MatrixConcept<multiplies, Matrix>> -> Matrix
 * @endcode
 *
 * The arithmetic operations never actually evaluate the expression
 * but return a conceptual matrix that performs the evaluation <em>if
 * requested to do so</em>. In this example, the assignment at the end
 * does. This kind of lazy evaluation is used not only by arithmetic
 * operators but also with algorithms such as @ref Pii::transpose()
 * and many functions that work with matrices. The following code
 * doesn't create a single temporary object:
 *
 * @code
 * PiiMatrix<int> A(5,5);
 * A += pow(abs(A) - 3, 2) + transpose(A);
 * @endcode
 *
 *
 * @par Sub-matrices
 *
 * One of the most important implementations of the matrix concept is
 * PiiSubmatrix, a matrix that references a sub-matrix inside another
 * matrix. Sub-matrices make it possible to avoid unnecessary copies,
 * and they are extensively used in Into from low-level algoritms to
 * high-level operations such as PiiImageSplitter.
 *
 *
 *
 * @par Linear algebra
 *
 * The matrix library contains independent, pure C++ implementations
 * of many standard linear algebra algoritms. The main design
 * principles for the algorithms have been numerical stability and
 * code maintainability (which also means readability). The algoritms
 * try to make optimal usage of CPU cache (cache-oblivious
 * algorithms), which makes them also relatively fast. Many algoritms
 * are modeled after their LAPACK and Alglib counterparts, although
 * they have been completely rewritten.
 *
 * The most important functions of the linear algebra libary are
 *
 * @li @ref Pii::householderTransform() "Householder transform"
 * @li @ref Pii::qrDecompose() "QR decomposition"
 * @li @ref Pii::luDecompose() "LU decomposition"
 * @li @ref Pii::svDecompose() "Singular value decomposition"
 */
