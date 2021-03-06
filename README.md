

## EVSL:  EigenValues Slicing Library (Version 1.1.0)         

 ![Alt](evsl_logo3.png "EVSL-logo")

~~~
                          ChebLanNr, ChebLanTr, ChebSI, RatLanNr and RatLanTr 
               Polynomial and Rational Filtered Lanczos and subspace iteration algorithms 
                                 For Symmetric Eigenvalue problems
~~~

Welcome to EVSL. EVSL is a  C library for computing the eigenvalues of
a  symmetric  matrix that  are  located  in  a given  interval.   EVSL
implements  a polynomial  filtered Lanczos  method (thick  restart, no
restart)  a  rational  filtered  Lanczos  method  (thick  restart,  no
restart),  and a  polynomial filtered  subspace iteration  for solving
standard  eigenvalue  problems  A  u  =  &lambda;  u  and  generalized
eigenvalue problems A u = &lambda;  B u.  EVSL also provides tools for
spectrum slicing, i.e., the technique  of subdividing a given interval
into  p smaller  subintervals and  computing the  eigenvalues in  each
subinterval independently. This is achieved by estimating the spectral
density of the  matrix (or matrix pair for  generalized problems) with
either the Kernel  Polynomial method (KPM) or a  Lanczos based method.
This release does not yet offer full parallel implementations (trivial
openMP test programs are available  among the test drivers).  Versions
1.1.x  have much  added functionality  relative to  version 1.0.  Most
notably:

+ EVSL now handles both standard and generalized problems
+ Matrix-free mechanism  [whereby user provides own matvec function]
+ Interface for FORTRAN users
+ Spectrum slicing by KPM as well as Lanczos. Also effective
   spectrum slicing for Ax=&lambda; Bx
+ Interfaces to various direct solvers that are needed by the code
+ Recently added: Generalized eigenvalue problems without factorizations
  [using polynomials in B]

For questions/feedback send e-mail to Yousef Saad [saad@umn.edu]

-----------------------------------------------------------------------

###  HANDBOOK AND DOCUMENTATION

-----------------------------------------------------------------------

