#include <petsc/private/ftnimpl.h>

/*MC
   PetscFortranAddr - a variable type in Fortran that can hold a
     regular C pointer.

   Note:
    Used, for example, as the file argument in `PetscFOpen()`

   Level: beginner

.seealso:  `PetscOffset`, `PetscInt`
M*/
/*MC
   PetscOffset - a variable type in Fortran used with `VecGetArray()`
     and `ISGetIndices()`

   Level: beginner

.seealso:  `PetscFortranAddr`, `PetscInt`
M*/

/*
    This is code for translating PETSc memory addresses to integer offsets
    for Fortran.
*/
char *PETSC_NULL_CHARACTER_Fortran       = NULL;
void *PETSC_NULL_INTEGER_Fortran         = NULL;
void *PETSC_NULL_SCALAR_Fortran          = NULL;
void *PETSC_NULL_DOUBLE_Fortran          = NULL;
void *PETSC_NULL_REAL_Fortran            = NULL;
void *PETSC_NULL_BOOL_Fortran            = NULL;
void *PETSC_NULL_ENUM_Fortran            = NULL;
void *PETSC_NULL_INTEGER_ARRAY_Fortran   = NULL;
void *PETSC_NULL_SCALAR_ARRAY_Fortran    = NULL;
void *PETSC_NULL_REAL_ARRAY_Fortran      = NULL;
void *PETSC_NULL_INTEGER_POINTER_Fortran = NULL;
void *PETSC_NULL_SCALAR_POINTER_Fortran  = NULL;
void *PETSC_NULL_REAL_POINTER_Fortran    = NULL;

EXTERN_C_BEGIN
void (*PETSC_NULL_FUNCTION_Fortran)(void) = NULL;
EXTERN_C_END
void *PETSC_NULL_MPI_COMM_Fortran = NULL;

size_t PetscIntAddressToFortran(const PetscInt *base, const PetscInt *addr)
{
  size_t tmp1 = (size_t)base, tmp2 = 0;
  size_t tmp3 = (size_t)addr;
  size_t itmp2;

#if !defined(PETSC_HAVE_CRAY90_POINTER)
  if (tmp3 > tmp1) {
    tmp2  = (tmp3 - tmp1) / sizeof(PetscInt);
    itmp2 = (size_t)tmp2;
  } else {
    tmp2  = (tmp1 - tmp3) / sizeof(PetscInt);
    itmp2 = -((size_t)tmp2);
  }
#else
  if (tmp3 > tmp1) {
    tmp2  = (tmp3 - tmp1);
    itmp2 = (size_t)tmp2;
  } else {
    tmp2  = (tmp1 - tmp3);
    itmp2 = -((size_t)tmp2);
  }
#endif

  if (base + itmp2 != addr) {
    PetscCallAbort(PETSC_COMM_SELF, (*PetscErrorPrintf)("PetscIntAddressToFortran:C and Fortran arrays are\n"));
    PetscCallAbort(PETSC_COMM_SELF, (*PetscErrorPrintf)("not commonly aligned or are too far apart to be indexed \n"));
    PetscCallAbort(PETSC_COMM_SELF, (*PetscErrorPrintf)("by an integer. Locations: C %zu Fortran %zu\n", tmp1, tmp3));
    PETSCABORT(PETSC_COMM_WORLD, PETSC_ERR_PLIB);
  }
  return itmp2;
}

PetscInt *PetscIntAddressFromFortran(const PetscInt *base, size_t addr)
{
  return (PetscInt *)(base + addr);
}

