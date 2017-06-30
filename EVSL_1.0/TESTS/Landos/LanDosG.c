#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
#include "evsl.h"
#include "io.h"

/*-------------------- protos */
int exDOS(double *vals, int n, int npts, double *x, double *y, double *intv);
int read_coo_MM(const char *matfile, int idxin, int idxout, cooMat *Acoo);
int get_matrix_info(FILE *fmat, io_t *pio);
int findarg(const char *argname, ARG_TYPE type, void *val, int argc,
            char **argv);

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
/**
 * Reads in a vector as an nx1 matrix.
 *
 * @parm[out] npts pointer to an int to store # of points
 * @parm[out] vec UNallocated space to read vector to
 * @parm[in] filename file to read from, where the first line contains number
 * of elements/width/height of matrix, and the rest of the lines contain the
 * values. */
int readVec(const char *filename, int *npts, double **vec) {
  int i;
  FILE *ifp = fopen(filename, "r");
  fscanf(ifp, "%i", npts);
  *vec = (double *)malloc(sizeof(double) * *npts);
  for (i = 0; i < (*npts); i++) {
    fscanf(ifp, "%lf", (&(*vec)[i]));
  }
  fclose(ifp);
  return 0;
}

/**
 *-----------------------------------------------------------------------
 * Tests landosG.c , the Lanczos DOS computed for the general eigenvalue
 * problem. Includes graphical comparison of calculated vs exact DOS
 *
 * use -graph_exact_dos 1 to enable graphing the exact DOS
 *-----------------------------------------------------------------------
 */