An article describing EVSL is available from
[arXiv](https://arxiv.org/abs/1802.05215), which 
can be cited as follows [bibtex entry] 
```
@article{LiEigenvaluesSlicingLibrary2018a,
  archivePrefix = {arXiv},
  eprinttype = {arxiv},
  eprint = {1802.05215},
  primaryClass = {math},
  title = {The {{Eigenvalues Slicing Library}} ({{EVSL}}): {{Algorithms}}, {{Implementation}}, and {{Software}}},
  url = {http://arxiv.org/abs/1802.05215},
  shorttitle = {The {{Eigenvalues Slicing Library}} ({{EVSL}})},
  date = {2018-02-14},
  keywords = {Mathematics - Numerical Analysis},
  author = {Li, Ruipeng and Xi, Yuanzhe and Erlandson, Lucas and Saad, Yousef},
}
```

A detailed documentation of the code can be found in the `Documentation'
folder of the package in EVSL_1.1.x 

-----------------------------------------------------------------------

###  INSTALLATION

-----------------------------------------------------------------------

**Library**: The user only needs to modify the file makefile.in [see makefile.in.example for samples 
  of files makefile.in that are given for mac-os and for Linux].

    cp makefile.in_Linux/MacOS.example makefile.in. 
    modify makefile.in [provide C compiler and BLAS/LAPACK path]
    make clean; make

**Test programs**:
   In the directories under TESTS/, you will find a number of
   directories which contain makefiles to 
   build sample drivers that test a few different situations.

**CXSparse**
   CXSparse is the default direct linear solver used in EVSL for the 
   linear systems that arise in rational filtering methods and    in 
   generalized eigenvalue problems. CXSparse is included only to allow quick 
   testing. However,  be aware that it is significantly slower than other available
    direct solvers such as those in SuiteSparse  for example  (see below).

   NOTE: The compiler used to compile CXSparse is specified in the makefile
   at EXTERNAL/CXSparse/Lib/Makefile.

>  NOTE: CXSparse, which is  distributed with EVSL, is Copyrighted by
>  Timothy Davis. As noted above much better performance can be
>  achieved by other existing direct solvers. 
>  Refer to CXSparse package for its License. [http://faculty.cse.tamu.edu/davis/suitesparse.html]

**SuiteSparse**:
   As a replacement for CXSparse, bindings are provided for the package SuiteSparse.
   Once SuiteSparse is installed, simply switch the DIRECTSOL variable
   in the makefile.in, and add the path to
   EXTERNAL/makefile_suitesparse.in.

   EVSL invokes SuiteSparse to solve linear systems with (A-SIGMA I) or (A-SIGMA B),
   and  CHOLMOD for  solving linear systems with B.

   Other solvers can also be used by providing the same interface as done for SuiteSparse.
   Follow the examples implemented in EXTERNAL/evsl_suitesparse.c
 
>  NOTE:  SuiteSparse is NOT distributed with EVSL, and it is Copyrighted by Timothy Davis.  
>  Refer to the SuiteSparse package for its license. [http://faculty.cse.tamu.edu/davis/suitesparse.html]

**Pardiso**
  Pardiso bindings are provided. The Pardiso web-site can be accessed at
  [https://pardiso-project.org/]

-----------------------------------------------------------------------

###  LINKING  WITH  UMFPACK (SuiteSparse 4.5.3)

-----------------------------------------------------------------------
  UMFPACK and CHOLMOD requires AMD, COLAMD, CCOLAMD
  and  CAMD,  and  optionally  METIS 5.1.0.   Compile  each  of  these
  packages  to  have  the  library  file in  the  Lib  directory.   If
  SuiteSparse  is configured  with METIS,  give the  path to  METIS (v
  5.1.0)  as  well  to  make  libmetis.a,  in metis-5.1.0/ type

      make  config; make

  Please  refer to SuiteSparse and METIS for installation details.

-----------------------------------------------------------------------

  Bindings with Pardiso as a direct solver are also provided -- see the directory EXTERNAL 
  for details.

-----------------------------------------------------------------------

###  RATIONAL FILTERING

-----------------------------------------------------------------------
  Rational  filtering  requires  solving  linear  systems  (where  the
  coefficient matrix  is the  original matrix  shifted by  a **complex**
  shift).  A linear solver routine  must be provided.  

  After  having  computed  the  rational  filter  by

      find_ratf(intv, &rat),

  users  can call

      SetASigmaBSol(&rat, func, allf, data)

  to set the solver functions and associated data for all the poles of 
  the rational filter.
  `func` is an array of function pointers of  length num of  poles, i.e.,  `rat->num`. 
  So, `func[i]`  is the  function  to solve  the systems  with  pole `i`,  the
  coefficient matrix of  which is `A - s_i I(or, B)`,  where `s_i = rat->zk[i]`
  is the  complex shift.  `data`  is an array  of `(void*)` of  the same
  length,  where `data[i]`  is the  data needed  by `func[i]`.   

  Each "func[i]" must be of the following prototype

      void SolFuncC(int n, double *br, double *bz, double *xr, double *xz, void *data);

  where `n`  is the size  of the system,  `br`, `bz` are  the right-hand
  side (real and  imaginary parts of complex vector),  `xr`, `xz` will
  hold the  solution (complex vector) on return,  and `data` contains  all the
  data  needed  for  the  solver.    

  If all `func[i]` are the same, one can set `func==NULL` and set `allf` to the function   

  Once `SetASigmaBSol` is executed, rational filtering Lanczos methods 
  should be ready to use.
  
-----------------------------------------------------------------------

###  MATRIX-FREE EIGEN-SOLVERS

-----------------------------------------------------------------------
  All the eigensolvers  in EVSL can be used in  `matrix-free' mode. In
  this  mode,  users need  only  to  provide their  own  matrix-vector
  product function of the following prototype:

      void MVFunc(double *x, double *y, void *data);

  where y  = A *  x and data  is the pointer  to the associated  data to
  perform the  matvec. The  `(void *)`  argument is  to provide  a uniform
  interface  to all  user-specific  functions. For  a particular  Matvec
  function, one can pack all data  needed by this function into a struct
  and pass the  pointer of this struct  to EVSL (after casting  it to `(void
  *)`). This  function needs to  be passed to EVSL  as well, so  EVSL can
  call  it  to  perform  all matvecs.    The user can also pass needed
  matrices in the standard CSR format to EVSL, in which case EVSL will use 
  its internal   MATVEC routine. This can be set by

      SetAMatrix(csrMat *A)
      SetBMatrix(csrMat *B)

  for the matrices A and B 

  
  In  TESTS/LAP,  an example  of  matvec  functions for  2D/3D  Laplacian
  matrices is  provided, where the  matrix is not explicitly  formed but
  5pt/7pt stencil is used instead to perform the matvecs. 
  In  this example, a struct for matvec is first defined:

      typedef struct _lapmv_t {  
        int  nx,  ny,  nz; 
        double  *stencil;  
      } lapmv_t;

  and the matvec function is implemented as: 

      void Lap2D3DMatvec(double  *x, double  *y,  void  *data) {  
        lapmv_t *lapmv = (lapmv_t *) data; 
        int nx = lapmv->nx; 
        int ny = lapmv->ny;
        int nz = lapmv->nz; 
        double *stencil = lapmv->stencil; 
        ...  
      }

  in  which   the  pointer  is  first   cast  and  all  the   data  is
  unpacked. Once these are ready, they can be passed to EVSL by calling    

      SetAMatvec(n, &Lap2D3DMatvec, (void*) &lapmv) and
      SetBMatvec(n, &Lap2D3DMatvec, (void*) &lapmv)

  to set the matvec routines for A and B respectively,
  where the first input is the size of the "matrix", the second input is
  the  function pointer  and the  third one  is the  data pointer.  Once
  `SetMatvecFunc`  is  called,  EVSL  will  use  the  registered  matvec
  function to perform all matvecs with A.

  Users should first create a function  wrapper of the above type for an
  external matvec routine. Then, following  the steps in the example, it
  will be straightforward to use it in EVSL.
  
-----------------------------------------------------------------------

###  GENERALIZED EIGENVALUE PROBLEMS

-----------------------------------------------------------------------
  For solving the generalized eigenproblem A x = &lambda; B  x, the users 
  must also provide a solver  for the B matrix by calling

      SetBSol(SolFuncR func, void *data).

  To indicate to EVSL that a generalized eigenvalue problem is being solve, 
  one must call

      SetGenEig()

  since by default, EVSL assumes that a  standard eigenvalue problem is being
  solved even when a B matrix is provided. Call the function

      SetStdEig()

  for solving standard eigenvalue problem

  To implement spectrum slicing, the current version of EVSL requires system
  solutions with the matrix L', where B=L*L' is the Cholesky factorization of B. 
  Call the function

    SetLTSol(SolFuncR func, void *data)

  to set the solver function for L'.

 Version  1.1.1   includes  drivers  for  solving   certain  types  of
 generalized eigenvalue  problems by  polynomial filtering  by methods
 that avoid matrix factorizations.

-----------------------------------------------------------------------

###  Initialization and Completion

-----------------------------------------------------------------------
* Use `EVSLStart()` and `EVSLFinish()` before and after any call to the EVSL functions 



-----------------------------------------------------------------------    

### DETAILED DESCRIPTION OF CONTENTS

-----------------------------------------------------------------------

 * INC
   - evsl.h           : user-level function prototypes and constant definitions
   - blaslapack.h     : C API for BLAS/LAPACK functions used in evsl
   - def.h            : miscellaneous macros 
   - struct.h         : miscellaneous structs used in evsl
   - internal_proto.h : internal function prototypes for SRC/
    
 * SRC
   - cheblanNr.c   :  Polynomial Filtered no-restart Lanczos
   - cheblanTr.c   :  Polynomial Filtered thick restart Lanczos
   - chebpoly.c    :  Computing and applying polynomial filters
   - chebsi.c      :  Polynomial Filtered Subspace iteration
   - dumps.c       :  Miscellaneous functions for I/O and for debugging 
   - dos_utils.c   :  Miscellaneous functions used for DOS based functions.
   - evsl.c        :  Set EVSL solver options and data
   - lanbounds.c   :  Lanczos alg. to compute bounds of spectrum
   - landos.c      :  Lanczos based DOS algorithm for the standard problem
   - landosG.c     :  Lanczos based DOS algorithm for generalized and standard problems
   - lanTrbounds.c :  A more robust alg. to compute bounds of spectrum based on TR Lanczos
   - mactime.c     :  Timer for mac iOS
   - misc_la.c     :  Miscellaneous linear algebra functions
   - ratfilter.c   :  Computing and applying rational filters
   - ratlanNr.c    :  Rational Filtered no-restart Lanczos
   - ratlanTr.c    :  Rational Filtered thick restart Lanczos
   - spmat.c       :  Sparse matrix routines
   - spslice.c    :  Spectrum slicing functions for Kernel Polynomial Method
   - spslice2.c   :  Spectrum slicing functions for Lanczos 
   - timing.c      :  Timer
   - vect.c        :  Vector operations

 * libevsl.a       : library

 * TESTS/          : Test drivers

   * Fortran/   : Fortran test drivers

   * PLanR         : Polynomial Filtered Thick Restart Lanczos test drivers
     * LapPLanR.c         : Polynomial filtering T-R Lanczos (Lapliacian)
     * MMPLanR.c     : Polynomial filtering T-R Lanczos (Matrix Market)
     * MMPLanR_omp.c : Polynomial filtering T-R Lanczos (parallelized with OMP for slices) (Matrix Market)

   * RLanR         : Rational Filtered Thick Restart Lanczos test drivers
     * LapRLanR.c         : Rational filtering T-R Lanczos (Laplacian)
     * MMRLanR.c     : Rational filtering T-R Lanczos (Matrix Market)

   * PLanN         : Polynomial Filtered No-Restart Lanczos test drivers
     * MMPLanN.c     : Polynomial filtering non-restart Lanczos (Matrix Market)
     * LapPLanN.c         : Polynomial filtering non-restart Lanczos (Laplacian)
     * LapPLanN_MatFree.c : "matrix-free" version: not forming matrix but passing mat-vec function (Laplacian)

   * RLanN         : Rational Filtered No-Restart Lanczos test drivers
     * LapRLanN.c         : Rational filtering non-restart Lanczos (Laplacian)
     * MMRLanN.c     : Rational filtering non-restart Lanczos (Matrix Market)

   * GEN           : Test drivers for the generalized eigenvalue problem
     * MMsimple.c    : A stripped down (no slicing) drivier based on MMPLanN.c
     * MMPLanN.c     : Polynomial filtering non-restart Lanczos (Matrix Market)
     * MMPLanR.c     : Polynomial filtering T-R Lanczos (Matrix Market) 
     * MMRLanN.c     : Rational filtering non-restart Lanczos (Matrix Market) 
     * MRLanR.c      : Rational filtering T-R Lanczos (Matrix Market) 

   * PSI           : Test drivers for polynomial filter subspace iteration
     * LapPSI.c     : Polynomial filtering subspace iterations (Laplacian)
     * MMPSI.c      : Polynomial filtering subspace iterations (Matrix Market)

   * COMMON        : Routines common to the test drivers
      * io.c        : parse command-line input parameters
      * lapl.c      : Build Laplacian matrices and compute the exact eigenvalues of Laplacians
      * mmio.c        : IO routines for the matrix market format

   * COMMON_GEN    : Routines common to the test drivers for the generalized eigenvalue problem
      * io.c        : parse command-line input parameters
      * lapl.c      : Build Laplacian matrices and compute the exact eigenvalues of Laplacians
      * mmio.c        : IO routines for the matrix market format

   * TESTS/Landos    : test drivers for the lanDOS related functions.
      * LanDos.c      : DOS for standard eigenvalue problem  using Lanczos 
      * LanDosG.c     : DOS for generalized eigenvalue problem  using Lanczos 

 * EXTERNAL             : direct solver  interface for generalized eigenvalue problems
   - evsl_suitesparse.c : suitesparse UMFPACK and CHOLMOD interface
   - evsl_cxsparse.c    : cxsparse interface
   - evsl_pardiso.c     : pardiso interface
   - evsl_direct.h      : Direct solver interface


 * FORTRAN         : Fortran interface
   - evsl_f90.c    : Fortran interface

-----------------------------------------------------------------------