/*
       obj - PETSc object on which request is made
       base - Fortran array address
       addr - C array address
       res  - will contain offset from C to Fortran
       shift - number of bytes that prevent base and addr from being commonly aligned
       N - size of the array

       align indicates alignment relative to PetscScalar, 1 means aligned on PetscScalar, 2 means aligned on 2 PetscScalar etc
*/
PetscErrorCode PetscScalarAddressToFortran(PetscObject obj, PetscInt align, PetscScalar *base, PetscScalar *addr, PetscInt N, size_t *res)
{
  size_t   tmp1 = (size_t)base, tmp2;
  size_t   tmp3 = (size_t)addr;
  size_t   itmp2;
  PetscInt shift;

  PetscFunctionBegin;
#if !defined(PETSC_HAVE_CRAY90_POINTER)
  if (tmp3 > tmp1) { /* C is bigger than Fortran */
    tmp2  = (tmp3 - tmp1) / sizeof(PetscScalar);
    itmp2 = (size_t)tmp2;
    shift = (align * sizeof(PetscScalar) - (PetscInt)((tmp3 - tmp1) % (align * sizeof(PetscScalar)))) % (align * sizeof(PetscScalar));
  } else {
    tmp2  = (tmp1 - tmp3) / sizeof(PetscScalar);
    itmp2 = -((size_t)tmp2);
    shift = (PetscInt)((tmp1 - tmp3) % (align * sizeof(PetscScalar)));
  }
#else
  if (tmp3 > tmp1) { /* C is bigger than Fortran */
    tmp2  = (tmp3 - tmp1);
    itmp2 = (size_t)tmp2;
  } else {
    tmp2  = (tmp1 - tmp3);
    itmp2 = -((size_t)tmp2);
  }
  shift = 0;
#endif

  if (shift) {
    /*
        Fortran and C not PetscScalar aligned,recover by copying values into
        memory that is aligned with the Fortran
    */
    PetscScalar   *work;
    PetscContainer container;

    PetscCall(PetscMalloc1(N + align, &work));

    /* recompute shift for newly allocated space */
    tmp3 = (size_t)work;
    if (tmp3 > tmp1) { /* C is bigger than Fortran */
      shift = (align * sizeof(PetscScalar) - (PetscInt)((tmp3 - tmp1) % (align * sizeof(PetscScalar)))) % (align * sizeof(PetscScalar));
    } else {
      shift = (PetscInt)((tmp1 - tmp3) % (align * sizeof(PetscScalar)));
    }

    /* shift work by that number of bytes */
    work = (PetscScalar *)(((char *)work) + shift);
    PetscCall(PetscArraycpy(work, addr, N));

    /* store in the first location in addr how much you shift it */
    ((PetscInt *)addr)[0] = shift;

    PetscCall(PetscContainerCreate(PETSC_COMM_SELF, &container));
    PetscCall(PetscContainerSetPointer(container, addr));
    PetscCall(PetscObjectCompose(obj, "GetArrayPtr", (PetscObject)container));

    tmp3 = (size_t)work;
    if (tmp3 > tmp1) { /* C is bigger than Fortran */
      tmp2  = (tmp3 - tmp1) / sizeof(PetscScalar);
      itmp2 = (size_t)tmp2;
      shift = (align * sizeof(PetscScalar) - (PetscInt)((tmp3 - tmp1) % (align * sizeof(PetscScalar)))) % (align * sizeof(PetscScalar));
    } else {
      tmp2  = (tmp1 - tmp3) / sizeof(PetscScalar);
      itmp2 = -((size_t)tmp2);
      shift = (PetscInt)((tmp1 - tmp3) % (align * sizeof(PetscScalar)));
    }
    if (shift) {
      PetscCall((*PetscErrorPrintf)("PetscScalarAddressToFortran:C and Fortran arrays are\n"));
      PetscCall((*PetscErrorPrintf)("not commonly aligned.\n"));
      PetscCall((*PetscErrorPrintf)("Locations/sizeof(PetscScalar): C %g Fortran %g\n", (double)(((PetscReal)tmp3) / (PetscReal)sizeof(PetscScalar)), (double)(((PetscReal)tmp1) / (PetscReal)sizeof(PetscScalar))));
      PETSCABORT(PETSC_COMM_WORLD, PETSC_ERR_PLIB);
    }
    PetscCall(PetscInfo(obj, "Efficiency warning, copying array in XXXGetArray() due\n\
    to alignment differences between C and Fortran\n"));
  }
  *res = itmp2;
  PetscFunctionReturn(PETSC_SUCCESS);
}

/*
    obj - the PETSc object where the scalar pointer came from
    base - the Fortran array address
    addr - the Fortran offset from base
    N    - the amount of data

    lx   - the array space that is to be passed to XXXXRestoreArray()
*/
PetscErrorCode PetscScalarAddressFromFortran(PetscObject obj, PetscScalar *base, size_t addr, PetscInt N, PetscScalar **lx)
{
  PetscInt       shift;
  PetscContainer container;
  PetscScalar   *tlx;

  PetscFunctionBegin;
  PetscCall(PetscObjectQuery(obj, "GetArrayPtr", (PetscObject *)&container));
  if (container) {
    PetscCall(PetscContainerGetPointer(container, (void **)lx));
    tlx = base + addr;

    shift = *(PetscInt *)*lx;
    PetscCall(PetscArraycpy(*lx, tlx, N));
    tlx = (PetscScalar *)((char *)tlx - shift);

    PetscCall(PetscFree(tlx));
    PetscCall(PetscContainerDestroy(&container));
    PetscCall(PetscObjectCompose(obj, "GetArrayPtr", NULL));
  } else {
    *lx = base + addr;
  }
  PetscFunctionReturn(PETSC_SUCCESS);
}

#if defined(PETSC_HAVE_FORTRAN_CAPS)
  #define petscisinfornanscalar_ PETSCISINFORNANSCALAR
  #define petscisinfornanreal_   PETSCISINFORNANREAL
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE)
  #define petscisinfornanscalar_ petscisinfornanscalar
  #define petscisinfornanreal_   petscisinfornanreal
#endif

PETSC_EXTERN PetscBool petscisinfornanscalar_(PetscScalar *v)
{
  return (PetscBool)PetscIsInfOrNanScalar(*v);
}

PETSC_EXTERN PetscBool petscisinfornanreal_(PetscReal *v)
{
  return (PetscBool)PetscIsInfOrNanReal(*v);
}