int main(int argc, char *argv[]) {
  srand(time(NULL));
  const int msteps = 30;   /* Number of steps */
  const int degB = 40;     /* Degree to aproximate B with */
  const int npts = 200;    /* Number of points */
  const int nvec = 30;     /* Number of random vectors to use */
  const double tau = 1e-4; /* Tolerance in polynomial approximation */
  /* ---------------- Intervals of interest
     intv[0] and intv[1] are the smallest and largest eigenvalues of (A,B)
     intv[2] and intv[3] are the input interval of interest [a. b]
     intv[4] and intv[5] are the smallest and largest eigenvalues of B after
     diagonal scaling */
  double intv[6] = {-2.739543872224533e-13,
                    0.0325,
                    -2.739543872224533e-13,
                    0.0325,
                    0.5479,
                    2.5000};
  int n = 0, i, nslices, ierr;
  double a, b;

  cooMat Acoo, Bcoo; /* A, B */
  csrMat Acsr, Bcsr; /* A, B */
  double *sqrtdiag = NULL;

  FILE *flog = stdout, *fmat = NULL;
  FILE *fstats = NULL;
  io_t io;
  int numat, mat;
  char line[MAX_LINE];
  int graph_exact_dos_tmp = 0;
  findarg("graph_exact_dos", INT, &graph_exact_dos_tmp, argc, argv);
  const int graph_exact_dos = graph_exact_dos_tmp;
  /*-------------------- start EVSL */
  EVSLStart();
  /*------------------ file "matfile" contains paths to matrices */
  if (NULL == (fmat = fopen("matfile", "r"))) {
    fprintf(flog, "Can't open matfile...\n");
    exit(2);
  } else {
    printf("Read matfile \n");
  }
  /*-------------------- read number of matrices ..*/
  memset(line, 0, MAX_LINE);
  if (NULL == fgets(line, MAX_LINE, fmat)) {
    fprintf(flog, "error in reading matfile...\n");
    exit(2);
  }
  if ((numat = atoi(line)) <= 0) {
    fprintf(flog, "Invalid count of matrices...\n");
    exit(3);
  }
  for (mat = 1; mat <= numat; mat++) {
    if (get_matrix_info(fmat, &io) != 0) {
      fprintf(flog, "Invalid format in matfile ...\n");
      exit(5);
    }
    /*----------------input matrix and interval information -*/
    fprintf(flog, "MATRIX A: %s...\n", io.MatNam1);
    fprintf(flog, "MATRIX B: %s...\n", io.MatNam2);
    a = io.a; /* left endpoint of input interval */
    b = io.b; /* right endpoint of input interval */
    nslices = io.n_intv;
    char path[1024]; /* path to write the output files */
    strcpy(path, "OUT/LanDosG_");
    strcat(path, io.MatNam1);
    strcat(path, ".log");
    fstats = fopen(path, "w"); /* write all the output to the file io.MatNam */
    if (!fstats) {
      printf(" failed in opening output file in OUT/\n");
      fstats = stdout;
    }
    fprintf(fstats, "MATRIX A: %s...\n", io.MatNam1);
    fprintf(fstats, "MATRIX B: %s...\n", io.MatNam2);
    fprintf(fstats,
            "Partition the interval of interest [%f,%f] into %d slices\n", a, b,
            nslices);
    /*-------------------- Read matrix - case: COO/MatrixMarket formats */
    if (io.Fmt > HB) {
      ierr = read_coo_MM(io.Fname1, 1, 0, &Acoo);
      if (ierr == 0) {
        fprintf(fstats, "matrix read successfully\n");
        /* nnz = Acoo.nnz; */
        n = Acoo.nrows;
        if (n <= 0) {
          fprintf(stderr, "non-positive number of rows");
          exit(7);
        }

        /* printf("size of A is %d\n", n);
           printf("nnz of  A is %d\n", nnz); */
      } else {
        fprintf(flog, "read_coo error for A = %d\n", ierr);
        exit(6);
      }
      ierr = read_coo_MM(io.Fname2, 1, 0, &Bcoo);
      if (ierr == 0) {
        fprintf(fstats, "matrix read successfully\n");
        if (Bcoo.nrows != n) {
          return 1;
        }
      } else {
        fprintf(flog, "read_coo error for B = %d\n", ierr);
        exit(6);
      }
      /*------------------ diagonal scaling for Acoo and Bcoo */
      sqrtdiag = (double *)calloc(n, sizeof(double));
      extractDiag(&Bcoo, sqrtdiag);
      diagScaling(&Acoo, &Bcoo, sqrtdiag);
      /*-------------------- conversion from COO to CSR format */
      ierr = cooMat_to_csrMat(0, &Acoo, &Acsr);
      if (ierr) {
        fprintf(flog, "Could not convert matrix to csr: %i", ierr);
        exit(8);
      }
      ierr = cooMat_to_csrMat(0, &Bcoo, &Bcsr);
      if (ierr) {
        fprintf(flog, "Could not convert matrix to csr: %i", ierr);
        exit(9);
      }
    }
    if (io.Fmt == HB) {
      fprintf(flog, "HB FORMAT  not supported (yet) * \n");
      exit(7);
    }
    /* Finish with input */

    /* Start with actual driver */
    if (1) {
      /*----------------  compute the range of the spectrum of B */
      SetStdEig();
      SetAMatrix(&Bcsr);
      double *vinit = (double *)malloc(n * sizeof(double));
      rand_double(n, vinit);
      double lmin = 0.0, lmax = 0.0;
      ierr = LanTrbounds(50, 200, 1e-8, vinit, 1, &lmin, &lmax, fstats);
      SetGenEig();
      if (ierr) {
        fprintf(flog, "Could not run LanTrBounds: %i", ierr);
        exit(10);
      }
      /*------------- get the bounds for B ------*/
      intv[4] = lmin;
      intv[5] = lmax;
      /*-------------------- set the left-hand side matrix A */
      SetAMatrix(&Acsr);
      /*-------------------- set the right-hand side matrix B */
      SetBMatrix(&Bcsr);
      /*-------------------- Use polynomial to solve B */
      BSolDataPol Bsol;
      Bsol.intv[0] = lmin;
      Bsol.intv[1] = lmax;
      set_pol_def(&Bsol.pol_sol);
      (Bsol.pol_sol).max_deg = degB;
      SetupBSolPol(&Bcsr, &Bsol);
      SetBSol(BSolPol, (void *)&Bsol);
      SetGenEig();
      rand_double(n, vinit);
      ierr = LanTrbounds(40, 200, 1e-10, vinit, 1, &lmin, &lmax, fstats);
      if (ierr) {
        fprintf(flog, "Could not run LanTrBounds: %i", ierr);
        exit(10);
      }
      free(vinit);
      FreeBSolPolData(&Bsol);
      /*----------------- get the bounds for (A, B) ---------*/
      intv[0] = lmin;
      intv[1] = lmax;
      /*----------------- plotting the DOS on [a, b] ---------*/
      intv[2] = lmin;
      intv[3] = lmax;
    } else {
      /*-------------------- set the left-hand side matrix A */
      SetAMatrix(&Acsr);
      /*-------------------- set the right-hand side matrix B */
      SetBMatrix(&Bcsr);
      SetGenEig();
    }
    /*-------------------- Read in the eigenvalues */
    double *ev;
    int numev;
    readVec("NM1AB_eigenvalues.dat", &numev, &ev);

    int ret;
    double neig;
    double *xHist;
    double *yHist;
    /*-------------------- exact histogram and computed DOS */
    if (graph_exact_dos) {
      xHist = (double *)malloc(npts * sizeof(double));
      yHist = (double *)malloc(npts * sizeof(double));
    }
    double *xdos = (double *)malloc(npts * sizeof(double));
    double *ydos = (double *)malloc(npts * sizeof(double));

    double t0 = cheblan_timer();
    /* ------------------- Calculate the computed DOS */
    ret = LanDosG(nvec, msteps, degB, npts, xdos, ydos, &neig, intv, tau);
    double t1 = cheblan_timer();
    fprintf(stdout, " LanDos ret %d  in %0.04fs\n", ret, t1 - t0);

    /* -------------------- Calculate the exact DOS */
    if (graph_exact_dos) {
      ret = exDOS(ev, numev, npts, xHist, yHist, intv);
      fprintf(stdout, " exDOS ret %d \n", ret);
    }

    free_coo(&Acoo);
    free_coo(&Bcoo);
    free_csr(&Acsr);
    free_csr(&Bcsr);

    /*--------------------Make OUT dir if it doesn't exist */
    struct stat st = {0};
    if (stat("OUT", &st) == -1) {
      mkdir("OUT", 0750);
    }

    /*-------------------- Write to  output files */
    char computed_path[1024];
    strcpy(computed_path, "OUT/LanDosG_Approx_DOS_");
    strcat(computed_path, io.MatNam1);
    FILE *ofp = fopen(computed_path, "w");
    for (i = 0; i < npts; i++)
      fprintf(ofp, " %10.4f  %10.4f\n", xdos[i], ydos[i]);
    fclose(ofp);
    printf("Wrote to:%s \n", computed_path);

    if (graph_exact_dos) {
      /*-------------------- save exact DOS */
      strcpy(path, "OUT/LanDosG_Exact_DOS_");
      strcat(path, io.MatNam1);
      ofp = fopen(path, "w");
      for (i = 0; i < npts; i++)
        fprintf(ofp, " %10.4f  %10.4f\n", xHist[i], yHist[i]);
      fclose(ofp);
    }

    printf("The data output is located in  OUT/ \n");
    struct utsname buffer;
    errno = 0;
    if (uname(&buffer) != 0) {
      perror("uname");
      exit(EXIT_FAILURE);
    }

    /*-------------------- invoke gnuplot for plotting ... */
    char command[1024];
    strcpy(command, "gnuplot ");
    strcat(command, " -e \"filename='");
    strcat(command, computed_path);

    if (graph_exact_dos) {
      strcat(command, "';exact_dos='");
      strcat(command, path);
      strcat(command, "'\" testerG_ex.gnuplot");
      ierr = system(command);
    } else {
      strcat(command, "'\" testerG.gnuplot");
      ierr = system(command);
    }

    if (ierr) {
      fprintf(stderr,
              "Error using 'gnuplot < testerG.gnuplot', \n"
              "postscript plot could not be generated \n");
    } else {
      printf("A postscript graph has been placed in %s%s\n", computed_path,
             ".eps");
      /*-------------------- and gv for visualizing / */
      if (!strcmp(buffer.sysname, "Linux")) {
        strcpy(command, "gv ");
        strcat(command, computed_path);
        strcat(command, ".eps &");
        ierr = system(command);
        if (ierr) {
          fprintf(stderr, "Error using 'gv %s' \n", command);
          printf(
              "To view the postscript graph use a postcript viewer such as  "
              "gv \n");
        }
      } else {
        printf(
            "To view the postscript graph use a postcript viewer such as  "
            "gv \n");
      }
    }
    if (graph_exact_dos) {
      free(xHist);
      free(yHist);
    }
    free(xdos);
    free(ydos);
    free(ev);
    fclose(fstats);
    if (sqrtdiag) free(sqrtdiag);
  }
  fclose(fmat);
  EVSLFinish();
  return 0;
}
